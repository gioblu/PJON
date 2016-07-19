#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
}


void receiver_function(uint8_t *payload, uint8_t length, const PacketInfo &packet_info) {
  if(payload[0] == 'B') {
    Serial.println("BLINK");
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
}

void loop() {
  bus.update();
  bus.receive(1000);
};
