
/* Giovanni Blu Mitolo 2017 - gioscarab@gmail.com
   SoftwareBitBang packet separation consistency testing

   This device attempts to receive packets from pin 12. The other device
   transmits a continuous random byte stream. If an incoming packet is detected
   a false positive occurred. */

// Transmission speed modes (see Timing.h)
// #define SWBB_MODE 1 // 1.95kB/s - 15625Bd
// #define SWBB_MODE 2 // 2.21kB/s - 17696Bd
// #define SWBB_MODE 3 // 2.94kB/s - 23529Bd
// #define SWBB_MODE 4 // 3.40kB/s - 27210Bd

#include <PJON.h>

SoftwareBitBang swbb;
uint32_t attempts =  100000; // Number of random string reception attempts
uint8_t data[1];

uint32_t test;
uint32_t fail;
uint32_t time;
uint8_t percent;

void setup() {
  swbb.set_pin(12);
  swbb.begin();
  Serial.begin(115200);
};

void print_result() {
  Serial.println();
  Serial.print("Attempts: ");
  Serial.print(test);
  Serial.print(" - False positives: ");
  Serial.print(fail);
  Serial.print(" - Accuracy: ");
  Serial.print(100.0 - (100.0 / (float(test) / float(fail))), 5);
  Serial.println("%");
  Serial.print("Elapsed time: ");
  Serial.print(time / 1000.0);
  Serial.print("s - Data received: ");
  Serial.print(((1000000 / ((SWBB_BIT_WIDTH * 9) + SWBB_BIT_SPACER)) * (time / 1000.0)) / 1000.0);
  Serial.print("kB");
  Serial.println();
};

void loop() {
  fail = 0, test = 0, percent = 0;
  Serial.println();
  Serial.print("Executing SoftwareBitBang");
  Serial.println(" packet separation consistency test.");
  Serial.print(attempts);
  Serial.println(" reception attempts of a continuous random byte stream.");
  Serial.println(" __________________________________________________ ");
  Serial.println("[0%                     50%                    100%]");
  Serial.print(" ");
  test = 0, fail = 0, percent = 0;
  time = millis();
  while(test < attempts) {
    if(swbb.receive_frame(data, PJON_PACKET_MAX_LENGTH) != PJON_FAIL)
      fail++;
    test++;
    if(!(test % (attempts / 50))) {
      Serial.print("-");
      percent++;
    }
  }
  time = millis() - time;
  print_result();
};
