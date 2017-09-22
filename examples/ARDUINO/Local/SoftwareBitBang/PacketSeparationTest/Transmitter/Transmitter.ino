
/* Giovanni Blu Mitolo 2017 - gioscarab@gmail.com
   Execute SoftwareBitBang packet separation consistency testing

   This device transmits on pin 12 a continuous random bytestram.
   The receiver device will attempt to receive packets from pin 12.
   If an incoming packet is detected a false positive occurred.  */

// Uncomment to run SoftwareBitBang to mode FAST
// #define SWBB_MODE 2
// Uncomment to run SoftwareBitBang to mode OVERDRIVE
// #define SWBB_MODE 3

#include <PJON.h>

SoftwareBitBang swbb;

void setup() {
  swbb.begin();
  swbb.set_pin(12);
  pinMode(12, OUTPUT);
};

void loop() {
  swbb.send_byte(random(0, 256));
};
