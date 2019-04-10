
#define OS_PREAMBLE_PULSE_WIDTH 0
/* If only one pair of SRX and STX are used to connect 2 devices in SIMPLEX
   mode, being communication mono-directional, there is no need to include
   the preamble pulse. */

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(45);

uint8_t content[] = "01234567890123456789";

void setup() {
  // Connect STX882 data output pin in Arduino pin 7
  bus.strategy.set_pins(PJON_NOT_ASSIGNED, 7);

  bus.set_communication_mode(PJON_SIMPLEX);
  bus.begin();

  bus.send(44, content, 20);
}

void loop() {
  if(!bus.update()) // If all packets are delivered, send another
    bus.send(44, content, 20);
};
