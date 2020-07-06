#include <PJONOverSampling.h>


PJONOverSampling bus(44);

void setup() {
  /* When using more than one pin always use pins connected to
     a different port group to avoid cross-talk. */
  bus.strategy.set_pins(7, 12);

  bus.begin();

  Serial.begin(115200);
};

void loop() {
  bus.receive();
};
