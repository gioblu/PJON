#include <PJON.h>

float test;

// bus(Arduino pin used, selected device id)
PJON bus(12, 44);

void setup() {
  bus.begin(); // Initialize PJON network
  bus.set_receiver(receiver_function);

  Serial.begin(115200);
};

void receiver_function(uint8_t length, uint8_t *payload) {
 // Do nothing to avoid affecting speed analysis
}

void loop() {
  Serial.println("Starting 1 second communication speed test...");
  long time = millis();
  int response = 0;
  while(millis() - time < 1000) {
    response = bus.receive(1000);
    if(response == ACK)
      test++;
  }

  Serial.print("Absolute com speed: ");
  Serial.print(test * 24);
  Serial.print(" B/s |Practical bandwidth: ");
  Serial.print(test * 20);
  Serial.print(" B/s |Packets sent: ");
  Serial.println(test);
  Serial.println();
  if(test < 50)
    Serial.println("Check wiring! Maybe you need a pull down resistor.");

  test = 0;
};
