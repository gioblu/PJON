
/* Include Async ACK code setting INCLUDE_ASYNC_ACK as true before including PJON.h */
#define INCLUDE_ASYNC_ACK true

#include <PJON.h>

uint8_t bus_id[] = {0, 0, 0, 1};

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(bus_id, 44);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pins(11, 12);
  bus.begin();
  bus.set_receiver(receiver_function);
};

void receiver_function(uint8_t *payload, uint16_t length, const PacketInfo &packet_info) {
  Serial.print("Header: ");
  Serial.print(packet_info.header, BIN);
  // If packet formatted for a shared medium
  Serial.print(" Receiver bus id: ");
  Serial.print(packet_info.receiver_bus_id[0]);
  Serial.print(packet_info.receiver_bus_id[1]);
  Serial.print(packet_info.receiver_bus_id[2]);
  Serial.print(packet_info.receiver_bus_id[3]);
  Serial.print(" Device id: ");
  Serial.print(packet_info.receiver_id);
  // If sender info is included
  if((packet_info.header & SENDER_INFO_BIT) != 0) {
    Serial.print(" Sender bus id: ");
    Serial.print(packet_info.sender_bus_id[0]);
    Serial.print(packet_info.sender_bus_id[1]);
    Serial.print(packet_info.sender_bus_id[2]);
    Serial.print(packet_info.sender_bus_id[3]);
    Serial.print(" device id: ");
    Serial.println(packet_info.sender_id);
  }
  if(payload[0] == 'B') {
    digitalWriteFast(13, HIGH);
    delay(5);
    digitalWriteFast(13, LOW);
    delay(5);
  }
}

void loop() {
  bus.update();
  bus.receive(50000); // Receive for 50 milliseconds
};
