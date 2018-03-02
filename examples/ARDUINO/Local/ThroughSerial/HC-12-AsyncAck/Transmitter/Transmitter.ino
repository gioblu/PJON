
// Include asynchronous acknowledgement feature
#define PJON_INCLUDE_ASYNC_ACK true

#include <PJON.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);

uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, 45);

int latency = 1000;
/* 1 millisecond maximum expected latency having modules in close proximity
   higher latency if communication fails when modules are physically distant */

void setup() {
  // Initialize LED 13 to be off
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  // Set HC12 baudrate (you must use the one configured in HC12, default 9600)
  HC12.begin(9600);

  bus.strategy.set_serial(&HC12);

  /* Set configuration to send packet disabling synchronous and
     enabling asynchronous acknowledgement response */
  bus.set_synchronous_acknowledge(false);
  bus.set_asynchronous_acknowledge(true);

  bus.set_error(error_handler);
  bus.begin();

  // Send B to device 44 every 1.5s
  bus.send_repeatedly(44, "B", 1, 1500000);
}

void error_handler(uint8_t code, uint8_t data, void *custom_pointer) {
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
