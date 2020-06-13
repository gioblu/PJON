// For printf used below
#include <stdio.h>
// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TS_RESPONSE_TIME_OUT 35000

 // Include only ThroughSerial
#include <PJONThroughSerial.h>

#define BLINK_FREQUENCY 1000000 // 1 Blink per second

int main() {
  printf("PJON instantiation... \n");
  PJONThroughSerial bus(45);
  uint32_t baud_rate = 9600;
  printf("Opening serial... \n");

  // The name of the serial port may need to be edited
  int s = serialOpen("/dev/ttyUSB0", baud_rate);

  if(int(s) < 0) printf("Serial open fail!");
  printf("Setting serial... \n");
  bus.strategy.set_serial(s);
  bus.strategy.set_baud_rate(baud_rate);

  printf("Opening bus... \n");
  bus.begin();
  printf("Success, initiating BlinkTest repeated transmission... \n");
  uint32_t myTime = PJON_MICROS();
  while(true) {
    if((uint32_t)(PJON_MICROS() - myTime) > BLINK_FREQUENCY) {
      uint16_t result = bus.send_packet(44, "B", 1);
      if(result == PJON_ACK)
        printf("Packet transmission successful! \n");
      else printf("Packet transmission unsuccessful! Result: %d \n", result);
      myTime = PJON_MICROS();
    }
    bus.update();
    bus.receive();
  }
};
