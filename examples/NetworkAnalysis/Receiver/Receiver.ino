#include <PJON.h>
PJON network(12, 44);

void setup() {
  Serial.begin(115200);
};

void loop() {
  
  /* 
  receive() function  without parameter 
  should not be used because is a low level 
  function and is not aware of packets. Here is 
  is used only to detect the maximum bandwidth.
  */

  network.receive(); 
};