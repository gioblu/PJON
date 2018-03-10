/* Tunnel packets through another medium using one switch on each end. 
/  For example, two SoftwareBitBang buses that are far from each other
/  can be joined transparently through a LAN using the LocalUDP strategy, or
/  through a WAN or Internet using the EthernetTCP strategy. */

#define PJON_MAX_PACKETS 3
#include <PJONInteractiveRouter.h>

// Ethernet configuration for this device, MAC must be unique!
byte mac[] = {0xDE, 0x34, 0x4E, 0xEF, 0xFE, 0xE1};

StrategyLink<SoftwareBitBang> link1;
StrategyLink<LocalUDP> link2;

PJONAny bus1(&link1);
PJONAny bus2(&link2);

PJONInteractiveRouter<PJONVirtualBusRouter<PJONSwitch>> router(2, (PJONAny*[2]){&bus1, &bus2});

// Pins for blinking RGB LED for showing traffic in each direction
// (Use resistors approximately R:3.3k G:33k, B:8.2k)
const int ERROR_LED_PIN = 4, SWBB_LED_PIN = 5, LUDP_LED_PIN = 6;
const int LED_DURATION = 200; // how long each packet transfer shall be visible (ms)
uint32_t swbb_on_time = 0, ludp_on_time = 0;

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac); // Use DHCP
  link1.strategy.set_pin(7);
  link2.strategy.set_port(7200); // Use a "private" UDP port
  router.set_receiver(receiver_function);
  router.set_error(error_function);
  router.set_virtual_bus(0); // Enable virtual bus
  router.set_router(true); // Call receiver for all passing packets
  router.begin();
  
  // Init pins for external LEDs
  pinMode(ERROR_LED_PIN, OUTPUT); digitalWrite(ERROR_LED_PIN, LOW);
  pinMode(SWBB_LED_PIN, OUTPUT);
  pinMode(LUDP_LED_PIN, OUTPUT);
}

void loop() {
  router.loop();
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {

// HMM, this is called for ALL OBSERVED packets on either bus, not just the ones being routed.
  light_led(router.current_bus);
}

void error_function(uint8_t code, uint8_t data, void *custom_ptr) {
  digitalWrite(ERROR_LED_PIN, HIGH);
  error_time_on = millis();  
}

void light_led(uint8_t bus_number) {
  switch(bus_number) {
    0: {
      digitalWrite(SWBB_LED_PIN, HIGH);
      swbb_time_on = millis();
    };
    1: {
      digitalWrite(LUDP_LED_PIN, HIGH);
      ludp_time_on = millis();
    };
  }
}

void check_led(uint8_t pin, uint32_t &on_time) {
  if (on_time != 0 && (uint32_t)(millis() - on_time) > LED_DURATION) {
    digitalWrite(pin, LOW);
    on_time = 0;
  }  
}

void check_leds() {
  check_led(ERROR_LED_PIN, error_on_time);
  check_led(ETHERNET_LED_PIN, ethernet_on_time);
  check_led(PJON_LED_PIN, pjon_on_time);
}
