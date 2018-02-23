/* This sketch demonstrates how to spy on the packets being routed by a switch or router object. 
   In this case it just makes the on-board LED blink for every packet that passes. */

#include <PJONInteractiveRouter.h>

StrategyLink<SoftwareBitBang> link1;
StrategyLink<SoftwareBitBang> link2;

PJONAny bus1(&link1, (const uint8_t[4]){0,0,0,1});
PJONAny bus2(&link2, (const uint8_t[4]){0,0,0,2});

PJONInteractiveRouter<PJONSwitch> router(2, (PJONAny*[2]) {&bus1, &bus2});

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  link1.strategy.set_pin(11);
  link2.strategy.set_pin(12);
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