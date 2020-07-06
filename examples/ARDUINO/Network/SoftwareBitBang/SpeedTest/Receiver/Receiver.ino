
// Uncomment to use the mode you prefer (default SWBB_MODE 1)
// #define SWBB_MODE 1 // 1.95kB/s - 15625Bd
// #define SWBB_MODE 2 // 2.21kB/s - 17696Bd
// #define SWBB_MODE 3 // 2.94kB/s - 23529Bd
// #define SWBB_MODE 4 // 3.40kB/s - 27210Bd

/*  Response timeout (1500 microseconds default).
    If the acknowledgement fails SWBB_RESPONSE_TIMEOUT may be too short
    specially if long packets are sent or if devices are far from each other */

//#define SWBB_RESPONSE_TIMEOUT 1500


#include <PJONSoftwareBitBang.h>

float test;
float mistakes;
int busy;
int fail;
bool debug = false;

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJONSoftwareBitBang bus(bus_id, 44);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(debug) {
    Serial.print("Header: ");
    Serial.print(packet_info.header, BIN);
    // If packet formatted for a shared medium
    if(packet_info.header & PJON_MODE_BIT) {
      Serial.print(" Receiver bus id: ");
      Serial.print(packet_info.rx.bus_id[0]);
      Serial.print(packet_info.rx.bus_id[1]);
      Serial.print(packet_info.rx.bus_id[2]);
      Serial.print(packet_info.rx.bus_id[3]);
      Serial.print(" Device id: ");
      Serial.print(packet_info.rx.id);
      // If sender info is included
      if((packet_info.header & PJON_TX_INFO_BIT) != 0) {
        Serial.print(" Sender bus id: ");
        Serial.print(packet_info.tx.bus_id[0]);
        Serial.print(packet_info.tx.bus_id[1]);
        Serial.print(packet_info.tx.bus_id[2]);
        Serial.print(packet_info.tx.bus_id[3]);
        Serial.print(" device id: ");
        Serial.print(packet_info.tx.id);\
      // If local format and sender info included
      } else if(packet_info.header & PJON_TX_INFO_BIT) {
        Serial.print(" Sender id: ");
        Serial.print(packet_info.tx.id);
      }
      Serial.print(" Length: ");
      Serial.println(length);
    }
  }
};

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  bus.set_receiver(receiver_function);
  Serial.begin(115200);
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

  Serial.print("Packet Overhead: ");
  Serial.print(bus.packet_overhead(bus.last_packet_info.header) + 1);
  Serial.print("B - Total: ");
  Serial.print((unsigned int)((bus.packet_overhead(bus.last_packet_info.header) + 1) * test));
  Serial.println("B");
  Serial.print("Bandwidth: ");
  Serial.print(test * (20 + bus.packet_overhead(bus.last_packet_info.header) + 1));
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
