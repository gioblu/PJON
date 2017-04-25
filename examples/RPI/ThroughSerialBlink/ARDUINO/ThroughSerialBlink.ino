
/* Connect the Serial GPIO TX and RX of your Raspberry Pi with an Arduino
   compatible device (crossing the channels) through a level shifter not to
   damage the 3v3 rpi serial port. */

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(44);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off
  Serial.begin(115200);
  bus.strategy.set_serial(&Serial);
  bus.begin();
  bus.set_receiver(receiver_function);
};

void receiver_function(
  uint8_t *payload,
  uint16_t length,
  const PJON_Packet_Info &packet_info
) {
  if(payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(5);
    digitalWrite(13, LOW);
  }
}

void loop() {
  bus.receive(1000);
};
