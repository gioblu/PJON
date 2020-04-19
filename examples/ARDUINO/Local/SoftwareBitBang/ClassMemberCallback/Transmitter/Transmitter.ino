
// Uncomment to run SoftwareBitBang in MODE 2
// #define SWBB_MODE 2
// Uncomment to run SoftwareBitBang in MODE 3
// #define SWBB_MODE 3

/*  Acknowledge Latency maximum duration (1000 microseconds default).
    Can be necessary to higher SWBB_RESPONSE_TIMEOUT to leave enough time to
    receiver to compute the CRC and to respond with a synchronous acknowledgement
    SWBB_RESPONSE_TIMEOUT can be reduced to higher communication speed if
    devices are near and able to compute CRC fast enough. */
//#define SWBB_RESPONSE_TIMEOUT 1000

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

int packet;
uint8_t content[] = "01234567890123456789";

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
};

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  bus.set_error(error_handler);
  Serial.begin(115200);

  packet = bus.send(44, content, 20);
};

void loop() {
  if(!bus.update())
    packet = bus.send(44, content, 20);
};
