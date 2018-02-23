#include <PJONRouterDynamic.h>

/* Route packets between a collection of buses with the same
   or different strategies or media. Use Device1 and Device2
   from the Switch example. 
   
   What is happening here is that a device on bus 0.0.0.1 is connected 
   to the same media as bus 0.0.0.3, and a device on bus 0.0.0.2 is
   connected to the same media as bus 0.0.0.4. When packets arrive
   this will lead to an entry in the routing table so that packets
   in the reverse direction can be delivered later. 
   You may need to restart Device2 so that the packet it sends at
   startup will be registered and the reverse route established.
   */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1, (const uint8_t[4]){0,0,0,3});
PJONAny bus2(&link2, (const uint8_t[4]){0,0,0,4});

PJONRouterDynamic router(2, (PJONAny*[2]){&bus1, &bus2});

void setup() {
  link1.strategy.set_pin(11);
  link2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
}