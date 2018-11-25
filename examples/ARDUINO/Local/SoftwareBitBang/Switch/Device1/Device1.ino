#include <PJON.h>

PJON<SoftwareBitBang> bus(100);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pin(7);
  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send(200, "B", 1);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if((char)payload[0] == 'B') {
    bus.reply("B", 1);
    static bool led_on = false;
    digitalWrite(LED_BUILTIN, led_on ? HIGH : LOW);
    led_on = !led_on;
  }
}

void loop() {
  bus.receive(2000);
  bus.update();
};
