
// For printf used below
#include <stdio.h>
// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

 // Include only ThroughSerial
#include <PJONThroughSerial.h>

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    printf("BLINK\n");
  }
}

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
  bus.set_receiver(receiver_function);

  printf("Opening bus... \n");
  bus.begin();
  printf("Success, initiating BlinkTest repeated transmission... \n");
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second

  while(true) {
    bus.update();
    bus.receive(1000);
  }
};
