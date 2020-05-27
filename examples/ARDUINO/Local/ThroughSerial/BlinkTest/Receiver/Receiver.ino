#include <PJON.h>

PJON<ThroughSerial> bus(44);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
    digitalWrite(LED_BUILTIN, LOW);
  }
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
  bus.begin();

  bus.set_receiver(receiver_function);
};

void loop() {
  bus.receive(1000);
};
