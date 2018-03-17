
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>

// Maximum expected latency or response time, higher if necessary
uint32_t latency = 6000;

// Bus id definition
uint8_t bus_id_A[] = {0, 0, 0, 1};
uint8_t bus_id_B[] = {0, 0, 0, 2};

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(bus_id_B, 2);

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST)
    digitalWrite(13, HIGH);
  // Light up LED 13 if a packet transmission failed
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
}

void setup() {
  // Initialize LED 13 to be off
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  bus.set_error(error_handler);
  bus.set_receiver(receiver_function);
  bus.set_synchronous_acknowledge(true);
  bus.set_asynchronous_acknowledge(true);
  bus.strategy.set_pin(12);
  bus.begin();
}

void loop() {
  bus.update();
  bus.receive(latency);
};
