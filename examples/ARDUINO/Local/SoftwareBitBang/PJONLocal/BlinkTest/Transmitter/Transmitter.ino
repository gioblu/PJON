
// Include only SoftwareBitBang
#define PJON_INCLUDE_SWBB
// Set maximum packet length
#define PJON_PACKET_MAX_LENGTH 10
// Include PJONLocal
#include <PJONLocal.h>

/* Create PJONLocal object:
   Class name <Strategy name> instance name ( device id ) */
PJONLocal<SoftwareBitBang> bus(45);

// Buffer used by PJONLocal
uint8_t packet[PJON_PACKET_MAX_LENGTH];

uint32_t time = 0;

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
}

void loop() {
  if(((millis() - time) > 1000) && bus.ready_to_send())
    if(bus.send_packet(44, packet, "B", 1) == PJON_ACK)
      time = millis();
};
