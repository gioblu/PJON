#define PJON_INCLUDE_ETCP
#define _WIN32
#include <PJON.h>

// Address of remote device
uint8_t remote_ip[] = { 192, 168, 1, 78 };

PJON<EthernetTCP> bus(45);

uint32_t count = 0;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	count++;
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    bus.send(44, (uint8_t *)"P", 1);
    printf("Error: packet lost\n");
  }
  if(code == PJON_PACKETS_BUFFER_FULL)
    printf("Error: packets' buffer full\n");
};

void loop() {
  bus.update();
  bus.receive();

  // Show information every second
  static uint32_t last = PJON_MILLIS();
  if ((uint32_t)(PJON_MILLIS() - last) >= 1000ul) {
    printf("PONG/s: %f\n", 1000.0f * float(count) / float((uint32_t)(PJON_MILLIS() - last)));
    last = PJON_MILLIS();
    count = 0;
  }
}

int main() {
  printf("Transmitter started.\n");
  bus.set_receiver(receiver_function);
  bus.strategy.link.set_id(bus.device_id());
  bus.strategy.link.add_node(44, remote_ip, 16001);
  bus.strategy.link.start_listening(16000);
  bus.begin();
  bus.send(44, (uint8_t *)"P", 1);

  while (true) loop();
  return 0;
}
