
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>

// Bus id definition
uint8_t bus_id_A[] = {0, 0, 0, 1};
uint8_t bus_id_B[] = {0, 0, 0, 2};
// Tunneler is trasparently forwarding data, does not need a device id
PJON<SoftwareBitBang> busA(bus_id_A, PJON_NOT_ASSIGNED);
PJON<SoftwareBitBang> busB(bus_id_B, PJON_NOT_ASSIGNED);
// Maximum expected latency or response time, higher if necessary
uint32_t latency = 6000;

void receiver_functionA(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(!busA.bus_id_equality(packet_info.receiver_bus_id, bus_id_B)) return;
  // Forward packet to bus 0.0.0.2
  if(packet_info.header & PJON_ACK_REQ_BIT)
    busA.strategy.send_response(PJON_ACK);
  busB.send_from_id(
    packet_info.sender_id,
    packet_info.sender_bus_id,
    packet_info.receiver_id,
    packet_info.receiver_bus_id,
    (char *)payload,
    length,
    packet_info.header,
    packet_info.id
  );
}

void receiver_functionB(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(!busB.bus_id_equality(packet_info.receiver_bus_id, bus_id_A)) return;
  // Forward packet to bus 0.0.0.1
  if(packet_info.header & PJON_ACK_REQ_BIT)
    busB.strategy.send_response(PJON_ACK);
  busA.send_from_id(
    packet_info.sender_id,
    packet_info.sender_bus_id,
    packet_info.receiver_id,
    packet_info.receiver_bus_id,
    (char *)payload,
    length,
    packet_info.header,
    packet_info.id
  );
}

void setup() {
  // Initialize LED 13 to be off
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  // Bus 0.0.0.1 configuration
  busA.set_synchronous_acknowledge(true);
  busA.set_asynchronous_acknowledge(true);
  busA.set_receiver(receiver_functionA);
  busA.set_router(true);
  busA.strategy.set_pin(11);
  busA.begin();
  // Bus 0.0.0.2 configuration
  busB.set_synchronous_acknowledge(true);
  busB.set_asynchronous_acknowledge(true);
  busB.set_receiver(receiver_functionB);
  busB.set_router(true);
  busB.strategy.set_pin(12);
  busB.begin();
}

void loop() {
  busB.update();
  busA.update();
  long time = micros();
  while((micros() - time) < latency) {
    busA.receive();
    busB.receive();
  }
}
