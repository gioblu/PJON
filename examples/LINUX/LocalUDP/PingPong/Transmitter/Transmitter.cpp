#define PJON_INCLUDE_LUDP
#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<LocalUDP> bus(45);

uint32_t cnt = 0;
uint32_t start = millis();

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
  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send_repeatedly(44, "P", 1, 10000); // Send P to device 44 repeatedly

  do loop(); while(true);
}
