#include <PJONLocalUDP.h>

// Ethernet configuration for this device
byte gateway[] = { 192, 1, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte mac[] = {0xDA, 0xCA, 0x7E, 0xEF, 0xFE, 0x5D};
uint8_t local_ip[] = { 192, 1, 1, 150 };

// <Strategy name> bus(selected device id)
PJONLocalUDP bus(45);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'P') cnt++;
};

void setup() {
  Serial.begin(115200);
  Serial.println("Transmitter started.");
  Ethernet.begin(mac, local_ip, gateway, gateway, subnet);

  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send_repeatedly(44, "P", 1, 100000); // Send P to device 44 repeatedly
};

void loop() {
  bus.update();
  bus.receive();

  if(millis() - start > 1000) {
    start = millis();
    Serial.print("PONG/s: "); Serial.println(cnt);
    cnt = 0;
  }
};
