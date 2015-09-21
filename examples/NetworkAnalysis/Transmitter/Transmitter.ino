#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;

PJON network(12, 45);

int packet;
char content[] = "01234567890123456789";

void setup() {
  Serial.begin(115200);
  network.set_error(error_handler);

  Serial.println("PJON - Network analysis");
  Serial.println("Starting a 10 seconds communication test..");
  Serial.println();
}

static void error_handler(uint8_t code, uint8_t data) {
  if(code == CONNECTION_LOST) {
    Serial.print("Connection lost with device id ");
    Serial.println(data);
  }
}

void loop() {
  long time = millis();
  while(millis() - time < 10000) {
 
    /*
    Here send_string low level function is used to
    be able to catch every single sending result. 
    */
 
    int response = network.send_string(44, content);
    if(response == ACK)
      test++;
    if(response == NAK)
      mistakes++;
    if(response == BUSY)
      busy++;
    if(response == FAIL)
      fail++;
  	delayMicroseconds(14);
  }
  
  Serial.print("Absolute com speed: ");
  Serial.print( (test * 23 ) / 10 );
  Serial.println("kB/s");
  Serial.print("Practical bandwidth: ");
  Serial.print( (test * 20 ) / 10 );
  Serial.println("kB/s");
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