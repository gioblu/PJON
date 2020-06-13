
/* Use a couple of visible light / IR / UV LEDs as wireless bidirectional transceivers
   To know how to wire up the circuit see the AnalogSampling README:
   https://github.com/gioblu/PJON/tree/master/src/strategies/AnalogSampling */

#include <PJONAnalogSampling.h>

// <Strategy name> bus(selected device id)
PJONAnalogSampling bus(44);

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
};

void loop() {
  bus.receive();
};
