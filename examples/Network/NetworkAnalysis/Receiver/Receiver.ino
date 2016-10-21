
// Uncomment to run SoftwareBitBang to mode FAST
// #define SWBB_MODE 2
// Uncomment to run SoftwareBitBang to mode OVERDRIVE
// #define SWBB_MODE 3

/*  Acknowledge Latency maximum duration (1000 microseconds default).
    Can be necessary to higher SWBB_LATENCY to leave enough time to receiver
    to compute the CRC and to respond with a synchronous acknowledgment.
    SWBB_LATENCY can be reduced to higher communication speed if
    devices are near and able to compute CRC fast enough. */
//#define SWBB_LATENCY 1000

#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 44);

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();

  Serial.begin(115200);
};

void loop() {
  bus.receive();
};
