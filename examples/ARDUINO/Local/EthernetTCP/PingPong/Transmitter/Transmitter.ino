#include <PJON.h>

// Ethernet configuration for this device
byte gateway[] = { 192, 1, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte mac[] = {0xDE, 0xCD, 0x7E, 0xEF, 0xFE, 0x5D};
uint8_t local_ip[] = { 192, 1, 1, 145 },
        remote_ip[] = { 192, 1, 1, 144 };

// <Strategy name> bus(selected device id)
PJON<EthernetTCP> bus(45);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'P') cnt++;
};

void setup() {
  Serial.begin(115200);
  Serial.println("Transmitter started.");
  Ethernet.begin(mac, local_ip, gateway, gateway, subnet);

  bus.strategy.link.set_id(bus.device_id());
  bus.strategy.link.add_node(44, remote_ip);
  bus.strategy.link.start_listening();

  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send_repeatedly(44, "P", 1, 10000); // Send P to device 44 repeatedly
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
