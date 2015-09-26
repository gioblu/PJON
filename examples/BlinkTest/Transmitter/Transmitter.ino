#include <PJON.h>

// network(Arduino pin used, selected device id)
PJON network(12, 45);

void setup() {
  // Send B to device 44 every second
  network.send(44, "B", 1, 1000000);
}

void loop() {
  network.update();
};