/* Tunnel packets through another medium using one switch on each end. 
/  For example, two SoftwareBitBang buses that are far from each other
/  can be joined transparently through a LAN using the LocalUDP strategy, or
/  through a WAN or Internet using the EthernetTCP strategy. */

#define PJON_MAX_PACKETS 3
#include <PJONVirtualBusRouter.h>

// Ethernet configuration for this device, MAC must be unique!
byte mac[] = {0xDE, 0x34, 0x4E, 0xEF, 0xFE, 0xE1};

StrategyLink<SoftwareBitBang> link1;
StrategyLink<LocalUDP> link2;

PJONAny bus1(&link1);
PJONAny bus2(&link2);

PJONVirtualBusRouter<PJONSwitch> router(2, (PJONAny*[2]){&bus1, &bus2});

void setup() {
  Serial.begin(115200);
  Ethernet.begin(mac); // Use DHCP
  link1.strategy.set_pin(7);
  link2.strategy.set_port(7200); // Use a "private" UDP port
  router.begin();
}

void loop() {
  router.loop();
};