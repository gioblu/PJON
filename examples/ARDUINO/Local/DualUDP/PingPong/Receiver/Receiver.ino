#include <PJONDualUDP.h>

// Ethernet configuration for this device
byte mac[] = {0xDA, 0x5A, 0x4E, 0xEF, 0xAE, 0xED};


PJONDualUDP bus(44);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'P') {
    cnt++;
    bus.reply("P", 1);
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver started.");
  while (!Ethernet.begin(mac)); // Wait for DHCP

  bus.set_receiver(receiver_function);
  bus.begin();
};

void loop() {
  bus.update();
  bus.receive();

  if(millis() - start > 1000) {
    start = millis();
    Serial.print("PING/s: "); Serial.println(cnt);
    cnt = 0;
    Ethernet.maintain();
  }
}; 
