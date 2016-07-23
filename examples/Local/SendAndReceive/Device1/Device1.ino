#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(44);
int packet;

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.set_pin(12);
  bus.begin();

  bus.set_receiver(receiver_function);
  packet = bus.send(45, "B", 1);

  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint8_t length, const PacketInfo &packet_info) {
 if((char)payload[0] == 'B') {
    if(!bus.packets[packet].state)
      packet = bus.reply("B", 1); // Avoid duplicate sending checking old packet state
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
