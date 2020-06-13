
#include <PJONGlobalUP.h>

// <Strategy name> bus(selected device id)
PJONGlobalUDP bus(44);

//uint32_t millis() { return PJON_MICROS()/1000; } // TODO: Move to interface

uint32_t cnt = 0;
uint32_t start = millis();

// Address of remote devices
const uint8_t remote_ip[] = { 192, 1, 1, 150 };

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'P') {
    cnt++;
    bus.reply("P", 1);
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
  bus.strategy.add_node(45, remote_ip, 16000);
  bus.strategy.set_port(16001);
  bus.set_receiver(receiver_function);
  bus.begin();

  do loop(); while(true);
}
