#include <PJON.h>

// Ethernet configuration for this device
byte gateway[] = { 192, 1, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte mac[] = {0xDA, 0x5A, 0x4E, 0xEF, 0xAE, 0xED};
uint8_t local_ip[] = { 192, 1, 1, 151 };

// Address of remote device
uint8_t remote_ip[] = { 192, 1, 1, 150 };

// <Strategy name> bus(selected device id)
PJON<GlobalUDP> bus(44);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'P') {
    cnt++;
    bus.reply((uint8_t *)"P", 1);
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver started.");
  Ethernet.begin(mac, local_ip, gateway, gateway, subnet);

  bus.strategy.add_node(45, remote_ip);
  bus.set_receiver(receiver_function);
  bus.begin();
};

void loop() {
  bus.receive();
  bus.update();

  if(millis() - start > 1000) {
    start = millis();
    Serial.print("PING/s: "); Serial.println(cnt);
    cnt = 0;
  }
};
