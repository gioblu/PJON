#include <PJON.h>

PJON network(A0, 45);
int packet;
char content[] = "01234567890123456789";

void setup() {
  Serial.begin(115200);
  packet = network.send(44, content);
  network.set_error(error_handler);
}

static void error_handler(uint8_t code, uint8_t data) {
  if(code == CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(data);
  }
}

void loop() {
  if(!network.packets[packet].state) 
    packet = network.send(44, content);
		  
  network.update();
};