#include <PJON.h>
PJON network(A0, 45);

void setup() {
  Serial.begin(115200);
  network.set_acknowledge(true);
  network.set_collision_avoidance(true);
  network.set_encryption(false);
  network.send(44, "XB", 1000000); 
  // X is the command symbol, you can also use send_command that will write that for you
  // B is the symbol we choose for the blink function on the receiver side
}

void loop() {
  network.update();
};
