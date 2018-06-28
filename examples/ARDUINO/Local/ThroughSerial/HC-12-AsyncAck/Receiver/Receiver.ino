
// Include asynchronous acknowledgement feature
#define PJON_INCLUDE_ASYNC_ACK true

#include <PJON.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);

uint8_t bus_id[] = {0, 0, 0, 1};
// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(bus_id, 44);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  // Set HC12 baudrate (you must use the one configured in HC12, default 9600)
  HC12.begin(9600);

  bus.strategy.set_serial(&HC12);
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
  bus.receive(1000);
};
