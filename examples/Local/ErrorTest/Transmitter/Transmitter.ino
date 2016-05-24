#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

int packet;
char content[4] = {0, 0, 0, 0};
// Content with contant and changing byte

void setup() {
  bus.set_pin(12);
  bus.begin();

  packet = bus.send(44, content, 4);

  Serial.begin(115200);
}

void loop() {
  content[0] += 1;
  content[1] += 1;
  content[2] += 1;

  if(!bus.packets[packet].state)
    packet = bus.send(44, content, 4);

  bus.update();

  if(content[0] == 255) content[0] = 0;
  if(content[1] == 255) content[1] = 0;
  if(content[2] == 255) content[2] = 0;
};
