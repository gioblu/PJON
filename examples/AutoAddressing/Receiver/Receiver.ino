#include <PJON.h>

// network(Arduino pin used, selected device id)
PJON network(12, 44);

void setup() {
  Serial.begin(115200);
  network.set_receiver(receiver_function);
};

void receiver_function(uint8_t length, uint8_t *payload) {
  Serial.println(payload[0]);
}

void loop() {
  network.receive(1000);
};
