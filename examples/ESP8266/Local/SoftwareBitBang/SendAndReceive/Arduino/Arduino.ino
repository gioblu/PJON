#include <PJONSoftwareBitBang.h>

/* In this example it is demonstrated how to connect an ESP8266 module and an
   Arduino compatible device exchanging the character "B" in both directions
   at the maximum speed.

   Arduino              ESP8266
    _________   wire   _________
   | ||   |_||        | ||_|_|| |
   ||     12||___     | |     | |
   ||       ||   |    | |_____| |
   ||       ||   |____|D1  _    |
   |_________|        |___|_|___|

   The Arduino built in LED blinks each time the
   character "B" is received. The string "BLINK" is printed in the ESP866
   Serial monitor each time the character "B" is received. */

PJONSoftwareBitBang bus(45); // Use device id 45

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  bus.set_error(error_handler);
  bus.set_receiver(receiver_function);
  bus.strategy.set_pin(12);
  bus.begin();
  bus.send(44, "B", 1);
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
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if((char)payload[0] == 'B') {
    Serial.println("BLINK");
    if(!bus.update()) // If all packets are delivered, send another
      bus.reply("B", 1);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5);
    digitalWrite(LED_BUILTIN, LOW);
    delay(5);
  }
}

void loop() {
  bus.receive(50000);
  bus.update();
};
