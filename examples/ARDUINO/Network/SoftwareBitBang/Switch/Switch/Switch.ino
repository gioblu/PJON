#include <PJONSwitch.h>

/* Route packets between a collection of buses with the same
   or different strategies or media. */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1, (uint8_t[4]){0,0,0,1});
PJONAny bus2(&link2, (uint8_t[4]){0,0,0,2});

PJONSwitch router(2, (PJONAny*[2]){&bus1, &bus2});

void setup() {
  link1.strategy.set_pin(11);
  link2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
}