
#include <PJONOverSampling.h>

float test;
float mistakes;
int busy;
int fail;


// <Strategy name> bus(selected device id)
PJONOverSampling bus(44);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
 // Do nothing to avoid affecting speed analysis
};

void setup() {
  Serial.begin(115200);

  // Connect SRX882 data output pin in Arduino pin 7
  bus.strategy.set_pins(7, PJON_NOT_ASSIGNED);

  bus.set_communication_mode(PJON_SIMPLEX);
  bus.set_receiver(receiver_function);
  bus.begin();
};

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

  Serial.print("Bandwidth: ");
  Serial.print(test * (20 + bus.packet_overhead(bus.data[1])));
  Serial.println("B/s");
  Serial.print("Data throughput: ");
  Serial.print(test * 20);
  Serial.println("B/s");
  Serial.print("Packets sent: ");
  Serial.println(test);
  Serial.print("Mistakes (error found with CRC): ");
  Serial.println(mistakes);
  Serial.print("Fail (no data found): ");
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
