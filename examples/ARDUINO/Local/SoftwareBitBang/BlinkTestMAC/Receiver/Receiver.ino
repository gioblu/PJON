#define PJON_INCLUDE_SWBB true
#define PJON_INCLUDE_MAC true
#include <PJON.h>

// <Strategy name> bus(mac address of the network interface)
const uint8_t mac[6] = {1, 2, 3, 4, 5, 6};
PJON<SoftwareBitBang> bus(mac);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    Serial.print("BLINK sent by MAC ");
    for(uint8_t i = 0; i < 6; i++)
      Serial.print(packet_info.tx.mac[i], DEC);
    Serial.println();
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
    digitalWrite(LED_BUILTIN, LOW);
  }
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off
  bus.strategy.set_pin(12);
  bus.begin();
  bus.set_receiver(receiver_function);
  Serial.begin(115200);
};

void loop() {
  bus.receive(1000);
};
