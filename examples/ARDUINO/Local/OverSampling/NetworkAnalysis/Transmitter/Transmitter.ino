#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;

// <Strategy name> bus(selected device id)
PJON<OverSampling> bus(45);

int packet;
uint8_t content[] = "01234567890123456789";

void setup() {
  /* When using more than one pin always use pins connected to
     a different port group to avoid cross-talk. */
  bus.strategy.set_pins(7, 12);
  bus.set_error(error_handler);
  bus.begin();

  Serial.begin(115200);
  Serial.println("PJON - Network analysis");
  Serial.println("Starting a 5 second communication test..");
  Serial.println();
}

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(data);
    Serial.println(" is lost.");
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(bus.packets[data].content[0], DEC);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher PJON_MAX_PACKETS if necessary.");
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
};

void loop() {
  long time = millis();
  while(millis() - time < 5000) {

    /* Here send_packet low level function is used to
    be able to catch every single sending result. */

    unsigned int response = bus.send_packet(44, content, 20);
    if(response == PJON_ACK)
      test++;
    if(response == PJON_NAK)
      mistakes++;
    if(response == PJON_BUSY)
      busy++;
    if(response == PJON_FAIL)
      fail++;
  }

  Serial.print("Bandwidth: ");
  Serial.print((test * (20 + bus.packet_overhead(bus.data[1]) + 1)) / 5.0);
  Serial.println("B/s");
  Serial.print("Data throughput: ");
  Serial.print((test * 20.0) / 5.0);
  Serial.println("B/s");
  Serial.print("Packets sent: ");
  Serial.println(test);
  Serial.print("Mistakes (error found with CRC): ");
  Serial.println(mistakes);
  Serial.print("Fail (no acknowledge from receiver): ");
  Serial.println(fail);
  Serial.print("Busy (Channel is busy or affected by interference): ");
  Serial.println(busy);
  Serial.print("Accuracy: ");
  Serial.print(100 - (100 / (test / mistakes)));
  Serial.println(" %");
  Serial.println(" --------------------- ");
  // Avoid Serial interference during test flushing
  Serial.flush();


  test = 0;
  mistakes = 0;
  busy = 0;
  fail = 0;
};
