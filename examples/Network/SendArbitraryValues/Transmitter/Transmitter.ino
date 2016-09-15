#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};
unsigned long time;
int packet;

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 45);

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  Serial.begin(115200);
  time = millis();
  Serial.println("PJON - Integer cyclical sending test...");
}


void loop() {
  if(millis() - time > 5000) {
    time = millis();
    int temperature = analogRead(A0);
    //Send a "T", break the int in two bytes
    char content[3] = {'T', temperature >> 8, temperature & 0xFF};
    /* Use a blocking version of send. */
    packet = bus.send_packet_blocking(44, bus_id, content, 3);
    Serial.println((packet == ACK) ? "Transmitted" : "Some error occurred");
  }
};
