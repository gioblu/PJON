#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 44);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.set_pin(12);
  bus.begin();

  bus.set_receiver(receiver_function);
  bus.send(45, "B", 1);

  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint8_t length, const PacketInfo &packet_info) {
 if((char)payload[0] == 'B') {
    bus.reply("B", 1);
    digitalWrite(13, HIGH);
    delay(5);
    digitalWrite(13, LOW);
    delay(5);
  }
}

void loop() {
  bus.receive(50000);
  bus.update();
};
