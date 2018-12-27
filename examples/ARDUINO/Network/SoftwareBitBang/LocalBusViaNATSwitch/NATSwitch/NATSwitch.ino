/* Route packets between a local bus and a network bus in both directions,
   using a switch that supports NAT (network address translation).
   
   The first bus is a local bus, but we still specify a bus id for it.
   This will be the "public" bus id for the local bus, allowing it to 
   be reached from the network bus through NAT (network address translation).
   __________                 ________                 __________
  | LOCAL    |         Pin 7 | NAT    | Pin 12        | NETWORK  |
  | DEVICE   |_______________| SWITCH |_______________| DEVICE   |
  |__________|  Local bus    |________|  Bus 0.0.0.2  |__________| */

#include <PJONSwitch.h>  

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1, (uint8_t[4]){0,0,0,1}); // This is a local bus
PJONAny bus2(&link2, (uint8_t[4]){0,0,0,2});

PJONSwitch router(2, (PJONAny*[2]){&bus1, &bus2});

void setup() {
  // We did specify a bus id for the first bus, so it will be in network mode.
  // Switch it to local mode before starting, activating NAT.
  bus1.set_shared_network(false);

  link1.strategy.set_pin(7);
  link2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
}
