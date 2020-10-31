#include <PJONSwitch.h>
#include <PJONSoftwareBitBang.h>

/* Route packets between a collection of buses with the same
   or different strategies or media.

   PJONSimpleSwitch routes between buses of the same strategy.
   It uses less storage and RAM than the more general PJONSwitch.
   __________                 ________                 __________
  |          |         Pin 7 |        | Pin 12        |          |
  | DEVICE 1 |_______________| SWITCH |_______________| DEVICE 2 |
  |__________|  Bus 0.0.0.1  |________|  Bus 0.0.0.2  |__________| */

PJONSoftwareBitBang bus1((const uint8_t[4]){0, 0, 0, 1}, PJON_NOT_ASSIGNED),
                    bus2((const uint8_t[4]){0, 0, 0, 2}, PJON_NOT_ASSIGNED);

PJONSimpleSwitch<SoftwareBitBang> router(bus1, bus2);

void setup() {
  bus1.strategy.set_pin(7);
  bus2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
}
