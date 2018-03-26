#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<OverSampling> bus(bus_id, 44);

void setup() {
  /* When using more than one pin always use pins connected to
     a different port group to avoid cross-talk. */
  bus.strategy.set_pins(7, 12);
  bus.begin();
};

void loop() {
  bus.receive();
};
