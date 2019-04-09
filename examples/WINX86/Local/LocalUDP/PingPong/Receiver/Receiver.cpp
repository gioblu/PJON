#define PJON_INCLUDE_LUDP
#include <PJON.h>

PJON<LocalUDP> bus(44);

uint32_t count = 0;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	bus.reply((uint8_t *)"P", 1);
	count++;
 };

void loop() {
  bus.update();
  bus.receive();

  // Write status every second
  static uint32_t last = PJON_MILLIS();
  if ((uint32_t)(PJON_MILLIS() - last) >= 1000) {
    printf("PING/s: %f\n", 1000.0f * float(count) / float((uint32_t)(PJON_MILLIS() - last)));
    last = PJON_MILLIS();
    count = 0;
  }
}

int main() {
  printf("Receiver started.\n");
  bus.set_receiver(receiver_function);
  bus.begin();
  while (true) loop();
  return 0;
}
