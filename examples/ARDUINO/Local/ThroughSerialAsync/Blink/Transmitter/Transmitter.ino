
// Include the ThroughSerialAsync strategy
#define PJON_INCLUDE_TSA true

#include <PJON.h>

// Use ThroughSerialAsync as a PJON Strategy
PJON<ThroughSerialAsync> bus(45);

void setup() {
  Serial.begin(9600);

  bus.strategy.set_serial(&Serial);
  bus.begin();
  // Send B to device 44 every second
  bus.send_repeatedly(44, (uint8_t *)"B", 1, 1000000);
};

void loop() {
  bus.update();
};
