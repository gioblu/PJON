
#define MQTTT_MODE MQTTT_MODE_MIRROR_DIRECT

#define PJON_MAX_PACKETS 2
#include <PJONMQTTTranslate.h>
#include <PJONSoftwareBitBang.h>

// Ethernet configuration for this device
byte mac[] = {0xDA, 0xCA, 0x7E, 0xEF, 0xFE, 0x5D};
uint8_t broker_ip[] = { 192, 168, 1, 71 };


#define PJON_GATEWAY_ID 254
PJONSoftwareBitBang bus(PJON_GATEWAY_ID);
PJONMQTTTranslate mqtt;

uint32_t cnt_to_mqtt = 0, cnt_from_mqtt = 0;
uint32_t start = millis();

// Pick up all packets flowing in the bus and forward to MQTT?
// Otherwise only packets sent to this gateway will be forwarded.
bool router = false;

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  const char *msg = (const char *)payload;
  // Forward to MQTT topic pjon/device45/output if from device 45
  mqtt.forward(packet_info, payload, length);
  cnt_to_mqtt++;
};

void receiver_function_mqtt(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  // Send to device 45 if source topic is pjon/device45/input
  bus.send(packet_info.rx.id, payload, length);
  cnt_from_mqtt++;
};

void setup() {
  Serial.begin(115200);
  Serial.println("SWBB-MQTT Gateway started.");
  while (Ethernet.begin(mac) == 0) delay(500); // Wait for DHCP response

  bus.strategy.set_pin(7);
  bus.set_receiver(receiver_function);
  bus.set_router(router);
  bus.begin();

  mqtt.set_receiver(receiver_function_mqtt);
  mqtt.strategy.set_address(broker_ip, 1883, "gateway");
  #if (MQTTT_MODE == MQTTT_MODE_MIRROR_TRANSLATE)
  mqtt.strategy.add_translation("P","pressure");
  mqtt.strategy.add_translation("T","temperature");
  #endif
  mqtt.strategy.set_subscribe_all(true); // Subscribe to all devices' input topics
  mqtt.set_router(true);
  mqtt.begin();
};

void loop() {
  bus.update();
  bus.receive(1000);
  mqtt.update();
  mqtt.receive();

  if(millis() - start > 1000) {
    start = millis();
    Serial.print("Routed packets to MQTT: "); Serial.print(cnt_to_mqtt);
    Serial.print(", from MQTT: "); Serial.println(cnt_from_mqtt);
    cnt_from_mqtt = cnt_to_mqtt = 0;
  }
};
