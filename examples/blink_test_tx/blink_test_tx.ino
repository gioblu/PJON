#include <PJON.h>
PJON network(A0, 45);

void setup() {
  network.send(44, "B", 1000000);
  // Send B to device 44 every second
}

void loop() {
  network.update();
};