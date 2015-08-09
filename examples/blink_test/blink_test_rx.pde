#include <PJON.h>

float test;
float mistakes;

PJON network(A0, 44);

void setup() {
  Serial.begin(115200);
  network.set_acknowledge(true);
  network.set_collision_avoidance(true);
  network.set_encryption(false);
  network.insert_reaction('B', led_blink, false);
};

void led_blink() {
  Serial.println("BLINK");
  digitalWrite(13, HIGH);
  delay(30);
  digitalWrite(13, LOW);
}

void loop() {
    network.receive(1000);
};
