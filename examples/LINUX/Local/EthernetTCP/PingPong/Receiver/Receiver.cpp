
#include <PJONEthernetTCP.h>

uint8_t remote_ip[] = { 192, 1, 1, 161 };

// <Strategy name> bus(selected device id)
PJONEthernetTCP bus(44);


uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'P') {
    cnt++;
    bus.reply("P", 1);
  }
}

void loop() {
  bus.update();
  bus.receive(1000);

  if (millis() - start > 1000) {
    start = millis();
    printf("PING/s: %d, queue %d\n", cnt, bus.get_packets_count());
    cnt = 0;
  }
};

int main() {
  bus.strategy.link.set_id(bus.device_id());
  bus.strategy.link.add_node(45, remote_ip, 7045);
  bus.strategy.link.start_listening(7044);

  bus.set_receiver(receiver_function);
  bus.begin();

  while(true) loop();
  return 0;
};
