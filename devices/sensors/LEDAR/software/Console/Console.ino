
/* LEDAR sensor configuration console
   Giovanni Blu Mitolo 2017
 This is a basic example to show how PJON can be used practically.
 The Serial monitor is used to configure and print data from LEDAR */

#define PJON_PACKET_MAX_LENGTH 20
#define PJON_MAX_PACKETS 2
#include <PJON.h>

PJON<SoftwareBitBang> bus(PJON_MASTER_ID);

uint8_t packet[20];
String string_number;
int req_index = 0;
uint8_t LEDAR_id = PJON_NOT_ASSIGNED;

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);
  Serial.begin(115200);
  Serial.println("LEDAR configuration console - Giovanni Blu Mitolo 2017");
  print_help();
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  Serial.print(" LEDAR id: ");
  Serial.print(packet_info.sender_id);
  Serial.print(" | Reflex: ");
  Serial.println((uint16_t)(payload[0] << 8 | payload[1] & 0xFF));
};

void error_handler(uint8_t code, uint8_t data) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost - LEDAR ID ");
    Serial.println((uint8_t)bus.packets[data].content[0]);
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

void print_help() {
  Serial.println();
  Serial.println("Input commands guide: ");
  Serial.println("?!        -> Help");
  Serial.println("C0-255!   -> Configure PJON header");
  Serial.println("D0-65535! -> Configure detection threshold");
  Serial.println("E!        -> Request a reading");
  Serial.println("I1-254!   -> Configure id");
  Serial.println("L0-65535! -> Configure reading iterations");
  Serial.println("M0-1!     -> Configure mode (0 passive - 1 active)");
  Serial.println("Q!        -> Block incoming configuration (caution)");
  Serial.println("R0-255!   -> Configure recipient id");
  Serial.println("T0-65535! -> Configure transmission interval");
  Serial.println("X!        -> Configuration reset");
  Serial.println();
  Serial.println("Use the Serial console input field to configure the LEDAR.");
  Serial.println("All commands must end with !");
  Serial.println();
}

void loop() {
  bus.receive(1000);
  bus.update();

  if(Serial.available() > 0) {
    char req_byte = Serial.read();
    packet[req_index] = (uint8_t)req_byte;
    if(req_index && req_byte != '!')
      string_number += req_byte;

    if(req_byte == '!') {
      uint16_t result = 0;
      uint8_t packet_length = 0;
      // Configuration update
      if((char)packet[0] == 'C') {
        packet[1] = (uint8_t)string_number.toInt();
        packet_length = 2;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.print("Setting LEDAR configuration: ");
        Serial.print(packet[1]);
        Serial.flush();
      }
      // Detection threshold update
      if((char)packet[0] == 'D') {
        packet[1] = string_number.toInt() >> 8;
        packet[2] = string_number.toInt() & 0xFF;
        packet_length = 3;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.print("Setting LEDAR detection threshold: ");
        Serial.print(string_number.toInt());
        Serial.flush();
      }
      // Reading request
      if((char)packet[0] == 'E') {
        packet_length = 1;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.println("Reading requested.");
        Serial.flush();
      }
      // LEDAR id update
      if((char)packet[0] == 'I') {
        packet_length = 2;
        packet[1] = (uint8_t)string_number.toInt();
        bus.send(LEDAR_id, packet, packet_length);
        LEDAR_id = packet[1];
        Serial.print("Setting LEDAR id: ");
        Serial.println(packet[1]);
        Serial.flush();
      }
      // Reading iterations update
      if((char)packet[0] == 'L' )  {
        packet[1] = string_number.toInt() >> 8;
        packet[2] = string_number.toInt() & 0xFF;
        packet_length = 3;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.print("Setting reading iterations count: ");
        Serial.println(string_number.toInt());
        Serial.flush();
      }
      // Mode configuration update
      if((char)packet[0] == 'M' )  {
        packet[1] = (uint8_t)string_number.toInt();
        packet_length = 2;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.print("Setting mode: ");
        Serial.println(packet[1]);
        Serial.flush();
      }
      // Block further incoming configuration
      if((char)packet[0] == 'Q') {
        packet_length = 1;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.println("Blocking further incoming configuration!");
        Serial.println("Flash the LEDAR with LEDAR.ino to unblock!");
        Serial.print(packet[1]);
        Serial.flush();
      }
      // Recipient id update
      if((char)packet[0] == 'R') {
        packet[1] = (uint8_t)string_number.toInt();
        packet_length = 2;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.println("Recipient id update: ");
        Serial.print(packet[1]);
        bus.set_id(packet[1]);
        Serial.flush();
      }
      // Interval configuration update
      if((char)packet[0] == 'T')  {
        packet[1] = string_number.toInt() >> 8;
        packet[2] = string_number.toInt() & 0xFF;
        packet_length = 3;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.print("Setting transmission interval: ");
        Serial.println(string_number.toInt());
        Serial.flush();
      }
      // Configuration reset to default
      if((char)packet[0] == 'X') {
        packet_length = 1;
        bus.send(LEDAR_id, packet, packet_length);
        Serial.print("Executing configuration reset!");
        Serial.flush();
      }
      // Print Help
      if((char)packet[0] == '?')
        print_help();

      req_index = 0;
      string_number = "";
      return;
    }
    req_index++;
  }
};
