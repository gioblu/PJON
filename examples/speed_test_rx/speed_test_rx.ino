#include <PJON.h>

float test;
float mistakes;
float busy;
float fail;

PJON network(A0, 44);

void setup() {
  Serial.begin(115200);
  network.set_receiver(receiver_function);
};

static void receiver_function(uint8_t length, uint8_t *payload) {
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
    if(response == BUSY)
      busy++;
    if(response == FAIL)
      fail++;
  }
    
  Serial.print("Absolute com speed: ");
  Serial.print( (test * 23 ) / 10 );
  Serial.print(" b/s |Practical bandwidth: ");
  Serial.print( (test * 20 ) / 10 );
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
    Serial.println("Check wiring! Maybe you need a pull down resistor. If you are using arduino Mega the behaviour is not predictable and PJON is not still tested with it.");

  test = 0; 
  mistakes = 0;
};
