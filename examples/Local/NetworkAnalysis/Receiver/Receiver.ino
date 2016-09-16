#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(44);

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
};

void loop() {
  bus.receive();
};
