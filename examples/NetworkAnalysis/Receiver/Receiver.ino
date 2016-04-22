#include <PJON.h>

// network(Arduino pin used, selected device id)
PJON network(12, 44);

void setup() {
  Serial.begin(115200);
};

void loop() {
  network.receive(); 
};
