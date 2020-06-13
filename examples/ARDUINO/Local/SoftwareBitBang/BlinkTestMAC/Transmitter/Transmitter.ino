#define PJON_INCLUDE_MAC
#include <PJONSoftwareBitBang.h>

// <Strategy name> bus(mac address of the network interface)
const uint8_t mac[6] = {2, 3, 4, 5, 6, 7};
const uint8_t rx_mac[6] = {1, 2, 3, 4, 5, 6};
PJONSoftwareBitBang bus(mac);

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  PJON_Packet_Info info;
  info.header = bus.config | PJON_MAC_BIT;
  memcpy(info.rx.mac, rx_mac, 6);
  bus.send_repeatedly(info, "B", 1, 1000000); // Send B to MAC 1,2,3,4,5,6 every second
}

void loop() {
  bus.update();
};
