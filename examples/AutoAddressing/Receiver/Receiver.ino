#include <PJON.h>

// bus(Arduino pin used, selected device id)
PJON bus(12, 44);

void setup() {
  bus.begin(); // Initialize PJON bus
  bus.set_receiver(receiver_function);

  Serial.begin(115200);
};

void receiver_function(uint8_t length, uint8_t *payload) {
  Serial.println(payload[0]);
}

void loop() {
  bus.receive(1000);
};
