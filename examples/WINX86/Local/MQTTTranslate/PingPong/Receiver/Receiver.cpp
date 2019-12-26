#define PJON_INCLUDE_ETCP
//#define MQTT_DEBUGPRINT
#include <PJON.h>
#include <strategies/MQTTTranslate/MQTTTranslate.h>

// Ethernet address of the broker
uint8_t broker_ip[] = { 127, 0, 0, 1 };

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
  printf("Receiver started.\n");
  bus.set_receiver(receiver_function);
  bus.strategy.mqttclient.set_address(broker_ip, 1883, "receiver");
  bus.begin();
};

void loop() {
  bus.receive();
  bus.update();

  if(millis() - start > 1000) {
    start = millis();
    printf("PING/s: %d\n", cnt);
    cnt = 0;
  }
};

int main() {
  setup();
  while (true) loop();
  return 0;
}