
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

#define PJON_INCLUDE_TS true // Include only ThroughSerial

#include <PJON.h>

PJON<ThroughSerial> bus(45);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    printf("BLINK\n");
    fflush(stdout);
  }
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    printf("Connection with device ID ");
    printf("%d is lost", bus.packets[data].content[0]);
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    printf("Packet buffer is full, has now a length of %d \n", data);
    printf("Possible wrong bus configuration! \n");
    printf("higher PJON_MAX_PACKETS if necessary. \n");
  }
  if(code == PJON_CONTENT_TOO_LONG)
    printf("Content is too long, length: %d \n", data);
};

int main() {
  uint32_t baud_rate = 9600;

  printf("PJON ThroughSerial BlinkWithResponse Transmitter test \n");
  printf("Opening serial port /dev/ttyAMA0 @ %d \n", baud_rate);
  int s = serialOpen("/dev/ttyAMA0", baud_rate);
  if(int(s) < 0) printf("ERROR - Serial open fail");
  if(wiringPiSetup() == -1) printf("ERROR - WiringPi setup fail");

  printf("Strategy Serial link configuration \n");
  bus.strategy.set_serial(s);
  bus.strategy.set_baud_rate(baud_rate);

  printf("Bus initialization \n");
  bus.set_synchronous_acknowledge(false);
  bus.set_receiver(receiver_function);
  bus.begin();

  printf("Repeated sending dispatch \n");
  bus.send_repeatedly(44, "B", 1, 1000000);

  printf("Packet transmission \n");
  bus.update();
  printf("Reception attempt \n");
  bus.receive();
  printf("No errors occurred \n");
  printf("Press control + c to quit \n");

  while(true) {
    bus.update();
    bus.receive();
  }

  return 0;
};
