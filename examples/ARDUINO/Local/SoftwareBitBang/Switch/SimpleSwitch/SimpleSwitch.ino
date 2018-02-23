#define PJON_INCLUDE_SWBB

#include <PJONSimpleSwitch.h>

/* Route packets between a collection of buses with the same strategy.
   The SimpleSwitch does the same as the Switch but is limited to one 
   strategy and uses less memory. */

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