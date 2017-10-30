
// Uncomment to run SoftwareBitBang in MODE 2
// #define SWBB_MODE 2
// Uncomment to run SoftwareBitBang in MODE 3
// #define SWBB_MODE 3

/*  Acknowledge Latency maximum duration (1000 microseconds default).
    Can be necessary to higher SWBB_RESPONSE_TIMEOUT to leave enough time to
    receiver to compute the CRC and to respond with a synchronous acknowledgement
    SWBB_RESPONSE_TIMEOUT can be reduced to higher communication speed if
    devices are near and able to compute CRC fast enough. */
//#define SWBB_RESPONSE_TIMEOUT 1000

#include <PJON.h>

float test;
float mistakes;
int busy;
int fail;
uint8_t header;

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 45);

int packet;
char content[] = "01234567890123456789"; // First 10 bytes left empty for bus id

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  // Define a header used for transmission.
  // Force CRC32 and add an additional header byte (only for feature testing)
  header = bus.config | PJON_CRC_BIT | PJON_EXT_HEAD_BIT;
  Serial.begin(115200);
  Serial.println("PJON - Network analysis");
  Serial.println("Starting a 1 second communication test..");
  Serial.println();
}

void loop() {
  long time = millis();
  while(millis() - time < 1000) {

    /* Here send_packet low level function is used to
    be able to catch every single sending result. */

    unsigned int response = bus.send_packet(44, bus_id, content, 20, header);
    if(response == PJON_ACK)
      test++;
    if(response == PJON_NAK)
      mistakes++;
    if(response == PJON_BUSY)
      busy++;
    if(response == PJON_FAIL)
      fail++;
  }

  Serial.print("Packet Overhead: ");
  Serial.print(bus.packet_overhead() + 1);
  Serial.print("B - Total: ");
  Serial.print((unsigned int)((bus.packet_overhead() + 1) * test));
  Serial.println("B");
  Serial.print("Maximum Bandwidth: ");
  // length + packet overhead + PJON_ACK
  Serial.print((unsigned int)(test * (20 + bus.packet_overhead() + 1)));
  Serial.println("B/s");
  Serial.print("Data throughput: ");
  Serial.print((unsigned int)(test * 20));
  Serial.println("B/s");
  Serial.print("Packets sent: ");
  Serial.println((unsigned int)test);
  Serial.print("Mistakes (error found with CRC): ");
  Serial.println((unsigned int)mistakes);
  Serial.print("Fail (no answer from receiver): ");
  Serial.println(fail);
  Serial.print("Busy (Channel is busy or affected by interference): ");
  Serial.println(busy);
  Serial.print("Accuracy: ");
  Serial.print(100 - (100 / (test / mistakes)));
  Serial.println(" %");
  Serial.println("---------------------");
  // Avoid Serial interference during test flushing
  Serial.flush();

  test = 0;
  mistakes = 0;
  busy = 0;
  fail = 0;
};
