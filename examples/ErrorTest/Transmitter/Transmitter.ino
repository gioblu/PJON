#include <PJON.h>

PJON network(12, 45);

int packet;
char content[4] = {0, 0, 0, 0};
// Content with contant and changing byte

void setup() {
  Serial.begin(115200);
  packet = network.send(44, content, 4);
}

void loop() {
  content[0] += 1;
  content[1] += 1;
  content[2] += 1;

  if(!network.packets[packet].state)
    packet = network.send(44, content, 4);

  network.update();

  if(content[0] == 255) content[0] = 0;
  if(content[1] == 255) content[1] = 0;
  if(content[2] == 255) content[2] = 0;
};
