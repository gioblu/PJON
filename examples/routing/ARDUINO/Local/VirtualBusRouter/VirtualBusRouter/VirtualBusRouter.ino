#include <PJONVirtualBusRouter.h>

/* This sketch is routing between two local buses to form one larger local bus. 
   It will discover which attached bus each device is connected to, and route 
   packets dynamically.
   Devices can be placed on any of the attached buses as long as device
   ids are unique, forming a "virtual bus".
   Using this router class, two or more physical buses can be merged.
   It can also be used as a range extender.

   Note that this example demonstrates the general way which allows any
   number of buses to be connected. For the simple two-bus case there is
   a specialized class allowing for less code. See the example
   VirtualBusRouter2.ino for this.

  // ROUTER forwarding from bus 1 to bus 2 and vice versa
    __________                 ________                 __________
   |          | Bus 1   Pin 7 |        | Pin 12  Bus 2 |          |
   | DEVICE 1 |_______________| ROUTER |_______________| DEVICE 2 |
   |__________|               |________|               |__________| */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1);
PJONAny bus2(&link2);

PJONVirtualBusRouter<PJONSwitch> router(2, (PJONAny*[2]){&bus1, &bus2});

void setup() {
  link1.strategy.set_pin(7);
  link2.strategy.set_pin(12);
  router.set_virtual_bus(0);
  router.begin();
};

void loop() {
  router.loop();
};
