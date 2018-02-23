
/* This example illustrates how to route packets between devices sharing the
   same bus id although potentially using different strategies or media to
   be connected.

   PJONSwitch routing between two separate SoftwareBitBang buses
     __________           ________           __________
    |          | 0.0.0.1 |        | 0.0.0.1 |          |
    | DEVICE 1 |_________| SWITCH |_________| DEVICE 2 |
    |__________| Pin 11  |________| Pin 12  |__________| */

#include <PJONSwitch.h>

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
