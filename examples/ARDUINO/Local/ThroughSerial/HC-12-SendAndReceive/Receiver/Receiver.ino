
/* Giovanni Blu Mitolo 2017 - gioscarab@gmail.com
   With this example it is possible to test HC-12 433MHz wireless serial
   module (HCMODU0054) along with PJON and ThroughSerial strategy. A software
   emulated Serial port is used on pin 2 and 3 to communicate with HC12. Both
   devices should start blinking when programmed. */

/* Set synchronous response timeout to 100 milliseconds.
   If operating at less than 9600Bd TS_RESPONSE_TIME_OUT should be longer */
#define TS_RESPONSE_TIME_OUT 100000

#include <PJON.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);

// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus(44);

void setup() {
  // Initialize LED 13 to be off
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  Serial.begin(115200);

  // Set HC12 baudrate (you must use the one configured in HC12, default 9600)
  HC12.begin(9600);
  // Pass the HC12 Serial instance you want to use for PJON communication
  bus.strategy.set_serial(&HC12);
  bus.set_error(error_handler);
  bus.set_receiver(receiver_function);
  bus.begin();
};


void error_handler(uint8_t code, uint8_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(bus.packets[data].content[0], DEC);
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

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if((char)payload[0] == 'B') {
    if(!bus.update()) // If all packets are delivered, send another
      bus.reply("B", 1);
    digitalWrite(13, HIGH);
    delay(5);
    digitalWrite(13, LOW);
    delay(5);
  }
};

void loop() {
  bus.receive(50000);
  bus.update();
};
