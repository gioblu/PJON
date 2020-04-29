## GlobalUDP

**Medium:** Ethernet port, wired or WiFi |
**Pins used:** NA |
**Inclusion constant:** `PJON_INCLUDE_GUDP`

With the `GlobalUDP` PJON strategy, multiple devices with Ethernet ports can use PJON to communicate with each other over an
Ethernet network, wired or over WiFi or both. This strategy demands a little more configuration than the `LocalUDP` strategy
but is not limited to the local network and can therefore reach devices farther away, to another LAN connected through
VPN, or potentially across the Internet (beware of security issues).

### Why PJON over UDP?
If a cabled or wireless Ethernet network exists, using this to let devices communicate can be easier than to pull new wires or utilize other radio communication modules.

It can also be useful for connecting physically separate clusters of devices that are connected wired with the `SoftwareBitBang` strategy, or wirelessly with the `Oversampling` strategy, when a LAN or WAN is connecting the locations.

### How to use GlobalUDP
Pass the `GlobalUDP` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
  // Use LocalUDP strategy with PJON device id 44
  PJON<LocalUDP> bus(44);
```
Set up the Ethernet card in the usual manner by calling `Ethernet.begin`, register the other devices to send to,
then call the `begin` method on the PJON object:
```cpp  
void setup() {
  Ethernet.begin(mac, local_ip, gateway, gateway, subnet);
  bus.strategy.add_node(45, remote_ip1);
  bus.strategy.add_node(46, remote_ip2);
  bus.begin();
}
```
All the IP addresses of the registered nodes should be reachable. UDP port forwarding can be used to obtain this
through firewalls. The IP address of the device can be DHCP assigned if none of the other devices need to reach it
except with ACKs. Otherwise it should be static, unless using sender autoregistration.

Sender autoregistration is now enabled by default and can be disabled with:

```
  bus.set_autoregistration(false);
```

With sender autoregistration the sender of each incoming packet is automatically registered in the node table, meaning that replies will work also for unregistered devices with static or dynamic IP. Also, after a packet from a device has been received, packets can be sent to it by its PJON id.

This means that sender autoregistration can be used in setups where packets are exchanged through a central device, typically a master or switch, to let all devices except the central device use DHCP for dynamic network configuration. Each device then need to register the central device in its table, and the central device can have an empty table at startup.

Note that the preprocessor define `GUDP_MAX_REMOTE_NODES` is important when using autoregistration. For a device it should be higher than the maximum number of other devices it will communicate with. Its default value of 10 is low to save memory, and in larger setups it must be increased.

UDP packets are _not_ broadcast like with the `LocalUDP` strategy, but directed to a selected receiver.

All the other necessary information is present in the general [Documentation](/documentation).

### Known issues
- Firewall may block `GlobalUDP` packets, edit its configuration to allow them

### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. When connecting a local bus to the internet using [EthernetTCP](/src/strategies/EthernetTCP) or [GlobalUDP](/src/strategies/GlobalUDP) all connected devices must be considered potentially compromised, potentially manipulated or remotely actuated against your will. It should be considered a good practice not to connect to the internet systems that may create a damage (fire, flood, data-leak) if hacked.
