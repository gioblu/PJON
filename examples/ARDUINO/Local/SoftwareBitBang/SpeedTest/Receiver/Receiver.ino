
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

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(44);

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();

  bus.set_receiver(receiver_function);

  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
 // Do nothing to avoid affecting speed analysis
}

void loop() {
  Serial.println("Starting 1 second communication speed test...");
  long time = millis();
  unsigned int response = 0;
  while(millis() - time < 1000) {
    response = bus.receive();
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
  Serial.print(bus.packet_overhead(bus.last_packet_info.header) + 1);
  Serial.print("B - Total: ");
  Serial.print((unsigned int)((bus.packet_overhead(bus.last_packet_info.header) + 1) * test));
  Serial.println("B");
  Serial.print("Absolute com speed: ");
  Serial.print(test * (20 + bus.packet_overhead(bus.last_packet_info.header) + 1));
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
  // Avoid Serial interference during test flushing
  Serial.flush();

  test = 0;
  mistakes = 0;
  busy = 0;
  fail = 0;
};
