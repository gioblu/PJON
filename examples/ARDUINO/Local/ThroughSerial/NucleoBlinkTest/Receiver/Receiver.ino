
/* Include only ThroughSerial
   (not all data-link are still compatible with STM32 microcontrollers) */
#define PJON_INCLUDE_TS true

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(44);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  Serial1.begin(9600); // Serial1 or pin 2 and 8 on F401RE and L053R8
  bus.strategy.set_serial(&Serial1);
  bus.begin();

  bus.set_receiver(receiver_function);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    // Light up the built in LED if a B is received
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
    digitalWrite(LED_BUILTIN, LOW);
  }
};

void loop() {
  bus.receive();
};
