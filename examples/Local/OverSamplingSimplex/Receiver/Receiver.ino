#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;


// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(44);

void setup() {
  bus.set_pin(11);
  bus.begin();

  bus.set_receiver(receiver_function);

  Serial.begin(115200);
};

void receiver_function(uint8_t id, uint8_t *payload, uint8_t length) {
 // Do nothing to avoid affecting speed analysis
}

void loop() {
  Serial.println("Starting 1 second communication speed test...");
  long time = millis();
  int response = 0;
  while(millis() - time < 1000) {
    response = bus.receive();
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
  Serial.print(test * 24);
  Serial.println("B/s");
  Serial.print("Practical bandwidth: ");
  Serial.print(test * 20);
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
