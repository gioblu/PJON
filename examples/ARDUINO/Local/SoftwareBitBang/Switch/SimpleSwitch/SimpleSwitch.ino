#define PJON_INCLUDE_SWBB

#include <PJONSimpleSwitch.h>

/* Route packets between a collection of buses with the same strategy.
   SimpleSwitch operates as Switch but is limited to one strategy although
   uses less memory.

   // SWITCH forwarding from bus 1 to bus 2 and vice versa
     __________                 ________                 __________
    |          | Bus 1  Pin 11 |        | Pin 12  Bus 2 |          |
    | DEVICE 1 | ______________| SWITCH |_______________| DEVICE 2 |
    |__________| Range 1-127   |________| Range 128-253 |__________| */

PJONBus<SoftwareBitBang> bus1(PJON_NOT_ASSIGNED, 1000, 2, 0), // Devices with id 1-127
                         bus2(PJON_NOT_ASSIGNED, 1000, 2, 1); // Devices with id 128-253

PJONSimpleSwitch<SoftwareBitBang> router(2, (PJONBus<SoftwareBitBang>*[2]){&bus1,&bus2});

void setup() {
  bus1.strategy.set_pin(11);
  bus2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
};