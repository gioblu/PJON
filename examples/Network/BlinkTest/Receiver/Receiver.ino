#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 44);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pin(12);
  bus.begin();
  bus.set_receiver(receiver_function);

  Serial.begin(9600);
};

void receiver_function(uint8_t *payload, uint16_t length, const PacketInfo &packet_info) {
  Serial.print("Receiver bus id: ");
  Serial.print(packet_info.receiver_bus_id[0]);
  Serial.print(packet_info.receiver_bus_id[1]);
  Serial.print(packet_info.receiver_bus_id[2]);
  Serial.print(packet_info.receiver_bus_id[3]);
  Serial.print(" - device id: ");
  Serial.println(packet_info.receiver_id);

  Serial.print("Sender bus id: ");
  Serial.print(packet_info.sender_bus_id[0]);
  Serial.print(packet_info.sender_bus_id[1]);
  Serial.print(packet_info.sender_bus_id[2]);
  Serial.print(packet_info.sender_bus_id[3]);
  Serial.print(" - device id: ");
  Serial.println(packet_info.sender_id);

  if(payload[0] == 'B') {
    Serial.println(" BLINK!");
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
}

void loop() {
  bus.receive(1000);
};
