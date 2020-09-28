
// Uncomment to use the mode you prefer (default SWBB_MODE 1)
// #define SWBB_MODE 1 // 1.95kB/s - 15625Bd
// #define SWBB_MODE 2 // 2.21kB/s - 17696Bd
// #define SWBB_MODE 3 // 2.94kB/s - 23529Bd
// #define SWBB_MODE 4 // 3.40kB/s - 27210Bd

#include <PJONSoftwareBitBang.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJONSoftwareBitBang bus(bus_id, 44);

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();

  Serial.begin(115200);
};

void loop() {
  bus.receive();
};
