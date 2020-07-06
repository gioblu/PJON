/* Giovanni Blu Mitolo 2017 - gioscarab@gmail.com
   With this example it is possible to test HC-12 433MHz wireless serial
   module (HCMODU0054) along with PJON and ThroughSerial strategy. A software
   emulated Serial port is used on pin 2 and 3 to communicate with HC12. The
   receiver device should blink every second.*/

#include <PJONThroughSerial.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);


PJONThroughSerial bus(44);

void setup() {
  // Initialize LED 13 to be off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Set HC12 baudrate (you must use the one configured in HC12, default 9600)
  HC12.begin(9600);

  // Pass the HC12 Serial instance you want to use for PJON communication
  bus.strategy.set_serial(&HC12);

  bus.begin();
  bus.set_receiver(receiver_function);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
    digitalWrite(LED_BUILTIN, LOW);
  }
};

void loop() {
  bus.receive(1000);
};
