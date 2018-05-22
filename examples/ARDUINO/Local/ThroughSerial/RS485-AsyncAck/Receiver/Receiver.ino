
/* PJON MAX485 serial modules RS485 communication example */

// Include async ack defining PJON_INCLUDE_ASYNC_ACK before including PJON.h
#define PJON_INCLUDE_ASYNC_ACK true
// Synchronous acknowledgement is not used, set TS_RESPONSE_TIME_OUT to 0
#define TS_RESPONSE_TIME_OUT 0

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(44);

void setup() {
  // Initialize LED 13 to be off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  // Initialize Serial instance used for PJON communication
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
  // Avoid default sync ack
  bus.set_synchronous_acknowledge(false);
  // Set enable pins
  bus.strategy.set_RS485_rxe_pin(3);
  bus.strategy.set_RS485_txe_pin(2);
  bus.begin();
  bus.set_receiver(receiver_function);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(3);
    digitalWrite(LED_BUILTIN, LOW);
    delay(3);
  }
}

void loop() {
  bus.update();
  bus.receive();
};
