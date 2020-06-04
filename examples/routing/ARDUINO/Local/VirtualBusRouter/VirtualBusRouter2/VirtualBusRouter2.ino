#include <PJONVirtualBusRouter.h>

/* This sketch is routing between two local buses to form one larger local bus,
   just like the VirtualBusRouter example, but it uses the class 
   PJONVirtualBusRouter2 which is specialized to using two buses to provide
   a simpler declaration.

  // ROUTER forwarding from bus 1 to bus 2 and vice versa
    __________                 ________                 __________
   |          | Bus 1   Pin 7 |        | Pin 12  Bus 2 |          |
   | DEVICE 1 |_______________| ROUTER |_______________| DEVICE 2 |
   |__________|               |________|               |__________| */

PJONVirtualBusRouter2<SoftwareBitBang, OverSampling> router;

void setup() {
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_pin(12);
  router.set_virtual_bus(0);
  router.begin();
};

void loop() {
  router.loop();
};
