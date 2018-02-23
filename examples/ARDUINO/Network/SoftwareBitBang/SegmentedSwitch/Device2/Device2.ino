
/* This example illustrates how to route packets between devices sharing the
   same bus id although potentially using different strategies or media to
   be connected.

   PJONSwitch routing between two separate SoftwareBitBang buses
     __________           ________           __________
    |          | 0.0.0.1 |        | 0.0.0.1 |          |
    | DEVICE 1 |_________| SWITCH |_________| DEVICE 2 |
    |__________| Pin 11  |________| Pin 12  |__________| */

#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};
uint8_t remote_bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 200);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED to be off

  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send(100, remote_bus_id, "B", 1);
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if((char)payload[0] == 'B') {
    bus.reply("B", 1);
    static bool led_on = false;
    digitalWrite(LED_BUILTIN, led_on ? HIGH : LOW);
    led_on = !led_on;
  }
}

void loop() {
  bus.receive(1000);
  bus.update();
}
