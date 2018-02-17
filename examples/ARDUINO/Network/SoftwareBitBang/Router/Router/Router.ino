#include <PJONRouter.h>

/* Route packets between a collection of buses with the same
   or different strategies or media. */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1((uint8_t[4]){0,0,0,1}, 1, &link1),
        bus2((uint8_t[4]){0,0,0,2}, 1, &link2);
PJONRouter router(2, (PJONAny*[2]){&bus1,&bus2}, PJON_NOT_ASSIGNED);

void setup() {
  link1.strategy.set_pin(7);
  link2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
}
