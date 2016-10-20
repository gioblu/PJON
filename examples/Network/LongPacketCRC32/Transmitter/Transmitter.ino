
#define PACKET_MAX_LENGTH 325 // Make the buffer big enough
#define MAX_PACKETS         2 // Reduce number of packets not to empty memory

#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 45);

int packet;
char content[] = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();

  bus.set_error(error_handler);

  packet = bus.send(44, content, 300, bus.get_header() | EXTEND_HEADER_BIT);
  // Extend the header to 2 bytes, for simple feature testing
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
    packet = bus.send(44, content, 300, bus.get_header() | EXTEND_HEADER_BIT);

  bus.update();
};
