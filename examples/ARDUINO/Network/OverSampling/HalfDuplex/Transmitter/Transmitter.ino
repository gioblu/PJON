
/* Include Async ACK code setting PJON_INCLUDE_ASYNC_ACK as true before including PJON.h */
#define PJON_INCLUDE_ASYNC_ACK true

#include <PJON.h>
uint16_t packet;
uint8_t bus_id[] = {0, 0, 0, 1};

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(bus_id, 45);

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

  /* A packet containing the id of every packet received will be sent back
     by this device to the packet's sender to acknowledge packet reception.
     (With this setup you can avoid sending packet duplicates) */
  bus.set_asynchronous_acknowledge(true);
  bus.set_synchronous_acknowledge(false);
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
