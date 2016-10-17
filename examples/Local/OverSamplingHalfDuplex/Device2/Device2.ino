#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(45);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pins(11, 12);
  bus.set_receiver(receiver_function);

  bus.begin();

  bus.send_repeatedly(44, "B", 1, 1000000);

  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint8_t length, const PacketInfo &packet_info) {
 if((char)payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(5);
    digitalWrite(13, LOW);
    delay(5);
  }
}

void loop() {
  bus.receive();
  bus.update();
};
