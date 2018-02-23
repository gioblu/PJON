#include <PJONSwitch.h>

/* Route packets between a collection of buses with the same
   or different strategies or media.
   __________                 ________                 __________
  |          |        Pin 11 |        | Pin 12        |          |
  | DEVICE 1 |_______________| SWITCH |_______________| DEVICE 2 |
  |__________|  Bus 0.0.0.1  |________|  Bus 0.0.0.2  |__________| */

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
