#include <PJON.h>
float test;
float mistakes;
boolean debug = false;

PJON network(A0, 44);

void setup() {
 Serial.begin(115200);
 network.set_acknowledge(true);
 network.set_encryption(true);
}

void loop() {
  network.receive();
  /*
  if(!debug)
    network.receive();
  else {
    long time = millis();
    int response = 0; 
    while(millis() - time < 10000) {
      response = network.receive();
      if(response == 6)
       test++;
      if(response == 21)
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
    test = 0; 
    mistakes = 0;
    
    Serial.print(" - ");
    for(int i = 0; i < 10; i++) 
      Serial.print(network.hash[i]);
    Serial.println();    
  }*/
}
