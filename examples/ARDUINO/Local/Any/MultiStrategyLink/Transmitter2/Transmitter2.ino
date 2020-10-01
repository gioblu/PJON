
// Transmission speed modes (see Timing.h)
// #define SWBB_MODE 1 // 1.95kB/s - 15625Bd
// #define SWBB_MODE 2 // 2.21kB/s - 17696Bd
// #define SWBB_MODE 3 // 2.94kB/s - 23529Bd
// #define SWBB_MODE 4 // 3.40kB/s - 27210Bd

#include <PJONAny.h>


PJON<SoftwareBitBang> bus(1);

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
};

void setup() {
  bus.strategy.set_pin(12);
  bus.set_error(error_handler);
  bus.begin();
  bus.send_repeatedly(45, "B", 1, 500000);
  Serial.begin(115200);
}

void loop() {
  bus.update();
};
