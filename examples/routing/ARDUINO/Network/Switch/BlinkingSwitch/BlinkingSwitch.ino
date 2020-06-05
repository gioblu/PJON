
/* This sketch is just the Switch example with a user defined receive callback.
   It demonstrates how to spy on the packets being routed by a switch or router object.
   In this case it just makes the on-board LED blink for every packet that passes. 
   
   Note that the default basis for the PJONInteractiveRouter is PJONSwitch,
   but that you can specify to use PJONRouter, PJONDynamicRouter or
   PJONVirtualBusRouter instead by adding the class as an additional
   template parameter, to get additional functionality.
   */

#include <PJONInteractiveRouter.h>

PJONInteractiveRouter2<SoftwareBitBang, SoftwareBitBang> router;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_pin(12);
  router.get_bus(0).set_bus_id((const uint8_t[4]){0, 0, 0, 1});
  router.get_bus(1).set_bus_id((const uint8_t[4]){0, 0, 0, 2});
  router.set_receiver(receiver_function);
  router.set_router(true); // Pick up every packet passing
  router.begin();
}

void loop() {
  router.loop();
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  toggle_led();
}

void toggle_led() {
  static bool led_is_on = false;
  led_is_on = !led_is_on;
  digitalWrite(LED_BUILTIN, led_is_on ? HIGH : LOW);
}
