#include <PJON.h>

// Ethernet configuration for this device
byte mac[] = {0xDA, 0xCA, 0x7E, 0xEF, 0xFE, 0x5D};

// <Strategy name> bus(selected device id)
PJON<DualUDP> bus(45);

uint32_t cnt = 0, wait_for_reply = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'P') { cnt++; wait_for_reply = 0; }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Transmitter started.");
  while (!Ethernet.begin(mac)); // Wait for DHCP

  bus.set_receiver(receiver_function);
  bus.begin();
};

void loop() {
  bus.update();
  bus.receive();

  if (bus.get_packets_count() == 0 && !wait_for_reply) {
    wait_for_reply = 1;
    bus.send(44, "P", 1); // Send P to device 44 repeatedly
  }

  if(millis() - start > 1000) {
    start = millis();
    Serial.print("PONG/s: "); Serial.println(cnt);
    cnt = 0;
    Ethernet.maintain();
  }
};