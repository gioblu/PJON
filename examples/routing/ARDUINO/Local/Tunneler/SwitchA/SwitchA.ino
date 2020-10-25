/* Tunnel packets through another medium using one switch on each end.
/  For example, two SoftwareBitBang buses that are far from each other
/  can be joined transparently through a LAN/WAN using the DualUDP strategy
/  or the EthernetTCP strategy.
/  In this case, use a non-default UDP port to isolate the "tunnel" from
/  potential other DualUDP communication on the same network. */

#define PJON_MAX_PACKETS 3
#include <PJONVirtualBusRouter.h>
#include <PJONSoftwareBitBang.h>
#include <PJONDualUDP.h>

// Ethernet configuration for this device, MAC must be unique!
byte mac[] = {0xDE, 0x34, 0x4E, 0xEF, 0xFE, 0xE1};

PJONVirtualBusRouter2<SoftwareBitBang, DualUDP> router;

void setup() {
  while (Ethernet.begin(mac) == 0) delay(5000); // Wait for DHCP response
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_port(7200); // Use a "private" UDP port
  router.set_virtual_bus(0); // Enable virtual bus
  router.begin();
}

void loop() {
  router.loop();
  Ethernet.maintain(); // Maintain DHCP lease
};