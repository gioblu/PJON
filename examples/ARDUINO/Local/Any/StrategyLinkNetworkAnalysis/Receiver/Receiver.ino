#include <PJON.h>

/* Defining strategies using StrategyLink template class it is possible to
   leverage of virtual inheritance to handle PJON instances dynamically, for
   example keeping an array of PJON instances using different strategies. */

StrategyLink<SoftwareBitBang> link;

PJON<Any> bus(44);

void setup() {
  link.strategy.set_pin(12);
  bus.strategy.set_link(&link);
  bus.begin();
};


void loop() {
  bus.receive();
};
