#include <PJON.h>

// bus(Arduino pin used, selected device id)
PJON bus(12, 45);

void setup() {
  bus.begin(); // Initialize PJON bus
  bus.send(44, "B", 1, 1000000); // Send B to device 44 every second
}

void loop() {
  bus.update();
};
