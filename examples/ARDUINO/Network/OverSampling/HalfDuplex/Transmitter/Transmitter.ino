
#include <PJONOverSampling.h>
uint16_t packet;
uint8_t bus_id[] = {0, 0, 0, 1};


PJONOverSampling bus(bus_id, 45);

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(bus.packets[data].content[0], DEC);
    Serial.print(" - ");
    Serial.print(micros());
    Serial.println(" is lost.");
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data, DEC);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher PJON_MAX_PACKETS if necessary.");
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
};

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  /* When using more than one pin always use pins connected to
     a different port group to avoid cross-talk. */
  bus.strategy.set_pins(7, 12);
  bus.set_error(error_handler);
  bus.begin();

  packet = bus.send_repeatedly(44, "B", 1, 2000000);
  /* Send B to device 44 every 2 seconds.
     Try to avoid to consinuosly jam the entire zone around you.
     Consider that many appliances and remotes around you are affected by
     PJON packet flooding occurring in this sketch, for this reason try to
     run your tests for short timeframes.
     DO NOT FORGET THIS SKETCH TRANSMITTING ALL DAY LONG :) */
};

void loop() {
  bus.update();
  bus.receive(50000); // Receive for 50 milliseconds
};
