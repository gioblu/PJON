#include <PJON.h>

// bus(Arduino pin used, selected device id)
PJON bus(12, 45);

void setup() {
  bus.begin(); // Initialize PJON bus
  bus.set_error(error_handler);

  Serial.begin(115200);
}

void error_handler(uint8_t code, uint8_t data) {
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

    bus.send(44, content, 3);
  }

  bus.update();
};
