#include <PJON.h>

int busy;
bool debug = true;
int fail;
float mistakes;
float test;

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 44);

void setup() {
  /* Include a custom port, pnly packet including port 8001
     are received others are filtered out. */
  bus.include_port(true, 8001);
  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.begin();

  Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(debug) {
    Serial.print("Header: ");
    Serial.print(packet_info.header, BIN);
    // If packet formatted for a shared medium
    if(packet_info.header & PJON_MODE_BIT) {
      Serial.print(" Receiver bus id: ");
      Serial.print(packet_info.receiver_bus_id[0]);
      Serial.print(packet_info.receiver_bus_id[1]);
      Serial.print(packet_info.receiver_bus_id[2]);
      Serial.print(packet_info.receiver_bus_id[3]);
      Serial.print(" Receiver id: ");
      Serial.print(packet_info.receiver_id);
      // If sender info is included
      if(packet_info.header & PJON_TX_INFO_BIT) {
        Serial.print(" Sender bus id: ");
        Serial.print(packet_info.sender_bus_id[0]);
        Serial.print(packet_info.sender_bus_id[1]);
        Serial.print(packet_info.sender_bus_id[2]);
        Serial.print(packet_info.sender_bus_id[3]);
      }
    }
    // If sender device id is included
    if(packet_info.header & PJON_TX_INFO_BIT) {
      Serial.print(" Sender id: ");
      Serial.print(packet_info.sender_id);
    }
    // Payload Length
    Serial.print(" Length: ");
    Serial.print(length);
    // If port id is included
    if(packet_info.header & PJON_PORT_BIT) {
      Serial.print(" Port bit: ");
      Serial.print(packet_info.port);
    }
  }
  Serial.println();
}


void loop() {
  Serial.print("Starting 1 second communication speed test on port ");
  Serial.println(bus.port);
  long time = millis();
  unsigned int response = 0;
  while(millis() - time < 1000) {
    response = bus.receive();
    if(response == PJON_ACK) test++;
    if(response == PJON_NAK) mistakes++;
    if(response == PJON_BUSY) busy++;
    if(response == PJON_FAIL) fail++;
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

  busy = 0;
  fail = 0;
  mistakes = 0;
  test = 0;
};
