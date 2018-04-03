
/* LocalChat is a chat application example using the Arduino IDE serial
   monitor implemented in 170 lines of code. PJON in local mode is used to
   support up to 254 participants, the Arduino IDE serial monitor is used as
   the user interface. Using HC-12 (HCMODU0054) wireless serial module a
   decentralized wireless network of computers can be made with USB enabled
   Arduino compatible devices used as USB to HC-12 radio 433MHz gateways.

   User's 1 laptop                                        User's 2 laptop
  __________________                                     __________________
 ||1:Ciao 2!       ||                                   ||1:Ciao 2!       ||
 ||2:Ciao 1 :)     ||                                   ||2:Ciao 1 :)     ||
 ||1:Awesome chat! ||     LOS or line of sight range    ||1:Awesome chat! ||
 ||________________||             150-1800m             ||________________||
 |__________________| USB  _______         _______  USB |__________________|
 |[][][][][][][][][]|=====|ARDUINO|       |ARDUINO|=====|[][][][][][][][][]|
 |[][][][][][][][][]|     | HC-12 |       | HC-12 |     |[][][][][][][][][]|
 |[][][]|____|[][][]|     |_______|       |_______|     |[][][]|____|[][][]|
 |__________________|                                   |__________________|

 Open the Arduino IDE serial monitor at 115200 baud to initiate the chat

 Giovanni Blu Mitolo 2018 */

/* Set synchronous response timeout to 100 milliseconds.
   If operating at less than 9600Bd TS_RESPONSE_TIME_OUT should be longer */
#define TS_RESPONSE_TIME_OUT 200000

/* Include the packet id feature to support packet duplication avoidance i.e.
   avoid cases where a missed acknowledgement because of range or interference
   can lead to message duplications:
   user 1: Ciao!
   user 1: Ciao!
   user 1: Ciao! */
#define PJON_INCLUDE_PACKET_ID true

/* Use 63 characters maximum packet length.
   HC-12 has two 64-byte Rx and Tx FIFO memories built into the chip,
   supporting up to 64 bytes maximum packet length */
#define PJON_PACKET_MAX_LENGTH 63

#include <PJON.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);

// <Strategy name> bus(selected device id)
PJON<ThroughSerial> bus;

uint8_t packet[100];
String string_number;
int req_index = 0;
bool parse_id = true;
bool initialized = false;
uint8_t recipient = 0;

void setup() {
  // Initialize LED 13 to be off
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  // Set HC12 baudrate (you must use the one configured in HC12, default 9600)
  HC12.begin(9600);

  // Pass the HC12 Serial instance you want to use for PJON communication
  bus.strategy.set_serial(&HC12);
  // Avoid messages duplication
  bus.set_packet_id(true);

  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);
  bus.begin();

  Serial.begin(115200);
  Serial.println("LocalChat - Giovanni Blu Mitolo 2018");
  Serial.println("User id configuration guide:");
  Serial.println("1) Set serial monitor 'Newline' ending");
  Serial.println("2) Enter your user id (1-254) in the text field");
  Serial.println("3) Press 'Send'");
  Serial.println();
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
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
  // Received messages sender id and content are printed here
  if(packet_info.sender_id == recipient || packet_info.sender_id == PJON_BROADCAST) {
    Serial.print("user ");
    Serial.print(packet_info.sender_id);
    Serial.print((packet_info.receiver_id == PJON_BROADCAST) ? " " : ": ");
    for(uint16_t i = 0; i < length; i++)
      Serial.print((char)payload[i]);
    Serial.println();
  }
};

void error_handler(uint8_t code, uint8_t data) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Message delivery fail to user id: ");
    Serial.println((uint8_t)bus.packets[data].content[0]);
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Message too long, length: ");
    Serial.print(data);
    Serial.print(", maximum length: ");
    Serial.println(PJON_PACKET_MAX_LENGTH);
  }
};

void loop() {
  // Handle PJON connectivity if no data has been sent from serial monitor
  if(Serial.available() <= 0) {
    bus.receive(200000);
    bus.update();
  } else { // Handle data from serial monitor
    char req_byte = Serial.read();
    string_number += req_byte;
    if(initialized && recipient) {
      if(!req_index) {
        Serial.print("user ");
        Serial.print((uint8_t)bus.device_id(), DEC);
        Serial.print(": ");
      }
      Serial.print(req_byte);
      packet[req_index] = req_byte;
    }
    if(req_byte == '\n') {
      if(initialized) {
        if(!recipient) {
          recipient = (uint8_t)string_number.toInt();
          Serial.print("Recipient's user id ");
          Serial.print(recipient, DEC);
          Serial.println(" set.");
          Serial.println();
          Serial.println("LocalChat is now online!");
          uint8_t hello[10] = "is online.";
          bus.send(0, hello, 10);
        } else bus.send(recipient, packet, req_index);
      } else {
        bus.set_id((uint8_t)string_number.toInt());
        Serial.print("User id ");
        Serial.print(bus.device_id(), DEC);
        Serial.println(" set.");
        Serial.println("Enter the recipient's user id (1-254) and press 'Send'");
        initialized = true;
      }
      parse_id = true;
      req_index = 0;
      string_number = "";
      return;
    }
    req_index++;
  }
};
