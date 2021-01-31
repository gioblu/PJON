
// Uncomment to use the mode you prefer (default SWBB_MODE 1)
// #define SWBB_MODE 1 // 1.95kB/s - 15625Bd
// #define SWBB_MODE 2 // 2.21kB/s - 17696Bd
// #define SWBB_MODE 3 // 2.94kB/s - 23529Bd
// #define SWBB_MODE 4 // 3.40kB/s - 27210Bd

// Include only SoftwareBitBang

#include <PJONLocal.h> // Include PJONLocal
#include <strategies/SoftwareBitBang/SoftwareBitBang.h> // Include only SoftwareBitBang

PJONLocal<SoftwareBitBang> bus(44);

uint8_t data[PJON_PACKET_MAX_LENGTH];
PJON_Packet_Info info;

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off
}

void loop() {
  bus.receive(data, info);
};
