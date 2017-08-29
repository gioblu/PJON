/* This sketch is a modified copy of the example:
   ARDUINO/Local/EthernetTCP/PingPong/Transmitter.

   In this context it is a "RemoteWorker" for a corresponding "Surrogate" on a
   SoftwareBitBang bus, connected to the Surrogate via Ethernet TCP.
   The surrogate will connect to this device, allowing this device to talk to
   all devices on the SWBB bus by their ids, and receiving all packets sent to
   the id if this device on the SWBB bus.

   It sends BLINK to device id 44 which is on a SWBB bus connected to the
   Surrogate, and receives BLINK replies back.
   Device 44 is the example:
   ARDUINO/Local/SoftwareBitBang/BlinkWithResponse/Receiver.

   This is just a demonstration of the concept. It is more meaningful when the
   RemoteWorker is not an Arduino but a process on a PC or a RPI, effectively
   allowing that process to transparently be part of a SWBB bus. The Surrogate
   will "phone home" to the RemoteWorker to create a permanent link.
   The two alternative modes differ in the speed and the number of sockets used.
   ETCP_SINGLE_DIRECTION is fastest and recommended if not on limited hardware.

   Ethernet strategies and related concepts are contributed by Fred Larsen. */

// Define one of these modes
#define ETCP_SINGLE_DIRECTION
//#define ETCP_SINGLE_SOCKET_WITH_ACK

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<EthernetTCP> bus(45);

// Ethernet configuration for this device
uint8_t gateway[] = { 192, 1, 1, 1 };
uint8_t subnet[] = { 255, 255, 255, 0 };
uint8_t mac[] = {0xD4, 0xED, 0x4C, 0xCF, 0xFE, 0xED};
uint8_t ip[] = { 192, 1, 1, 145 };

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to RemoteWorker 1 (Transmitter)");
  Ethernet.begin(mac, ip, gateway, gateway, subnet);

  bus.strategy.link.set_id(bus.device_id());
  busB.strategy.link.keep_connection(true);
  #ifdef ETCP_SINGLE_DIRECTION
    bus.strategy.link.single_initiate_direction(true);
  #else if ETCP_SINGLE_SOCKET_WITH_ACK
    bus.strategy.link.single_socket(true);
  #endif
  bus.set_receiver(receiver_function);
  bus.strategy.link.start_listening();
  bus.begin();
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'B') Serial.println("BLINK");
}

void loop() {
  bus.update();
  bus.receive(1000);

  // Show the number of sockets created after startup
  // (Try disconnecting the Ethernet cable for a while to see it increase when reconnected.)
  static uint32_t last = millis();
  if (millis() - last > 5000) {
    last = millis();
    Serial.print(F("CONNECT COUNT: "));
    Serial.println(bus.strategy.link.get_connection_count());
  }
};
