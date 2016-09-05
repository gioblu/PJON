#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(44);

void setup() {
  bus.strategy.set_pins(11, 12);

  bus.begin();

  Serial.begin(115200);
};

void loop() {
  bus.receive();
};
