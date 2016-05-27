#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<OverSampling> bus(bus_id, 45);

void setup() {
  pinModeFast(13, OUTPUT);
  digitalWriteFast(13, LOW); // Initialize LED 13 to be off

  bus.set_acknowledge(false);
  bus.set_pins(11, 12);
  bus.set_receiver(receiver_function);

  bus.begin();

  bus.send(44, "B", 1, 1000000);

  Serial.begin(115200);
};

void receiver_function(uint8_t id, uint8_t *payload, uint8_t length) {
  Serial.print("Message received for bus id: ");
  Serial.print(payload[0]);
  Serial.print(payload[1]);
  Serial.print(payload[2]);
  Serial.print(payload[3]);
  Serial.print(" - device id: ");
  Serial.println(id);

  if((char)payload[4] == 'B') {
    digitalWrite(13, HIGH);
    delay(5);
    digitalWrite(13, LOW);
    delay(5);
  }
}

void loop() {
  bus.receive();
  bus.update();
};
