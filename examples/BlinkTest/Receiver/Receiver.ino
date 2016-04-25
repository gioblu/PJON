#include <PJON.h>

// bus(Arduino pin used, selected device id)
PJON bus(12, 44);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.begin(); // Initialize PJON bus
  bus.set_receiver(receiver_function);

  Serial.begin(9600);
};

void receiver_function(uint8_t length, uint8_t *payload) {
  if(payload[0] == 'B') {
    Serial.println("BLINK");
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
}

void loop() {
  bus.receive(1000);
};
