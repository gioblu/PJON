#include <PJON.h>

// <Strategy name> bus(selected device id
PJON<SoftwareBitBang> bus(44);

void setup() {
  bus.set_pin(12);
  bus.begin();
  bus.set_receiver(receiver_function);

  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint8_t length, const PacketInfo &packet_info) {
  Serial.println(payload[0]);
}

void loop() {
  bus.receive(1000);
};
