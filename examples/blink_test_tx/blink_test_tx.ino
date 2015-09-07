#include <PJON.h>
PJON network(A0, 45);

void setup() {
  Serial.begin(115200);
  network.set_acknowledge(true);
  network.set_collision_avoidance(true);
  network.set_encryption(false);
}

void loop() {
  network.send_short_command(44, 'B');
  delay(1000);
};
