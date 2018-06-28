#include <PJON.h>

/* Use a couple of visible light / IR / UV LEDs as wireless bidirectional transceivers
   Connect LED + to A0
   Connect LED - to GND
   Place a 1-5 MΩ pull down resistor between A0 and GND
   Try different resistor values to find the optimal to maximize range
   Higher resistance can higher the range but can also higher background noise.  */

// <Strategy name> bus(selected device id)
PJON<AnalogSampling> bus(44);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  /* Use internal 1.1v analog reference (not available on Arduino Mega)
  DEFAULT: Default analog reference 5v (5v Arduino boards) or 3.3v (3.3V Arduino boards)
  INTERNAL: Built-in 1.1v on ATmega168 or ATmega328 and 2.56v on ATmega8 (not available on Arduino Mega)
  INTERNAL1V1: Built-in 1.1v reference (Arduino Mega only)
  INTERNAL2V56: a built-in 2.56v reference (Arduino Mega only)
  EXTERNAL: Voltage applied to the AREF pin (0 to 5v) is used as the reference. */
  analogReference(INTERNAL);

  bus.strategy.set_pin(A0);
  bus.begin();
  bus.set_receiver(receiver_function);
  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'B') {
    Serial.println("Received!");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(5);
    digitalWrite(LED_BUILTIN, LOW);
  }
}

void loop() {
  bus.receive(1000);

  Serial.print("Threshold: ");
  Serial.print(bus.strategy.threshold);
  Serial.print(" ");
  Serial.print((1.1 / (1023.0 / bus.strategy.threshold)) * 1000); // Convert threshold in mV
  Serial.println(" mV");
};
