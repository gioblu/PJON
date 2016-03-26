#include <PJON.h>

PJON network(D1, 44);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up");
};

void loop() {
  network.receive();
};
