#include <PJON.h>
PJON network(A0, 45);

void setup() {
  Serial.begin(115200);
  network.set_acknowledge(true);
  network.set_collision_avoidance(true);
  network.set_encryption(false);
  network.send(44, "XB", 1000000);
}

void loop() {
  network.update();
};
