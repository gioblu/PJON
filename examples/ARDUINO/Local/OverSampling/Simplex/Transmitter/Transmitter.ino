
#define OS_PREAMBLE_PULSE_WIDTH 0
/* Gain refresh time of SRX882 module is 100 milliseconds.
   If only one pair of SRX and STX are used to connect 2 devices in SIMPLEX
   mode, there is no need to refresh receiver's gain, being communication
   mono-directional. */

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(45);

char content[] = "01234567890123456789";

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
