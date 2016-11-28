
/* Include Async ACK code setting INCLUDE_ASYNC_ACK as true before including PJON.h */
#define INCLUDE_ASYNC_ACK true

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pin(12);
  bus.set_asynchronous_acknowledge(true);
  bus.begin();
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
}

void loop() {
  bus.update();
  bus.receive(1000);
};
