#include <PJON.h>

PJON network(12, 45);

int packet;
char content[] = {125 , 0};
// Content with contant and changing byte

void setup() {
  Serial.begin(115200);
  packet = network.send(44, content, 2);
}

void loop() {
  content[1]++;
  if(!network.packets[packet].state) {
    delayMicroseconds(random(1000, 2500)); // Add a random delay to simulate standard usage
    packet = network.send(44, content, 2);
  }
  network.update();

  if(content[1] == 255)
    content[1] = 0;
};
