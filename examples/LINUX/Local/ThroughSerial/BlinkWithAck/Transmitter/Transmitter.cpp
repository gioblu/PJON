
// For printf used below
#include <stdio.h>
// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* WiringPi RaspberryPi system access library is used to implement the Serial
   system calls, so be sure to have it available for use */

#include <wiringPi.h>

#ifndef RPI
  #define RPI true
#endif

#define TS_RESPONSE_TIME_OUT 35000
/* Maximum accepted timeframe between transmission and synchronous
   acknowledgement. This timeframe is affected by latency and CRC computation.
   Could be necessary to higher this value if devices are separated by long
   physical distance and or if transmitting long packets. */

#define PJON_INCLUDE_TS true // Include only ThroughSerial
#include <PJON.h>

int main() {
  printf("PJON instantiation... \n");
  PJON<ThroughSerial> bus(45);
  uint32_t baud_rate = 9600;
  printf("Opening serial... \n");
  int s = serialOpen("/dev/ttyACM0", baud_rate);
  if(int(s) < 0) printf("Serial open fail!");
  printf("Setting serial... \n");
  bus.strategy.set_serial(s);
  bus.strategy.set_baud_rate(baud_rate);

  printf("Opening bus... \n");
  bus.begin();
  printf("Success, initiating BlinkTest repeated transmission... \n");

  while(true) {
    usleep(100000);
    if(bus.send_packet(44, "B", 1) == PJON_ACK) {
      printf("Device blinked as requested! \n");
    } else printf("Failure! \n");
    bus.update();
    bus.receive(1000000);
  }
};
