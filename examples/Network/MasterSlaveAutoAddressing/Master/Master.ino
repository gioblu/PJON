#include <PJONMaster.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};
uint32_t time;

// PJON object - The Master device id is fixed to PJON_MASTER_ID or 254
PJONMaster<SoftwareBitBang> bus(bus_id);

void setup() {
  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  Serial.begin(115200);
  bus.begin();
  time = millis();
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  uint32_t rid =
    (uint32_t)(payload[1]) << 24 |
    (uint32_t)(payload[2]) << 16 |
    (uint32_t)(payload[3]) <<  8 |
    (uint32_t)(payload[4]);

  Serial.print("Header: ");
  Serial.print(packet_info.header, BIN);
  Serial.print(" Length: ");
  Serial.print(length);
  Serial.print(" Sender id: ");
  Serial.print(packet_info.sender_id);
  Serial.print(" Request: ");
  Serial.print(payload[0]);
  Serial.print(" RID: ");
  Serial.print(rid);
  Serial.print(" Packet: ");
  for(uint8_t i = 0; i < length; i++) {
    Serial.print(payload[i]);
    Serial.print(" ");
  }
  Serial.print("Packets in buffer: ");
  Serial.println(bus.update());
}

void loop() {
  if(millis() - time > 5000) {
   for(uint8_t i = 0; i < MAX_DEVICES; i++) {
    if(bus.ids[i].state) {
      Serial.print("Device id: ");
      Serial.print(i + 1); // Shifted by one to avoid PJON_BROADCAST
      Serial.print(" Device rid: ");
      Serial.print(bus.ids[i].rid);
      Serial.print(" | ");
    }
   }
   Serial.println();
   time = millis();
  }
  bus.receive();
  bus.update();
};
