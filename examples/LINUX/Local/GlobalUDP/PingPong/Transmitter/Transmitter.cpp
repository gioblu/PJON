#define PJON_INCLUDE_GUDP
#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<GlobalUDP> bus(45);

uint32_t cnt = 0;
uint32_t start = millis();

// Address of remote device
const uint8_t remote_ip[] = { 192, 1, 1, 151 };

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if (payload[0] == 'P') cnt++;
}

void loop() {
  bus.update();
  bus.receive();

  if (millis() - start > 1000) {
    start = millis();
    printf("PONG/s: "); printf("%d\n", cnt);
    cnt = 0;
  }
};

int main() {
  bus.strategy.add_node(44, remote_ip, 16001);
  bus.strategy.set_port(16000);
  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send_repeatedly(44, (uint8_t *)"P", 1, 10000); // Send P to device 44 repeatedly

  do loop(); while(true);
}
