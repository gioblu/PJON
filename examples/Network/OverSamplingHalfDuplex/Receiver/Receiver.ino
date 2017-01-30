
/* Include Async ACK code setting INCLUDE_ASYNC_ACK as true before including PJON.h */
#define INCLUDE_ASYNC_ACK true

#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(bus_id, 44);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pins(11, 12);
  bus.begin();
  bus.set_receiver(receiver_function);
  bus.set_synchronous_acknowledge(false);
};

void receiver_function(uint8_t *payload, uint16_t length, const PacketInfo &packet_info) {
  if(payload[0] == 'B') {
    digitalWriteFast(13, HIGH);
    delay(5);
    digitalWriteFast(13, LOW);
    delay(5);
  }
}

void loop() {
  bus.update();
  bus.receive(50000); // Receive for 50 milliseconds
};
