#define PJON_INCLUDE_ETCP
#include <PJON.h>

// Address of remote device
uint8_t remote_ip[] = { 192, 1, 1, 34 };

PJON<EthernetTCP> bus(45);

uint32_t count = 0;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	count++;
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
  bus.send_repeatedly(44, "P", 1, 1000);

  while (true) loop();
  return 0;
}
