
#include <PJONMQTTTranslate.h>

// Ethernet configuration for this device
byte mac[] = {0xDA, 0xCA, 0x7E, 0xEF, 0xFE, 0x5D};
uint8_t broker_ip[] = { 192, 1, 1, 71 };

// <Strategy name> bus(selected device id)
PJONMQTTTranslate bus(45);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'P') cnt++;
};

void setup() {
  Serial.begin(115200);
  Serial.println("Transmitter started.");
  while (Ethernet.begin(mac) == 0) delay(5000); // Wait for DHCP response
//  bus.strategy.set_topic("pjon");
//  bus.strategy.set_id(bus.device_id());
  bus.set_receiver(receiver_function);
  bus.strategy.set_address(broker_ip, 1883, "transmitter");
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
