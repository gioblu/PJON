#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(45);

int packet;
char content[] = "01234567890123456789";

void setup() {
  bus.set_pin(12);

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
