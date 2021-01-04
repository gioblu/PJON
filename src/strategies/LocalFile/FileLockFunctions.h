#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#ifdef _WIN32
#define W_OK 02
#define R_OK 04
#define X_OK 04
#include <direct.h>
#include <windows.h>
#include <io.h>
#include <sys/locking.h>
#define access _access
#define lseek _lseek
#define locking _locking
#define open _open
#define write _write
#define read _read
#define close _close
#else
#include <sys/vfs.h>
#include <sys/procfs.h>
#endif


//-----------------------------------------------------------------------------
int CheckIfDir(const char *name)
//-----------------------------------------------------------------------------
/* testing if name is a directory */
{
	struct stat s_buf;
	return ((stat(name, &s_buf) != -1) && ((s_buf.st_mode & S_IFMT) == S_IFDIR));
}


//-----------------------------------------------------------------------------
int CheckIfFile(const char *name)
//-----------------------------------------------------------------------------
/* testing if name is a file */
{
	struct stat s_buf;
	return ((stat(name, &s_buf) != -1) && ((s_buf.st_mode & S_IFMT) == S_IFREG));
}


//-----------------------------------------------------------------------------
int CheckForWrite(const char *filepath)
//-----------------------------------------------------------------------------
/* test if a file or directory is writable for current user */
{
	struct stat s_buf;
	return ((stat(filepath, &s_buf) != -1) && s_buf.st_mode && (access(filepath, W_OK) != -1));
}


//-----------------------------------------------------------------------------
int CheckForRead(const char *filepath)
//-----------------------------------------------------------------------------
/* testing if a file or directory is readable for current user */
{
	struct stat s_buf;
	return ((stat(filepath, &s_buf) != -1) && s_buf.st_mode && (access(filepath, R_OK) != -1));
}


//-----------------------------------------------------------------------------
int CheckForExe(const char *filepath)
//-----------------------------------------------------------------------------
/* testing if a file or directory is readable for current user */
{
	struct stat s_buf;
	return ((stat(filepath, &s_buf) != -1) && s_buf.st_mode && (access(filepath, X_OK) != -1));
}


//-----------------------------------------------------------------------------
int GetFileSize(const char *filepath)
//-----------------------------------------------------------------------------
/* testing if a file or directory and return the actual size of the file */
{
	struct stat s_buf;

	if ( stat( filepath, &s_buf) == -1)
		return (-1);
	else return (s_buf.st_size);
}


// Returns 1 if the section is locked, 0 otherwise. If an error occurs, the
// return value will indicate that the section is locked, to avoid possible
// access collisions or file corruptions.
// NOTE: This function will return 1 if it is not possible to get the requested
// lock because of an already existing lock. If bExclusive is 1 then this is
// straight-forward: a return value of 0 means that no locks are present.
//-----------------------------------------------------------------------------
int IsFileSectionLocked(const int iFileNo,       // the fileno
                        const int nPosition,     // start position
                        const int nSize,         // size of section to test
                        const int bExclusive)    // write or read lock
//-----------------------------------------------------------------------------
{
#ifdef _WIN32
    if (nPosition>=0) lseek(iFileNo, nPosition, SEEK_SET);
    if (locking(iFileNo, LK_NBLCK, nSize)!=0) return(1);
    locking(iFileNo, LK_UNLCK, nSize);
    return(0);
#else
    struct flock fl;
    fl.l_whence = 0;
    fl.l_start = nPosition;
    fl.l_len = nSize;
    fl.l_pid = 0;
    int fno = iFileNo;
    if (fno==-1) return(1);
    
    fl.l_type = bExclusive ? F_WRLCK : F_RDLCK;
    
    if (fcntl(fno, F_GETLK, &fl)!=0) {
        printf("IsFileSectionLocked: fcntl(F_GETLK,...) failed "
                  "with error %d (%s)\n", errno, strerror(errno));
        return(1);
    }
    return (fl.l_type != F_UNLCK);
#endif
}


