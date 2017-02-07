#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<OverSampling> bus(bus_id, 44);

int packet;
char content[] = "01234567890123456789"; // First 4 bytes left empty for bus id

void setup() {

  bus.strategy.set_pins(11, 12);

  bus.begin();

  Serial.begin(115200);
  Serial.println("PJON - Network analysis");
  Serial.println("Starting a 5 second communication test..");
  Serial.println();
}

void loop() {
  long time = millis();
  while(millis() - time < 5000) {

    /* Here send_packet low level function is used to
    be able to catch every single sending result. */

    unsigned int response = bus.send_packet(44, bus_id, content, 20);
    if(response == PJON_ACK)
      test++;
    if(response == PJON_NAK)
      mistakes++;
    if(response == PJON_BUSY)
      busy++;
    if(response == PJON_FAIL)
      fail++;
  }

  Serial.print("Absolute com speed: ");
  Serial.print((test * 28.0) / 5.0);
  Serial.println("B/s");
  Serial.print("Practical bandwidth: ");
  Serial.print((test * 20.0) / 5.0);
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
