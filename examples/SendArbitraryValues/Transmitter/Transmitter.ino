#include <PJON.h>

// network(Arduino pin used, selected device id)
PJON network(12, 45);

void setup() {
  Serial.begin(115200);
  network.set_error(error_handler);
}

static void error_handler(uint8_t code, uint8_t data) {
  if(code == CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(data);
  }
}


unsigned long time = millis();

void loop() {

  if(millis() - time > 1000) {
    time = millis();
    int temperature = analogRead(A0); 
    
    //Send a "T", break the int in two bytes 
    char content[3] = {'T', temperature >> 8, temperature & 0xFF};

    network.send(44, content, 3);
	}

  network.update();
};