/* This sketch lets a RemoteWorker device connected through Ethernet TCP act as if it
 * is present on the SoftwareBitBang bus connected to this Surrogate device.
 *
 *  The RemoteWorker sketch can the run on a device not capable of SWBB but
 *  with Ethernet support, like a PC or a Raspberry PI.
 *
 * Surrogate and RemoteWorker examples contributed by Fred Larsen. */

#define PJON_PACKET_MAX_LENGTH 50
#define PJON_MAX_PACKETS 3

// Define one of these modes
#define ETCP_SINGLE_DIRECTION
//#define ETCP_SINGLE_SOCKET_WITH_ACK

#include <PJON.h>

const uint8_t DEVICE_ID = 45;
// SWBB Device ID for this device and the RemoteWorker

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> busA(DEVICE_ID);
PJON<EthernetTCP> busB(1);

// Ethernet configuration for this device
uint8_t gateway[] = { 192, 1, 1, 1 };
uint8_t subnet[] = { 255, 255, 255, 0 };
uint8_t mac[] = {0xDF, 0xCF, 0x4F, 0xEF, 0xFE, 0xED};
uint8_t ip[] = { 192, 1, 1, 144 };

// Ethernet configuration for remote device
const byte remote_ip[] = { 192, 1, 1, 145 };
const uint16_t remote_port = 7000;

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to Surrogate 1");
  Ethernet.begin(mac, ip, gateway, gateway, subnet);

  busA.strategy.set_pin(7);
  busA.set_receiver(receiver_functionA);
  busA.begin();

  busB.strategy.link.set_id(1);
  busB.strategy.link.add_node(DEVICE_ID, remote_ip, remote_port);
  busB.strategy.link.keep_connection(true);
  #ifdef ETCP_SINGLE_DIRECTION
    busB.strategy.link.single_initiate_direction(true);
  #else if ETCP_SINGLE_SOCKET_WITH_ACK
    busB.strategy.link.single_socket(true);
  #endif
  busB.set_router(true);
  busB.set_receiver(receiver_functionB);
  busB.begin();
}

void receiver_functionA(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  // Forward packet to RemoteWorker on bus B, preserving the original sender id
  busB.send_from_id(
    packet_info.sender_id,
    packet_info.sender_bus_id,
    DEVICE_ID,
    busB.localhost,
    (char *)payload,
    length,
    packet_info.header
  );
}

void receiver_functionB(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  // All packets sent by the RemoteWorker is delivered to this device, when in the
  // single_initiate_direction listening mode.
  // Forward packet to specified target device on bus A
  busA.send_packet_blocking(
    packet_info.receiver_id,
    packet_info.receiver_bus_id,
    (char *)payload,
    length,
    packet_info.header
  );
}

void loop() {
  busA.receive(1000);
  busB.update();
  busB.receive(1000);
  busA.update();

  // Show the number of sockets created after startup
  // (Try disconnecting the Ethernet cable for a while to see it increase when reconnected.)
  static uint32_t last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.print(F("CONNECT COUNT: "));
    Serial.println(busB.strategy.link.get_connection_count());
  }
};
