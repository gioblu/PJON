#include <PJONRouter.h>
#include <PJONSoftwareBitBang.h>

/* Route packets between a collection of buses with the same or if required
   different strategies or media.

   PJONSwitch routing between two separate SoftwareBitBang buses

   - A Device on bus 0.0.0.1 is connected to the same medium as bus 0.0.0.3
   - A device on bus 0.0.0.2 is connected to the same medium as bus 0.0.0.4

   Packets are routed using configurable static routing table

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

   Use Device1 and Device2 from the Switch example  */

PJONRouter2<SoftwareBitBang, SoftwareBitBang> router;

void setup() {
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_pin(12);

  // Set bus ids
  router.get_bus(0).set_bus_id((const uint8_t[4]){0, 0, 0, 3});
  router.get_bus(1).set_bus_id((const uint8_t[4]){0, 0, 0, 4});

  // Populate routing table (which physical bus to use to reach remote bus)
  router.add((const uint8_t[4]){0,0,0,1}, 0);
  router.add((const uint8_t[4]){0,0,0,2}, 1);

  router.begin();
}

void loop() {
  router.loop();
}
