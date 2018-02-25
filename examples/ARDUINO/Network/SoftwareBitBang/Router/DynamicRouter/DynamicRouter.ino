#include <PJONDynamicRouter.h>

/* Route packets between a collection of buses with the same or if required
   different strategies or media. Packets are routed trasparently using a
   routing table dynamically populated by traffic.

   - A Device on bus 0.0.0.1 is connected to the same medium as bus 0.0.0.3
   - A device on bus 0.0.0.2 is connected to the same medium as bus 0.0.0.4

                     ________
        Bus 0.0.0.3 |        | Bus 0.0.0.4
    ________________| ROUTER |________________
          | Pin 11  |________|  Pin 12 |
          |                            |
          | Bus 0.0.0.1    Bus 0.0.0.2 |
     _____|____                   _____|____
    |          |                 |          |
    | DEVICE 1 |                 | DEVICE 2 |
    |__________|                 |__________|

    Use Device1 and Device2 from the Switch example.

    You may need to restart Device2 so that the packet it sends at
    startup will be registered and the reverse route established. */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1, (const uint8_t[4]){0,0,0,3});
PJONAny bus2(&link2, (const uint8_t[4]){0,0,0,4});

PJONDynamicRouter router(2, (PJONAny*[2]){&bus1, &bus2});

void setup() {
  link1.strategy.set_pin(11);
  link2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
}
