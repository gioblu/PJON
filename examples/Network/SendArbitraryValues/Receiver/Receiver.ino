#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 44);

void setup() {
  bus.set_pin(12);
  bus.begin();

  bus.set_receiver(receiver_function);

  Serial.begin(115200);
};

void receiver_function(uint8_t id, uint8_t *payload, uint8_t length) {
  Serial.print("Message received for bus id: ");
  Serial.print(payload[0]);
  Serial.print(payload[1]);
  Serial.print(payload[2]);
  Serial.print(payload[3]);
  Serial.print(" - device id: ");
  Serial.print(id);

  if((char)payload[4] == 'T') {
    Serial.print("Temperature received: ");
    Serial.print(payload[1] << 8 | payload[2] & 0xFF);
    // Get back the original integer from the 2 separate bytes
  }
  Serial.println();
}

void loop() {
  bus.receive(1000);
};
