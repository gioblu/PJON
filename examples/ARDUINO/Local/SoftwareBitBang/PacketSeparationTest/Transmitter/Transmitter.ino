
/* Giovanni Blu Mitolo 2017 - gioscarab@gmail.com
   SoftwareBitBang packet separation consistency testing

   This device transmits on pin 12 a continuous random byte stream.
   The receiver device attempts to receive packets from pin 12.
   If an incoming packet is detected a false positive occurred.  */

// Transmission speed modes (see Timing.h)
// #define SWBB_MODE 1 // 1.95kB/s - 15625Bd
// #define SWBB_MODE 2 // 2.21kB/s - 17696Bd
// #define SWBB_MODE 3 // 2.94kB/s - 23529Bd
// #define SWBB_MODE 4 // 3.40kB/s - 27210Bd

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
