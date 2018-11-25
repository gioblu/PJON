#define OS_PREAMBLE_PULSE_WIDTH 0
/* Gain refresh time of SRX882 module is around 100 milliseconds.
   If only one pair of SRX and STX are used to connect 2 devices in SIMPLEX
   mode, there is no need to refresh receiver's gain, being communication
   mono-directional. */

#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<OverSampling> bus(bus_id, 45);

char content[] = "01234567890123456789";

void setup() {
  Serial.begin(115200);

  // Connect transmitter module data input pin to Arduino pin 7
  bus.strategy.set_pins(PJON_NOT_ASSIGNED, 7);

  bus.set_communication_mode(PJON_SIMPLEX);
  bus.begin();
  bus.send(44, content, 20);
}

void loop() {
  if(!bus.update()) // If all packets are delivered, send another
    bus.send(44, content, 20);
};
