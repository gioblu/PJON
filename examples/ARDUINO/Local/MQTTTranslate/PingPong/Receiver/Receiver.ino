#define PJON_INCLUDE_MQTT
#include <PJON.h>

// Ethernet configuration for this device
byte mac[] = {0xDE, 0x5A, 0x4E, 0xEF, 0xAE, 0xED};
uint8_t broker_ip[] = { 192, 1, 1, 71 };

// <Strategy name> bus(selected device id)
PJON<MQTTTranslate> bus(44);

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
  while (Ethernet.begin(mac) == 0) delay(5000); // Wait for DHCP response
  //bus.strategy.set_topic("pjon");
  //bus.strategy.set_id(bus.device_id());
  bus.set_receiver(receiver_function);
  bus.strategy.set_address(broker_ip, 1883, "receiver");
  bus.begin();
//  bus.send_repeatedly(45, "P", 1, 100000); // Send P to device 45 repeatedly
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
