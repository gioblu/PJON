#include <PJON.h>

// network(Arduino pin used, selected device id)
PJON network(12, 44);

void setup() {
  Serial.begin(115200);
  network.set_receiver(receiver_function);
};

static void receiver_function(uint8_t length, uint8_t *payload) {
 if((char)payload[0] == 'T') {
    Serial.print("Temperature received: ");
    Serial.print(payload[1] << 8 | payload[2] & 0xFF);
    // Get back the original integer from the 2 separate bytes
 }
 Serial.println();
}

void loop() {
  network.receive(1000);
};
