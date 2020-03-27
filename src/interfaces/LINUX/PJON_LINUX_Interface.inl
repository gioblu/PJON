#ifndef LINUX
    #define LINUX
#endif
#ifndef PJON_LINUX_SEPARATE_DEFINITION
    #define PJON_LINUX_SEPARATE_DEFINITION
#endif

#include "PJON_LINUX_Interface.h"
#include "/usr/include/asm-generic/termbits.h"
#include "/usr/include/asm-generic/ioctls.h"

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
  auto begin_ts = std::chrono::high_resolution_clock::now();
  while(true) {
    auto elapsed_usec =
    std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::high_resolution_clock::now() - begin_ts
    ).count();
    if(elapsed_usec >= delay_value) break;
    std::this_thread::sleep_for(std::chrono::microseconds(50));
  }
};

void delay(uint32_t delay_value_ms) {
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_value_ms));
};

/* Open serial port ----------------------------------------------------- */

int serialOpen(const char *device, const int baud) {
  speed_t bd;
  int fd;

  if((fd = open(device, O_NDELAY | O_NOCTTY | O_NONBLOCK | O_RDWR)) == -1)
  return -1;

  fcntl(fd, F_SETFL, O_RDWR);

  struct termios2 config;
  int r = ioctl(fd, TCGETS2, &config);
  if(r) {
    return -1;
  }

  config.c_ispeed = config.c_ospeed = baud;
  config.c_cflag &= ~CBAUD;
  config.c_cflag |= BOTHER;
  config.c_cflag |= (CLOCAL | CREAD);
  config.c_cflag &= ~(CSTOPB | CSIZE | PARENB);
  config.c_cflag |= CS8;
  config.c_lflag &= ~(ECHO | ECHOE | ICANON | ISIG);
  config.c_oflag &= ~OPOST;
  config.c_cc [VMIN] = 0;
  config.c_cc [VTIME] = 50; // 5 seconds reception timeout

  r = ioctl(fd, TCSETS2, &config);
  if(r) {
    return -1;
  }

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
