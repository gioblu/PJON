#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

int packet;
char content[] = "01234567890123456789";

void setup() {
  bus.set_pin(12);
  bus.begin();

  bus.set_error(error_handler);

  packet = bus.send(44, content, 20);

  Serial.begin(115200);
}

void error_handler(uint8_t code, uint8_t data) {
  if(code == CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(data);
  }
}

void loop() {
  if(!bus.packets[packet].state)
    packet = bus.send(44, content, 20);

  bus.update();
};