//-----------------------------------------------------------------------------
int LockFileSection(const int iFileNo,       // the fileno
                    const int nPosition,     // start position
                    const int nSize,         // size of section to lock
                    const int bLock,         // 1=lock, 0=unlock
                    const int bExclusive,    // write or read lock
                    const int bNonBlocking)  // blocking or non-blocking
//-----------------------------------------------------------------------------
{
#ifdef _WIN32
    // set position
    if (nPosition>=0) lseek(iFileNo, nPosition, SEEK_SET);

    // get the windows file handle
    HANDLE hFile = (void *) _get_osfhandle(iFileNo);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("LockFileSection: get_ofshandle failed for file %d\n", iFileNo);
        return(0);
    }

    // lock or unlock
    struct _OVERLAPPED ol;
    ol.Offset = nPosition;
    ol.OffsetHigh = 0;
    ol.hEvent = 0;
    int bOK = 1;
    if (bLock) {
        // blocking or non-blocking?
        int nMode = LOCKFILE_EXCLUSIVE_LOCK;
        if (bNonBlocking) nMode |= LOCKFILE_FAIL_IMMEDIATELY;

        // Try to lock, repeat until success, an unexpected error,
        // or exceeding the max time.
        int nRetries = 0, nMaxRetries = 0, iLastErr = 0;
        if (!bNonBlocking) nMaxRetries = 1000000000;
        do {
            bOK = LockFileEx(hFile, nMode, 0, nSize, 0, &ol);
            if (!bOK) iLastErr = GetLastError();
            if (iLastErr!=ERROR_LOCK_VIOLATION) break;
            Sleep(100);
        } while (!bOK && (nMaxRetries>nRetries++));
    } else {
        // unlock
        bOK = UnlockFileEx(hFile, 0, nSize, 0, &ol);
    }

    // log error if any
    if (!bOK) {
       DWORD iLastErr = GetLastError();
       LPVOID lpMsgBuf;
       FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
           FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
           NULL, iLastErr,
           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
           (LPTSTR) &lpMsgBuf, 0, NULL);
        printf("%s of file %d returned error %d (%s)\n",
                (const char *)(bLock ? "Locking" : "Unlocking"),
                iFileNo, iLastErr, (const char*)(LPCTSTR) lpMsgBuf);
        LocalFree(lpMsgBuf);
        return(0);
    }
    return(1);
#else
    struct flock fl;
    fl.l_whence = 0;
    fl.l_start = nPosition;
    fl.l_len = nSize<0 ? 0 : nSize;
    fl.l_pid = 0;
    int fno = iFileNo;
    if (fno==-1) return(0);
    
    // Now take care to use the lock type specified. If we are trying to
    // get a write lock on a file opened for reading only, this will result
    // in a 'Bad file number' error (errno 9) on some Unix platforms.
    fl.l_type = bExclusive ? F_WRLCK : F_RDLCK;
    int retval = 0;
    if (bLock) {
        if (bNonBlocking) { // non-blocking
            retval = (fcntl(fno, F_SETLK, &fl)==0);
        } else { // blocking
            retval = (fcntl(fno, F_SETLKW, &fl)==0);          
            if (!retval) {
                if (errno == EAGAIN) {
                    printf("LockFileSection: "
                            "Lock is being held by process %d\n", fl.l_pid);
                } else {
                    printf("Lock of file %d (proc=%d) returned error %d (%s)\n",
                        fno, fl.l_pid, errno, strerror(errno));
                }
            }
        }
    } else {
        fl.l_type = F_UNLCK;
        retval = (fcntl(fno, F_SETLKW, &fl)==0);
        if (retval==0) {
            printf("LockFileSection: Unlock failed with error %d (%s)\n", errno, strerror(errno));
        }
    }
    return(retval);
#endif
}


#ifdef _WIN32
#undef W_OK
#undef R_OK
#undef X_OK
#undef access
#undef lseek
#undef locking
#undef open
#undef write
#undef read
#undef close
#endif

