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
  bus.strategy.set_pin(12);

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
