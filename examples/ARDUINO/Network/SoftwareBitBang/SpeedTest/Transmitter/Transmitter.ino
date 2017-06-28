#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 45);

int packet;
char content[] = "01234567890123456789";

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();

  bus.set_error(error_handler);

  packet = bus.send(44, content, 20);

  Serial.begin(115200);
}

void error_handler(uint8_t code, uint8_t data) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
}

void loop() {
  if(!bus.packets[packet].state)
    packet = bus.send(44, content, 20);

  bus.update();
};
