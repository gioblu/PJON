/* Route packets between a local bus and a network bus in both directions,
   using a switch that supports NAT (network address translation).
   
   A device on the local bus can send to a device on a network bus by 
   specifying the bus id. If the switch has configured a public bus id
   (a NAT bus id) for this local bus, the remote devices can also reply.
   __________                 ________                 __________
  | LOCAL    |         Pin 7 | NAT    | Pin 12        | NETWORK  |
  | DEVICE   |_______________| SWITCH |_______________| DEVICE   |
  |__________|  Local bus    |________|  Bus 0.0.0.2  |__________| 
               (NAT 0.0.0.1)
*/

#include <PJON.h>

// PJON object for a local bus
PJON<SoftwareBitBang> bus(44);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED to be off
  bus.strategy.set_pin(7);
  bus.set_receiver(receiver_function);
  bus.begin();
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if((char)payload[0] == 'B') {
    static bool led_on = false;
    digitalWrite(LED_BUILTIN, led_on ? HIGH : LOW);
    led_on = !led_on;
    bus.reply("B", 1);
  }
}

void loop() {
  bus.receive(1000);
  bus.update();
}
