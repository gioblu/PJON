#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 45);

void setup() {
  bus.set_pin(12);
  bus.begin();
  bus.send(44, "B", 1, 1000000); // Send B to device 44 every second
}

void loop() {
  bus.update();
};
