
#include <PJONGlobalUDP.h>

// IP Address of the receiver
uint8_t remote_ip[] = {127, 0, 0, 1};

PJONGlobalUDP bus(44);

uint32_t count = 0;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	bus.reply("P", 1);
	count++;
 };

void loop() {
  bus.update();
  bus.receive();

  // Show information every second
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
  bus.strategy.add_node(45, remote_ip, 16000);
  bus.strategy.set_port(16001);
  bus.begin();

  while (true) loop();
  return 0;
}
