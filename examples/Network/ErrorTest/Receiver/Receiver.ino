#include <PJON.h>

float undetected_errors;
float count;

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

void receiver_function(uint8_t *payload, uint8_t length, const PacketInfo &packet_info) {

  if(length != 8) { // Undetected error in length byte
    Serial.print("L ");
    Serial.print(length);
    Serial.print(" | ");
    undetected_errors++;
  }

  if(
    payload[4] != payload[5] ||
    payload[5] != payload[6] ||
    payload[4] != payload[6] ||
    payload[7] != 0
  ) {
    undetected_errors++;
    Serial.print(payload[4]);
    Serial.print(" ");
    Serial.print(payload[5]);
    Serial.print(" | ");
    Serial.print(payload[6]);
    Serial.print(" | ");
    Serial.println(payload[7]);
  }

  count++;
}

void loop() {
  Serial.println();
  Serial.println("PJON - Starting 20 seconds error rate test...");

  long time = millis();
  int response = 0;

  while(millis() - time < 20000)
    response = bus.receive(1000);

  Serial.println();
  Serial.print("Undetected errors: ");
  Serial.print(undetected_errors);
  Serial.print(" Transmission total (kB): ");
  Serial.println((count * 7) / 1000);
  Serial.print("BER or Bit Error Rate expressed in b/s: ");
  Serial.println(((undetected_errors / count) / 8) / 20, 6);
  Serial.println();

  count = 0;
  undetected_errors = 0;
}
