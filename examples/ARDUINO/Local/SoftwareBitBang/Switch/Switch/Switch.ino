#include <PJONSwitch.h>

/* Route packets between a collection of buses with the same
   or different strategies or media. All physical buses are taking part
   of the same local bus, with the device id range being segmented.

  // SWITCH forwarding from bus 1 to bus 2 and vice versa
    __________                 ________                 __________
   |          | Bus 1   Pin 7 |        | Pin 12  Bus 2 |          |
   | DEVICE 1 |_______________| SWITCH |_______________| DEVICE 2 |
   |__________| Range 1-127   |________| Range 128-253 |__________| */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1, PJON_NOT_ASSIGNED, 1000, 2, 0); // Devices with id 1-127
PJONAny bus2(&link2, PJON_NOT_ASSIGNED, 1000, 2, 1); // Devices with id 128-253

PJONSwitch router(2, (PJONAny*[2]){&bus1, &bus2});

void setup() {
  link1.strategy.set_pin(7);
  link2.strategy.set_pin(12);
  router.begin();
};

void loop() {
  router.loop();
};
