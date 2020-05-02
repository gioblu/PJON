#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(44);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off

  Serial.begin(9600);

  bus.strategy.set_serial(&Serial);
  bus.strategy.set_enable_RS485_pin(2);
  bus.set_receiver(receiver_function);
  bus.set_acknowledge(false);
  bus.begin();
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
    bus.reply("B", 1);
  }
};

void loop() {
  bus.update();
  bus.receive(1000);
};
