#include <PJONDynamicRouter.h>

/* Route packets between a collection of buses with the same or if required
   different strategies or media. Packets are routed transparently using a
   routing table dynamically populated by traffic.

   - A Device on bus 0.0.0.1 is connected to the same medium as bus 0.0.0.3
   - A device on bus 0.0.0.2 is connected to the same medium as bus 0.0.0.4

                     ________
        Bus 0.0.0.3 |        | Bus 0.0.0.4
    ________________| ROUTER |________________
          |  Pin 7  |________|  Pin 12 |
          |                            |
          | Bus 0.0.0.1    Bus 0.0.0.2 |
     _____|____                   _____|____
    |          |                 |          |
    | DEVICE 1 |                 | DEVICE 2 |
    |__________|                 |__________|

    Use Device1 and Device2 from the Switch example.

    You may need to restart Device2 so that the packet it sends at
    startup will be registered and the reverse route established. */

PJONDynamicRouter2<SoftwareBitBang, SoftwareBitBang> router;

void setup() {
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_pin(12);
  router.get_bus(0).set_bus_id((const uint8_t[4]){0, 0, 0, 3});
  router.get_bus(1).set_bus_id((const uint8_t[4]){0, 0, 0, 4});
  router.begin();
}

void loop() {
  router.loop();
}
