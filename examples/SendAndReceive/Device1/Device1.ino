#include <PJON.h>

// bus(Arduino pin used, selected device id)
PJON bus(12, 44);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off
  bus.begin(); // Initialize PJON bus

  bus.set_receiver(receiver_function);
  bus.send(45, "B", 1);
  Serial.begin(115200);
};

void receiver_function(uint8_t length, uint8_t *payload) {
 if((char)payload[0] == 'B') {
    bus.send(45, "B", 1);
    digitalWrite(13, HIGH);
    delay(15);
    digitalWrite(13, LOW);
  }
}

void loop() {
  bus.receive(1000);
  bus.update();
};
