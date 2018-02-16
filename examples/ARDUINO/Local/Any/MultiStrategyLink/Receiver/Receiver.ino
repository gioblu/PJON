#include <PJON.h>

/* Defining strategies using StrategyLink template class it is possible to
   leverage of virtual inheritance to handle PJON instances dynamically, for
   example keeping an array of PJON instances using different strategies. */

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJON<Any> bus1(44), bus2(45);

const uint8_t bus_count = 2;
PJON<Any> *buses[] = { &bus1, &bus2 };

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off

  link1.strategy.set_pin(12);
  link2.strategy.set_pin(11);

  bus1.strategy.set_link(&link1);
  bus2.strategy.set_link(&link2);

  bus1.set_receiver(receiver_function);
  bus2.set_receiver(receiver_function);

  bus1.set_custom_pointer(&bus1);
  bus2.set_custom_pointer(&bus2);

  bus1.begin();
  bus2.begin();

  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    Serial.print("BLINK FROM BUS ");
    Serial.println((packet_info.custom_pointer == &bus1 ? "1" : "2"));
    digitalWrite(13, HIGH);
    delay(10);
    digitalWrite(13, LOW);
  }
}

void loop() {
  for(uint8_t i = 0; i < bus_count; i++)
    buses[i]->receive();
};
