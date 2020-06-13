
// Use the following define to activate debug
// #define MQTT_DEBUGPRINT


#include <PJONMQTTTranslate.h>

// Ethernet address of the broker
uint8_t broker_ip[] = { 127, 0, 0, 1 };

// <Strategy name> bus(selected device id)
PJONMQTTTranslate bus(45);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'P') cnt++;
};

void setup() {
  printf("Transmitter started.\n");
  bus.set_receiver(receiver_function);
  bus.strategy.set_address(broker_ip, 1883, "transmitter");
  bus.begin();
  bus.send_repeatedly(44, "P", 1, 10000); // Send P to device 44 repeatedly
};

void loop() {
  bus.update();
  bus.receive();

  if(millis() - start > 1000) {
    start = millis();
    printf("PONG/s: %d\n", cnt);
    cnt = 0;
  }
};

int main() {
  setup();
  while (true) loop();
  return 0;
}