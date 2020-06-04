#include <PJONSwitch.h>

/* Route packets between a collection of buses with the same
   or different strategies or media.
   __________                 ________                 __________
  |          |         Pin 7 |        | Pin 12        |          |
  | DEVICE 1 |_______________| SWITCH |_______________| DEVICE 2 |
  |__________|  Bus 0.0.0.1  |________|  Bus 0.0.0.2  |__________| */

PJONSwitch2<SoftwareBitBang, SoftwareBitBang> router;

void setup() {
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_pin(12);
  router.get_bus(0).set_bus_id((const uint8_t[4]){0, 0, 0, 1});
  router.get_bus(1).set_bus_id((const uint8_t[4]){0, 0, 0, 2});
  router.begin();
}

void loop() {
  router.loop();
}
