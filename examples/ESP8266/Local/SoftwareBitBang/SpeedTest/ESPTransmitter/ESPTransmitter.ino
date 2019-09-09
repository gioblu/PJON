
/* ESP8266 - Node MCU side
   Devices can be wired directly being ESP8266 5v tolerant */

 // Include only SoftwareBitBang strategy
 #define PJON_INCLUDE_SWBB

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

uint8_t content[] = "01234567890123456789";

void setup() {
  bus.strategy.set_pin(D1); // ESP8266 GPIO 5 or pin D1
  bus.set_error(error_handler);
  bus.begin();

  Serial.begin(115200);
}

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
};

void loop() {
  if(!bus.update())
    bus.send(44, content, 20);
};
