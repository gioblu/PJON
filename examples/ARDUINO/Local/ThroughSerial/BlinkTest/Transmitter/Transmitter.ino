
#include <PJON.h>

PJON<ThroughSerial> bus(45);

void setup() {
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial); // Pass the Serial object you want to use for communication
  bus.begin();
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
};

void loop() {
  bus.update();
};
