/* Route packets between a local bus and a network bus in both directions,
   using a switch that supports NAT (network address translation).

   The local bus is assigned a public address 0.0.0.1 in the switch, and that
   bus id can be used to reach devices within the local bus.
   __________                 ________                 __________
  | LOCAL    |         Pin 7 | NAT    | Pin 12        | NETWORK  |
  | DEVICE   |_______________| SWITCH |_______________| DEVICE   |
  |__________|  Local bus    |________|  Bus 0.0.0.2  |__________|
               (NAT 0.0.0.1)
*/

#include <PJONSoftwareBitBang.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 2};
uint8_t remote_bus_id[] = {0, 0, 0, 1};

// PJON object for a network bus
PJONSoftwareBitBang bus(bus_id, 45);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED to be off

  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send(44, remote_bus_id, "B", 1);
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
