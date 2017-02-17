
/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true

#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(bus_id, 44);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pins(11, 12);
  bus.begin();
  bus.set_receiver(receiver_function);
  bus.set_synchronous_acknowledge(false);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(5);
    digitalWrite(13, LOW);
    delay(5);
  }
}

void loop() {
  bus.update();
  bus.receive(50000); // Receive for 50 milliseconds
};
