#include <PJON.h>

byte index;
float undetected_errors;
float count;
byte last;
boolean started = false;

// network(Arduino pin used, selected device id)
PJON network(12, 44);

void setup() {
  Serial.begin(115200);
  network.set_receiver(receiver_function);
};

void receiver_function(uint8_t length, uint8_t *payload) {
  if(!started) {
    started = true;
    last = payload[1];
    return;
  }

  if(last == 255) {
    if(payload[1] != 0) {
     undetected_errors++;
     Serial.print("Overflow, last was 255 ");
     Serial.println(payload[1]);
    }
  } else {
    if(last != payload[1] - 1) {
      undetected_errors++;
      Serial.print(last + 1);
      Serial.print(" ");
      Serial.print(payload[1]);
      Serial.print(" | ");
    }
  }
  last = payload[1];

  if(index == 255)
    index = 0;
  else index++;

  count++;
}

void loop() {
  Serial.println();
  Serial.println("PJON v1.1 - Starting 10 seconds error rate test...");
  long time = millis();
  int response = 0;
  while(millis() - time < 10000)
    response = network.receive(1000);

  Serial.print("Undetected errors: ");
  Serial.print(undetected_errors);
  Serial.print(" Transmission total count: ");
  Serial.println(count);
  Serial.print("BER or Bit Error Rate expressed in bit/s: ");
  Serial.println(((undetected_errors / count) * 8) / 10, 4);
  Serial.println();

  count = 0;
  index = 0;
  undetected_errors = 0;
};
