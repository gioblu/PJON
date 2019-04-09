
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>

// Maximum expected latency or response time, higher if necessary
uint32_t latency = 6000;

// Bus id definition
uint8_t bus_id_A[] = {0, 0, 0, 1};
uint8_t bus_id_B[] = {0, 0, 0, 2};

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(bus_id_A, 1);

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST)
    digitalWrite(LED_BUILTIN, HIGH);
  // Light up LED 13 if a packet transmission failed
}

void setup() {
  // Initialize LED 13 to be off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  bus.set_error(error_handler);
  bus.set_synchronous_acknowledge(true);
  bus.set_asynchronous_acknowledge(true);
  bus.strategy.set_pin(12);
  bus.begin();

  // Send B to device 44 every 1.5s
  bus.send_repeatedly(2, bus_id_B, (uint8_t *)"B", 1, 1000000);
}

void loop() {
  bus.update();
  bus.receive(latency);
};
