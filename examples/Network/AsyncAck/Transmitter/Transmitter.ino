
/* Include Async ACK code setting INCLUDE_ASYNC_ACK as true before including PJON.h */
#define INCLUDE_ASYNC_ACK true

#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(bus_id, 45);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pin(12);
  /* A packet containing the id of every packet received will be sent back
     by this device to the packet's sender to acknowledge packet reception. */
  bus.set_asynchronous_acknowledge(true);
  bus.begin();

  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
}

void loop() {
  bus.update();
  bus.receive(1000);
};
