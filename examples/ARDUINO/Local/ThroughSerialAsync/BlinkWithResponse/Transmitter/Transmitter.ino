#define PJON_INCLUDE_TSA true
#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ThroughSerialAsync> bus(45);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  // Initialize LED 13 to be off
  Serial.begin(9600);
  bus.set_receiver(receiver_function);
  bus.strategy.set_serial(&Serial); // Pass the Serial object you want to use for communication
  bus.begin();
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device id 44 every second
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
};

void loop() {
  bus.update();
  bus.receive();
};
