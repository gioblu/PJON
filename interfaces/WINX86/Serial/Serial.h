/** Serial.h
 *
 * A very simple serial port control class that does NOT require MFC/AFX.
 *
 * License: This source code can be used and/or modified without restrictions.
 * It is provided as is and the author disclaims all warranties, expressed
 * or implied, including, without limitation, the warranties of
 * merchantability and of fitness for any purpose. The user must assume the
 * entire risk of using the Software.
 *
 * @author Hans de Ruiter, Zbigniew Zasieczny
 *
 * @version 0.1 -- 28 October 2008
 * @version 0.2 -- 20 April 2017 (Zbigniew Zasieczny)
 *  - added methods needed by PJON to handle WINX86 abstraction interface
 */

#pragma once

#if defined(_WIN32)
  #include <string>
  #include <windows.h>

  typedef std::basic_string<TCHAR> tstring;

  class Serial {

  private:
    HANDLE commHandle;

  public:
    Serial(
      tstring &commPortName,
      int bitRate = 115200,
      bool testOnStartup = false,
      bool cycleDtrOnStartup = false
    );

    virtual ~Serial();

    /** Writes a NULL terminated string.
     *
     * @param buffer the string to send
     *
     * @return int the number of characters written
     */
    int write(const char buffer[]);

    /** Writes a string of bytes to the serial port.
     *
     * @param buffer pointer to the buffer containing the bytes
     * @param buffLen the number of bytes in the buffer
     *
     * @return int the number of bytes written
     */
    int write(char *buffer, int buffLen);

    /** Reads a string of bytes from the serial port.
     *
     * @param buffer pointer to the buffer to be written to
     * @param buffLen the size of the buffer
     * @param nullTerminate if set to true it will null terminate the string
     *
     * @return int the number of bytes read
     */
    int read(char *buffer, int buffLen, bool nullTerminate = true);

    /** Writes a single character to the serial port
    *
    * @param buffer pointer to the buffer to be written to
    *
    * @return int the number of bytes written (should be always 1)
    */
    int putChar(char *buffer);

    /** Returns single character from the receive buffer
    */
    char getChar();

    /** Returns true if there is data available in receive buffer
    */
    bool serialDataAvail();

    /** Flushes everything from the serial port's read buffer
     */
    void flush();
  };
  
#endif
