/* This sketch lets a RemoteWorker device connected through Ethernet TCP act
   as if it is present on the SoftwareBitBang bus connected to this Surrogate
   device.

   The RemoteWorker sketch can the run on a device not capable of SWBB but
   with Ethernet support, like a PC or a Raspberry PI.

 Surrogate and RemoteWorker examples contributed by Fred Larsen. */

#define PJON_PACKET_MAX_LENGTH 50
#define PJON_MAX_PACKETS 3

//// Define one of these modes
//#define ETCP_SINGLE_DIRECTION
////#define ETCP_SINGLE_SOCKET_WITH_ACK

#include <PJON.h>

const uint8_t DEVICE_ID = 44;
// SWBB Device ID for this device and the RemoteWorker

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> busA(DEVICE_ID);
//PJON<EthernetTCP> busB(1);
PJON<ThroughSerial> busB(1);

//// Ethernet configuration for this device
//uint8_t gateway[] = { 192, 1, 1, 1 };
//uint8_t subnet[] = { 255, 255, 255, 0 };
//uint8_t mac[] = {0xDE, 0x5D, 0x4E, 0xEF, 0xAE, 0xED};
//uint8_t ip[] = { 192, 1, 1, 144 };

//// Ethernet configuration for remote device
//const uint8_t remote_ip[] = { 192, 1, 1, 70 };

void setup() {
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW); // Initialize LED 13 to be off
//  digitalWrite(13, HIGH); // Initialize LED 13 to be on

//  Serial.begin(115200);
//  Serial.println("Welcome to Surrogate 1");
//  Ethernet.begin(mac, ip, gateway, gateway, subnet);
  //Serial.begin(115200);
  Serial1.begin(115200); 

  busA.strategy.set_pin(2);
  busA.set_receiver(receiver_functionA);
  busA.begin();

  busB.strategy.set_serial(&Serial1);
  busB.set_router(true);
  busB.set_receiver(receiver_functionB);
  busB.begin();

    attachInterrupt(digitalPinToInterrupt(2), intFunc , RISING);

}


void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  noInterrupts();
  busA.receive();
  interrupts();
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
    packet_info.header,
    packet_info.id,
    packet_info.port
  );
  digitalWrite(5, HIGH);
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
    packet_info.header,
    packet_info.id,
    packet_info.port
  );  
  digitalWrite(5, LOW);
}

void loop() {
  //busA.receive(1000);
  busB.update();
  busB.receive();
  busA.update();


};
