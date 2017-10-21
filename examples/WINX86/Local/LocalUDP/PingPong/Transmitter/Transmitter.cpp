#define PJON_INCLUDE_LUDP
#include <PJON.h>

PJON<LocalUDP> bus(45);

uint32_t count = 0;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	count++;
};

void loop() {
  bus.update();
  bus.receive();

  // Write status every second
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
  bus.begin();
  bus.send_repeatedly(44, "P", 1, 10000);
  while (true) loop();
  return 0;
}
