
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

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJONSoftwareBitBang bus(bus_id, 45);

int packet;
uint8_t content[] = "01234567890123456789"; // First 10 bytes left empty for bus id

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
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

    PJON_Packet_Info info;
    info.rx.id = 44;
    memcpy(info.rx.bus_id, bus_id);
    unsigned int response = bus.send_packet(info, content, 20);

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
  Serial.print(bus.packet_overhead(bus.data[1]) + 1);
  Serial.print("B - Total: ");
  Serial.print((unsigned int)((bus.packet_overhead(bus.data[1]) + 1) * test));
  Serial.println("B");
  Serial.print("Bandwidth: ");
  // length + packet overhead + PJON_ACK
  Serial.print((unsigned int)(test * (20 + bus.packet_overhead(bus.data[1]) + 1)));
  Serial.println("B/s");
  Serial.print("Data throughput: ");
  Serial.print((unsigned int)(test * 20));
  Serial.println("B/s");
  Serial.print("Packets sent: ");
  Serial.println((unsigned int)test);
  Serial.print("Mistakes (error found with CRC): ");
  Serial.println((unsigned int)mistakes);
  Serial.print("Fail (no acknowledge from receiver): ");
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
