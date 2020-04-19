
// Include only SoftwareBitBang
#define PJON_INCLUDE_SWBB
// Set maximum packet length
#define PJON_PACKET_MAX_LENGTH 10
// Include PJONLocal
#include <PJONLocal.h>
// <Strategy name> bus(selected device id)
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
  if(bus.receive(data, info)) {
    uint8_t *payload = bus.get_payload(data);
    if(payload[0] == 'B') {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(30);
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
};
