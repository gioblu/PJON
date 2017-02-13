#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(44);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off

  bus.begin();
  bus.set_receiver(receiver_function);

  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
};

void loop() {
  bus.receive(1000);
};
