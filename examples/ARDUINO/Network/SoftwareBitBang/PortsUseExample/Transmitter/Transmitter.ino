#define PJON_INCLUDE_PORT
#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};
// PJON object
PJON<SoftwareBitBang> bus(bus_id, 45);
int packet;
uint8_t content[] = "01234567890123456789";

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
};

void setup() {
  bus.begin();
  /* Include a custom port, only packet including port 8001
     are received others are filtered out. */
  bus.include_port(true, 8001);
  bus.strategy.set_pin(12);
  bus.set_error(error_handler);

  Serial.begin(115200);
  packet = bus.send(44, content, 20);

  /* This packet will be transmitted but ignored by recipient because
     configured on port 8002 */
  bus.send(44, content, 20, bus.config, 0, 8002);
};

void loop() {
  if(!bus.update()) // If all packets are delivered, send another
    packet = bus.send(44, content, 20);
};
