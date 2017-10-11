
/* PJON MAX485 serial modules RS485 communication example */

/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, 45);

void setup() {
  // Initialize LED 13 to be off
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  // Initialize Serial instance used for PJON communication
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
  // Set asynchronous acknowledgement response, avoid sync
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);
  // Set enable pins
  bus.strategy.set_RS485_rxe_pin(3);
  bus.strategy.set_RS485_txe_pin(2);
  bus.set_error(error_handler);
  bus.begin();
  // Send B to device 44 every 1.5s
  bus.send_repeatedly(44, "B", 1, 1500000);
}

void error_handler(uint8_t code, uint8_t data) {
  if(code == PJON_CONNECTION_LOST)
    digitalWrite(13, HIGH); // Light up LED 13 if a packet transmission failed
}

void loop() {
    bus.update();
    bus.receive();
};
