
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

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

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
  if(code == CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(data);
  }
};

void loop() {
  if(!bus.packets[packet].state)
    packet = bus.send(44, content, 20);

  bus.update();
};
