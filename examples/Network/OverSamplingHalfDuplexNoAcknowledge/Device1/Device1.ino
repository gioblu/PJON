#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<OverSampling> bus(bus_id, 44);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pins(11, 12);
  bus.set_synchronous_acknowledge(false);
  bus.set_receiver(receiver_function);

  bus.begin();

  bus.send_repeatedly(45, "B", 1, 1000000);

  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
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

  if((char)payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(5);
    digitalWrite(13, LOW);
    delay(5);
  }
}

void loop() {
  bus.receive();
  bus.update();
};
