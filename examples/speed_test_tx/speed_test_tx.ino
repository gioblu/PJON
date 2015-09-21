#include <PJON.h>

PJON network(A0, 45);
int packet;
char content[] = "01234567890123456789";

void setup() {
  Serial.begin(115200);
  packet = network.send(44, content);
  network.set_exception(exception_handler);
}

static void exception_handler(uint8_t data, uint8_t motivation) {
  if(motivation == CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(data);
  }
}

void loop() {
  if(!network.packets[packet].state) 
    packet = network.send(44, content);
		  
  network.update();
};