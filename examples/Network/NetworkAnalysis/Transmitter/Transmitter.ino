#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 45);

int packet;
char content[] = "000000000001234567890123456789"; // First 10 bytes left empty for bus id

void setup() {

  /* Manually copy transmitter and receiver info in the content.
     Done only because low level send_string function is used
     to test absolute performance. */

  for(uint8_t i = 0; i < 4; i++)
   content[i] = bus_id[i];
  content[4] = 44;

  for(uint8_t i = 5; i < 9; i++)
   content[i] = bus_id[i];
  content[9] = 45;

  bus.set_pin(12);
  bus.begin();

  Serial.begin(115200);
  Serial.println("PJON - Network analysis");
  Serial.println("Starting a 1 second communication test..");
  Serial.println();
}

void loop() {
  long time = millis();
  while(millis() - time < 1000) {

    /* Here send_string low level function is used to
    be able to catch every single sending result. */

    int response = bus.send_string(44, content, 30, (uint8_t)(ACK_REQUEST_BIT | SENDER_INFO_BIT | MODE_BIT));
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
  Serial.print(test * 34);
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
