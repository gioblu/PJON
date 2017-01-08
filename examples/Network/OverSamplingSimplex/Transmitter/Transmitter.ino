
#define OS_GAIN_REFRESH_DELAY 0
/* Gain refresh time of SRX882 module is around 100 milliseconds.
   If only one pair of SRX and STX are used to connect 2 devices in SIMPLEX
   mode, there is no need to refresh receiver's gain, being communication
   mono-directional. */

#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<OverSampling> bus(bus_id, 45);

int packet;
char content[] = "01234567890123456789";

void setup() {
  bus.set_communication_mode(SIMPLEX);
  bus.strategy.set_pins(NOT_ASSIGNED, 12);

  bus.begin();
  packet = bus.send(44, content, 20);

  Serial.begin(115200);
  Serial.println("PJON - Network analysis");
  Serial.println("Starting a 1 second communication test..");
  Serial.println();
}

void loop() {
  bus.update();

  if(!bus.packets[packet].state)
    packet = bus.send(44, content, 20);
};
