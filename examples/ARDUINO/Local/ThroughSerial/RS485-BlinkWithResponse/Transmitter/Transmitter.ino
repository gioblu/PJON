
/* PJON MAX485 serial modules RS485 communication example */

#include <PJONThroughSerial.h>

// <Strategy name> bus(selected device id)
PJONThroughSerial bus(45);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    Serial.println("BLINK");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void setup() {
  // Initialize LED 13 to be off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  // Initialize Serial instance used for PJON communication
  Serial.begin(9600);

  bus.strategy.set_serial(&Serial);
  bus.strategy.set_enable_RS485_pin(2);
  bus.set_acknowledge(false);
  bus.set_receiver(receiver_function);
  bus.begin();

  // Send B to device 44 every second
  bus.send_repeatedly(44, "B", 1, 1000000);
}

void loop() {
    bus.update();
    bus.receive();
};
