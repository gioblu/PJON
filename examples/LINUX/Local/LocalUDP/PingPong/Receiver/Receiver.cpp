#define PJON_INCLUDE_LUDP
#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<LocalUDP> bus(44);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'P') {
    cnt++;
    bus.reply((uint8_t *)"P", 1);
  }
}

void loop() {
  bus.receive();
  bus.update();

  if (millis() - start > 1000) {
    start = millis();
    printf("PING/s: %d\n", cnt);
    cnt = 0;
  }
}

int main() {
  bus.set_receiver(receiver_function);
  bus.begin();

  do loop(); while(true);
}
