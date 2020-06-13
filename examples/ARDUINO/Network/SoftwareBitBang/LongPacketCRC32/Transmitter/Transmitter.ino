
#define PJON_PACKET_MAX_LENGTH 325 // Make the buffer big enough
#define PJON_MAX_PACKETS         2 // Reduce number of packets not to empty memory

/* Response timeout duration duration (1500 microseconds default).
   SWBB_RESPONSE_TIMEOUT duration must be long enough for the receiver to have the
   time to compute the CRC and to respond with a synchronous acknowledgement.
   Below SWBB_RESPONSE_TIMEOUT is set to 4 milliseconds to give enough time to
   the receiver to compute CRC32 of a 300 bytes string. */
#define SWBB_RESPONSE_TIMEOUT 4000

#include <PJONSoftwareBitBang.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJONSoftwareBitBang bus(bus_id, 45);

uint8_t content[] = "012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789";

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
  bus.send(44, content, 300);

  Serial.begin(115200);
};

void loop() {
  if(!bus.update()) // If all packets are delivered, send another
    bus.send(44, content, 300);
};
