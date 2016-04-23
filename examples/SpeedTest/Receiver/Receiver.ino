#include <PJON.h>
float test;

// network(Arduino pin used, selected device id)
PJON network(12, 44);

void setup() {
  Serial.begin(115200);
  network.set_receiver(receiver_function);
  Serial.println("Starting cyclic 1 second communication speed test...");
};

void receiver_function(uint8_t length, uint8_t *payload) {
 // Do nothing to avoid affecting speed analysis 
}

void loop() {
  long time = millis();
  int response = 0; 
  while(millis() - time < 1000) {
    response = network.receive(1000);
    if(response == ACK) test++;
  }
    
  Serial.print("Absolute com speed: ");
  Serial.print(test * 24);
  Serial.print(" B/s |Practical bandwidth: ");
  Serial.print(test * 20);
  Serial.print(" B/s |Packets sent: ");
  Serial.print(test);
  Serial.println();
  Serial.println();
  if(test == 0)
    Serial.println("Check wiring (Maybe you need a pull down resistor)! Execute NetworkAnalysis example to dig further.");

  test = 0; 
};
