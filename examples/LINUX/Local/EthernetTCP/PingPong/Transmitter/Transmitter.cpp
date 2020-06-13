
#include <PJONEthernetTCP.h>

// Address of remote device
uint8_t remote_ip[] = { 192, 1, 1, 161 };

// <Strategy name> bus(selected device id)
PJONEthernetTCP bus(45);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if (payload[0] == 'P') cnt++;
}

void loop() {
  bus.update();
  bus.receive(10000);

  if (millis() - start > 1000) {
    start = millis();
    printf("PONG/s: %d, queue %d\n", cnt, bus.get_packets_count());
    cnt = 0;
  }
}

int main() {
  bus.strategy.link.set_id(bus.device_id());
  bus.strategy.link.add_node(44, remote_ip, 7044);
  bus.strategy.link.start_listening(7045);

  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send_repeatedly(44, "P", 1, 100000); // Send P to device 44 repeatedly

  while(true) loop();
  return 0;
}
