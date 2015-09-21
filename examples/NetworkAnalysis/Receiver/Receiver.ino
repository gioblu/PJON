#include <PJON.h>
PJON network(12, 44);

void setup() {
  Serial.begin(115200);
};

void loop() {
  network.receive(); 
};