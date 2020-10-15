#ifndef LINUX
    #define LINUX
#endif
#ifndef PJON_LINUX_SEPARATE_DEFINITION
    #define PJON_LINUX_SEPARATE_DEFINITION
#endif

#include "PJON_LINUX_Interface.h"

auto start_ts = std::chrono::high_resolution_clock::now();
auto start_ts_ms = std::chrono::high_resolution_clock::now();

uint32_t micros() {
  auto elapsed_usec =
  std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::high_resolution_clock::now() - start_ts
  ).count();

  if(elapsed_usec >= UINT32_MAX) {
    start_ts = std::chrono::high_resolution_clock::now();
    return 0;
  } else return elapsed_usec;
};

uint32_t millis() {
  return (uint32_t)
  std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::high_resolution_clock::now() - start_ts_ms
  ).count();
};

void delayMicroseconds(uint32_t delay_value) {
  struct timeval tv;
  if (delay_value < 1000000){
    tv.tv_sec = 0;
    tv.tv_usec = delay_value;
  }
  else{ 
    tv.tv_sec = floor(delay_value / 1000000); 
    tv.tv_usec = delay_value - tv.tv_sec * 1000000;
  }
 
  select(0, NULL, NULL, NULL, &tv);
};

void delay(uint32_t delay_value_ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_value_ms));
};

/* Open serial port ----------------------------------------------------- */

int serialOpen(const char *device, const int baud) {
  speed_t bd;
  int fd;
  int state;
  struct termios config;

  switch(baud) {
    case     200:	bd =     B200; break;
    case     300:	bd =     B300; break;
    case     600:	bd =     B600; break;
    case    1200:	bd =    B1200; break;
    case    1800:	bd =    B1800; break;
    case    2400:	bd =    B2400; break;
    case    4800:	bd =    B4800; break;
    case    9600:	bd =    B9600; break;
    case   19200:	bd =   B19200; break;
    case   38400:	bd =   B38400; break;
    case   57600:	bd =   B57600; break;
    case  115200:	bd =  B115200; break;
    case  230400:	bd =  B230400; break;
    #if !defined(__APPLE__) && !defined(__FreeBSD__)
      case  460800:	bd =  B460800; break;
      case  500000:	bd =  B500000; break;
      case  576000:	bd =  B576000; break;
      case  921600:	bd =  B921600; break;
      case 1000000:	bd = B1000000; break;
      case 1152000:	bd = B1152000; break;
      case 1500000:	bd = B1500000; break;
      case 2000000:	bd = B2000000; break;
      case 2500000:	bd = B2500000; break;
      case 3000000:	bd = B3000000; break;
      case 3500000:	bd = B3500000; break;
      case 4000000:	bd = B4000000; break;
    #endif
    default: return -2;
  }

  if((fd = open(device, O_NDELAY | O_NOCTTY | O_NONBLOCK | O_RDWR)) == -1)
  return -1;

  fcntl(fd, F_SETFL, O_RDWR);
  tcgetattr(fd, &config);
  cfmakeraw(&config);
  cfsetispeed(&config, bd);
  cfsetospeed(&config, bd);

  config.c_cflag |= (CLOCAL | CREAD);
  config.c_cflag &= ~(CSTOPB | CSIZE | PARENB);
  config.c_cflag |= CS8;
  config.c_lflag &= ~(ECHO | ECHOE | ICANON | ISIG);
  config.c_oflag &= ~OPOST;
  config.c_cc [VMIN] = 0;
  config.c_cc [VTIME] = 50; // 5 seconds reception timeout

  tcsetattr(fd, TCSANOW, &config);
  ioctl(fd, TIOCMGET, &state);
  state |= (TIOCM_DTR | TIOCM_RTS);
  ioctl(fd, TIOCMSET, &state);

  usleep(10000);	// Sleep for 10 milliseconds
  return fd;
};

/* Returns the number of bytes of data available to be read in the buffer */

int serialDataAvailable(const int fd) {
  int result = 0;
  ioctl(fd, FIONREAD, &result);
  return result;
};

/* Reads a character from the serial buffer ------------------------------- */

int serialGetCharacter(const int fd) {
  uint8_t result;
  if(read(fd, &result, 1) != 1) return -1;
  return ((int)result) & 0xFF;
};
