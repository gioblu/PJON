
// Uncomment to use the mode you prefer (default SWBB_MODE 1)
// #define SWBB_MODE 1 // 1.95kB/s - 15625Bd
// #define SWBB_MODE 2 // 2.21kB/s - 17696Bd
// #define SWBB_MODE 3 // 2.94kB/s - 23529Bd
// #define SWBB_MODE 4 // 3.40kB/s - 27210Bd

/*  Response timeout (1500 microseconds default).
    If the acknowledgement fails SWBB_RESPONSE_TIMEOUT may be too short
    specially if long packets are sent or if devices are far from each other */

//#define SWBB_RESPONSE_TIMEOUT 1500

// Include packet id feature
#define PJON_INCLUDE_PACKET_ID

#include <PJONSoftwareBitBang.h>


PJONSoftwareBitBang bus(45);

int packet;
uint8_t content[] = "01234567890123456789";

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
};

void setup() {
  bus.set_packet_id(true);
  bus.strategy.set_pin(12);
  bus.set_error(error_handler);
  bus.begin();

  packet = bus.send(44, content, 20);
  Serial.begin(115200);
}

void loop() {
  if(!bus.update()) packet = bus.send(44, content, 20);
};
