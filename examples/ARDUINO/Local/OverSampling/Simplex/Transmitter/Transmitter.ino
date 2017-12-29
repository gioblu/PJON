
#define OS_GAIN_REFRESH_DELAY 0
/* Gain refresh time of SRX882 module is around 100 milliseconds.
   If only one pair of SRX and STX are used to connect 2 devices in SIMPLEX
   mode, there is no need to refresh receiver's gain, being communication
   mono-directional. */

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(45);

char content[] = "01234567890123456789";

void setup() {
  bus.set_communication_mode(PJON_SIMPLEX);
  bus.strategy.set_pins(PJON_NOT_ASSIGNED, 12);
  bus.begin();
  bus.send(44, content, 20);

  Serial.begin(115200);
  Serial.println("PJON - Network analysis");
  Serial.println("Starting a 1 second communication test..");
  Serial.println();
}

void loop() {
  if(!bus.update()) // If all packets are delivered, send another
    bus.send(44, content, 20);
};
