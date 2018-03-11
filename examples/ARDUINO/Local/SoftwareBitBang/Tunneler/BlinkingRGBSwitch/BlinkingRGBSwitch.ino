/* Similar to the Switch but with a blinking RGB led indicating each packet 
   that is forwarded with blue and green for each direction respectively, 
   plus errors in red. */

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
// A clear, not diffuse, RGB LED lets the individual colors be seen directly.
const int ERROR_LED_PIN = 4, SWBB_LED_PIN = 5, LUDP_LED_PIN = 6;
const int LED_DURATION = 200; // how long each packet transfer shall be visible (ms)
uint32_t error_on_time = 0, swbb_on_time = 0, ludp_on_time = 0;

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac); // Use DHCP
  link1.strategy.set_pin(7);
  link2.strategy.set_port(7200); // Use a "private" UDP port
  router.set_sendnotification(sendnotification_function);
  router.set_error(error_function);
  router.set_virtual_bus(0); // Enable virtual bus
  router.set_router(true); // Call receiver for all passing packets
  router.begin();
  
  // Init pins for external LEDs
  pinMode(ERROR_LED_PIN, OUTPUT);
  pinMode(SWBB_LED_PIN, OUTPUT);
  pinMode(LUDP_LED_PIN, OUTPUT);
}

void loop() {
  router.loop();
  check_leds();
};

void sendnotification_function(const uint8_t * const payload, const uint16_t length, const uint8_t receiver_bus,
                               const uint8_t sender_bus, const PJON_Packet_Info &packet_info) {
  light_led(sender_bus);
}

void error_function(uint8_t code, uint8_t data, void *custom_ptr) {
  digitalWrite(ERROR_LED_PIN, HIGH);
  error_on_time = millis();  
}

void light_led(uint8_t bus_number) {
  switch(bus_number) {
    case 0: {
      digitalWrite(SWBB_LED_PIN, HIGH);
      swbb_on_time = millis();
    };
    case 1: {
      digitalWrite(LUDP_LED_PIN, HIGH);
      ludp_on_time = millis();
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
  check_led(SWBB_LED_PIN, swbb_on_time);
  check_led(LUDP_LED_PIN, ludp_on_time);
}