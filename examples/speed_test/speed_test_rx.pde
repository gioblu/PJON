#include <PJON.h>

float test;
float mistakes;

PJON network(A0, 44);

void setup() {
  Serial.begin(115200);
  network.set_acknowledge(true);
  network.set_collision_avoidance(true);
  network.set_encryption(false);
};

void loop() {
  Serial.println("Starting 10 seconds communication speed test...");
  long time = millis();
  int response = 0; 
  while(millis() - time < 10000) {
    response = network.receive();
    if(response == ACK)
      test++;
    if(response == NAK)
      mistakes++;
  }
    
  Serial.print("Absolute com speed: ");
  Serial.print( (test * 13 ) / 10 );
  Serial.print(" b/s |Practical bandwidth: ");
  Serial.print( (test * 10 ) / 10 );
  Serial.print(" b/s |Packets sent: ");
  Serial.print(test);
  Serial.print(" |Mistakes ");
  Serial.print(mistakes);
  Serial.print(" |Accuracy: ");
  Serial.print(100 - (100 / (test / mistakes)));
  Serial.print(" %");
  Serial.println();
  Serial.println();
  if(mistakes > test / 4 || test == 0)
    Serial.println("Check you wiring, maybe you need a pull down resistor or you are using a mistaken timing configuration according to your IDE, Check PJON.h. If you are using arduino Mega the behaviour is not predictable and PJON is not still tested with it.");

  test = 0; 
  mistakes = 0;
};
