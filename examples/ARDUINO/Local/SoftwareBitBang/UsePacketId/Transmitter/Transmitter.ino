// Include packet id feature
#define PJON_INCLUDE_PACKET_ID true

// Uncomment to run SoftwareBitBang in MODE 2
// #define SWBB_MODE 2
// Uncomment to run SoftwareBitBang in MODE 3
// #define SWBB_MODE 3

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

int packet;
char content[] = "01234567890123456789";

void setup() {
  bus.set_packet_id(true);
  bus.strategy.set_pin(12);
  bus.set_error(error_handler);
  bus.begin();

  packet = bus.send(44, content, 20);
  Serial.begin(115200);
}

void error_handler(uint8_t code, uint8_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
};

void loop() {
  if(!bus.update()) packet = bus.send(44, content, 20);
};
