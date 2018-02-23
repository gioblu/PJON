#include <PJONSwitch.h>

/* Route packets between a collection of buses with the same
   or different strategies or media, using the same bus id 
   but different parts of the device id range. */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1, (uint8_t[4]){0,0,0,1}, PJON_NOT_ASSIGNED, 1000, 2, 0);
PJONAny bus2(&link2, (uint8_t[4]){0,0,0,1}, PJON_NOT_ASSIGNED, 1000, 2, 1);

PJONSwitch router(2, (PJONAny*[2]){&bus1, &bus2}, PJON_NOT_ASSIGNED);

void setup() {
  link1.strategy.set_pin(11);
  link2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
}