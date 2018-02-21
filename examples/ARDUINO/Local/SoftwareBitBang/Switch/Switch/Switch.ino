#include <PJONSwitch.h>

/* Route packets between a collection of buses with the same
   or different strategies or media. All physical buses are taking part
   of the same local bus, with the device id range being segmented. */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(1, &link1, 1000, 2, 0); // Devices with id 1-127
PJONAny bus2(1, &link2, 1000, 2, 1); // Devices with id 128-253

PJONSwitch router(2, (PJONAny*[2]){&bus1, &bus2});

void setup() {
  link1.strategy.set_pin(11);
  link2.strategy.set_pin(12);
  router.begin();
};

void loop() {
  router.loop();
};
