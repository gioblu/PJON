
/* A 10K pull-down resistor could be required to have good communication quality.
   If test results are showing the channel often busy, pull-down resistor is necessary. */

#if defined(ESP8266)
  #include <ESP.h>
#endif

#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;

// network(Arduino pin used, selected device id)
PJON network(D1, 45);

int packet;
char content[] = "01234567890123456789";

void setup() {
  Serial.begin(115200);

  Serial.println("PJON - Network analysis");
  Serial.println("Starting a 10 seconds communication test..");
  Serial.println();
}

void loop() {
  long time = millis();
  while(millis() - time < 10000) {
    #if defined(ESP8266)
      ESP.wdtFeed();
    #endif

    /* Here send_string low level function is used to
    be able to catch every single sending result. */

    int response = network.send_string(44, content, 20);
    if(response == ACK)
      test++;
    if(response == NAK)
      mistakes++;
    if(response == BUSY)
      busy++;
    if(response == FAIL)
      fail++;
  }

  Serial.print("Absolute com speed: ");
  Serial.print( (test * 24 ) / 10 );
  Serial.println("B/s");
  Serial.print("Practical bandwidth: ");
  Serial.print( (test * 20 ) / 10 );
  Serial.println("B/s");
  Serial.print("Packets sent: ");
  Serial.println(test);
  Serial.print("Mistakes (error found with CRC) ");
  Serial.println(mistakes);
  Serial.print("Fail (no answer from receiver) ");
  Serial.println(fail);
  Serial.print("Busy (Channel is busy or affected by interference) ");
  Serial.println(busy);
  Serial.print("Accuracy: ");
  Serial.print(100 - (100 / (test / mistakes)));
  Serial.println(" %");
  Serial.println(" --------------------- ");

  test = 0;
  mistakes = 0;
  busy = 0;
  fail = 0;
};
