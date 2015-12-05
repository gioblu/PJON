#include <PJON.h>

float test;
float mistakes;

// network(Arduino pin used, selected device id)
PJON network(12, 44);

void setup() {
  Serial.begin(115200);
  network.set_receiver(receiver_function);
};

void receiver_function(uint8_t length, uint8_t *payload) {
 // Do nothing to avoid affecting speed analysis 
}

void loop() {
  Serial.println("Starting 10 seconds communication speed test...");
  long time = millis();
  int response = 0; 
  while(millis() - time < 10000) {
    response = network.receive(1000);
    if(response == ACK)
      test++;
    if(response == NAK)
      mistakes++;
  }
    
  Serial.print("Absolute com speed: ");
  Serial.print( (test * 24 ) / 10 );
  Serial.print(" B/s |Practical bandwidth: ");
  Serial.print( (test * 20 ) / 10 );
  Serial.print(" B/s |Packets sent: ");
  Serial.print(test);
  Serial.print(" |Mistakes ");
  Serial.print(mistakes);
  Serial.print(" |Accuracy: ");
  Serial.print(100 - (100 / (test / mistakes)));
  Serial.print(" %");
  Serial.println();
  Serial.println();
  if(mistakes > test / 4 || test == 0)
    Serial.println("Check wiring! Maybe you need a pull down resistor.");

  test = 0; 
  mistakes = 0;
};
