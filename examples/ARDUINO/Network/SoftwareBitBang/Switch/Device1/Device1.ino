
/* Route packets between a collection of buses with the same
   or different strategies or media.
   __________                 ________                 __________
  |          |        Pin 11 |        | Pin 12        |          |
  | DEVICE 1 |_______________| SWITCH |_______________| DEVICE 2 |
  |__________|  Bus 0.0.0.1  |________|  Bus 0.0.0.2  |__________| */

#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};
uint8_t remote_bus_id[] = {0, 0, 0, 2};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 44);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED to be off
  bus.strategy.set_pin(11);
  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send_repeatedly(45, remote_bus_id, "B", 1, 250000);
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if((char)payload[0] == 'B') {
    static bool led_on = false;
    digitalWrite(LED_BUILTIN, led_on ? HIGH : LOW);
    led_on = !led_on;
  }
}

void loop() {
  bus.receive(1000);
  bus.update();
}
