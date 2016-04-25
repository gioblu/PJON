#include <PJON.h>

// bus(Arduino pin used, selected device id)
PJON bus(12, 44);

void setup() {
  bus.begin(); // Initialize PJON bus

  Serial.begin(115200);
};

void loop() {
  bus.receive();
};
