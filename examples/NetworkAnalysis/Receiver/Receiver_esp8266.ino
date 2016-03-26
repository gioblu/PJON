#include <PJON.h>

// network(Arduino pin used, selected device id)
PJON network(D1, 44);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up");
};

void loop() {
  network.receive();
};
