/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true

#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, 45);

// 1 millisecond maximum expected latency using short wires, higher if necessary
uint32_t latency = 1000;

void setup() {
  // Initialize LED 13 to be off
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  /* Initialize the Serial instance used for communication
     and pass it to PJON's ThroughSerial strategy */
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);

  /* Set configuration to send packet requesting
     asynchronous acknowledgement response */
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);

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
    bus.receive(TS_TIME_IN + latency);
    /* Receive for, at least, the minumum timeframe necessary
       for receiver to respond back an acknowledgement packet
       (Avoid unnecessary retransmissions) */
};
