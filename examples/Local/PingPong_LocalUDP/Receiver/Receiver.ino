#include <PJON.h>

// Ethernet configuration for this device
byte gateway[] = { 192, 1, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte mac[] = {0xDE, 0x5D, 0x4E, 0xEF, 0xAE, 0xED};
uint8_t local_ip[] = { 192, 1, 1, 151 };

// <Strategy name> bus(selected device id)
PJON<LocalUDP> bus(44);

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver started.");
  Ethernet.begin(mac, local_ip, gateway, gateway, subnet);

  bus.begin();
  bus.set_receiver(receiver_function);
};

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'P') {
    cnt++;
    if (millis() - start > 1000) {
      start = millis();
      Serial.print("PING/s: "); Serial.println(cnt);
      cnt = 0;
    }
    bus.reply("P", 1);
  }
}

void loop() {
  bus.update();
  bus.receive();
};
