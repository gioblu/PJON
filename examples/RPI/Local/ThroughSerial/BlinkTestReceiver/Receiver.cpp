
// For printf used below
#include <cstdio>
// PJON library
#include <cinttypes>
#include <cstdlib>
#include <cstring>
// RPI serial interface
#include <wiringPi.h>
#include <wiringSerial.h>

#ifndef RPI
  #define RPI true
#endif

#define TS_RESPONSE_TIME_OUT 35000
/* Maximum accepted duration between transmission and acknowledgement.
   May be necessary to set a higher value if devices are separated by a long
   physical distance and or if long packets are transmitted. */


 // Include only ThroughSerial
#include <PJONThroughSerial.h>

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    printf("BLINK\n");
    fflush(stdout);
  }
};

int main() {
  printf("PJON instantiation... \n");
  PJONThroughSerial bus(44);
  uint32_t baud_rate = 9600;
  printf("Opening serial... \n");
  int s = serialOpen("/dev/ttyAMA0", baud_rate);
  if(int(s) < 0) printf("Serial open fail!");
  if(wiringPiSetup() == -1) printf("WiringPi setup fail");
  printf("Setting serial... \n");
  bus.strategy.set_serial(s);
  bus.strategy.set_baud_rate(baud_rate);
  bus.set_receiver(receiver_function);

  printf("Opening bus... \n");
  bus.begin();
  printf("Waiting for BLINK... \n");
  while(true) bus.receive();

  return 0;
};
