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
#include <PJONSoftwareBitBang.h>

PJONSwitch2<SoftwareBitBang, SoftwareBitBang> router;

void setup() {
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_pin(12);
  router.get_bus(0).set_bus_id((const uint8_t[4]){0, 0, 0, 1});
  router.get_bus(1).set_bus_id((const uint8_t[4]){0, 0, 0, 2});

  // We did specify a bus id for the first bus, so it will be in network mode.
  // Switch it to local mode before starting, activating NAT.
  router.get_bus(0).set_shared_network(false);  
  router.begin();
}

void loop() {
  router.loop();
}
