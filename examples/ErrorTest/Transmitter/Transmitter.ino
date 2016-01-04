#include <PJON.h>

PJON network(12, 45);

int packet;
char content[] = {125 , 0};

void setup() {
  Serial.begin(115200);
  packet = network.send(44, content, 1);
}

void loop() {
  content[1]++;
  if(!network.packets[packet].state)
    packet = network.send(44, content, 20);

  network.update();

  if(content[1] == 255)
    content[1] = 0;
};
