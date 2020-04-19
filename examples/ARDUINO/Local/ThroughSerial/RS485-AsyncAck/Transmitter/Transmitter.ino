
/* PJON MAX485 serial modules RS485 communication example */

// Include async ack defining PJON_INCLUDE_ASYNC_ACK before including PJON.h
#define PJON_INCLUDE_ASYNC_ACK true
// Synchronous acknowledgement is not used, set TS_RESPONSE_TIME_OUT to 0
#define TS_RESPONSE_TIME_OUT 0

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(45);

// 5 milliseconds maximum expected latency, higher if necessary
uint32_t latency = 5000;

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST)
    digitalWrite(LED_BUILTIN, HIGH); // Light up LED 13 if a packet transmission failed
};

void setup() {
  // Initialize LED 13 to be off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
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
};

void loop() {
    bus.update();
    bus.receive(TS_TIME_IN + latency);
    /* Receive for, at least, the minumum timeframe necessary
       for receiver to respond back an acknowledgement packet
       (Avoid unnecessary retransmissions) */
};
