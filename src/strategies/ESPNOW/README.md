### ESPNOW

**Medium:** 802.11 (WiFi)

With the `ESPNOW` PJON strategy, up to 10 ESP32 devices can use PJON to communicate with each other over the Espressif
ESPNOW protocol (peer-to-peer 802.11).

#### Why not use PJON over WiFi

WiFi is a "best effort" medium - using PJON over WiFI requires a Router and configuration of SSID, Password and IP Addresses.

PJON over ESPNOW has the benefits over wifi of :-

* Lower latency (no router is required - the devices communicate directly to each other)
* Auto configuration (devices register on the group when starting up and so are accessable by device_id immediately)

#### How to use GlobalUDP
Pass the `ESPNOW` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
  // Use ESPNOW strategy with PJON device id 44
  PJON<ESPNOW> bus(44);
```
Set up the Ethernet card in the usual manner by calling `Ethernet.begin`, register the other devices to send to,
then call the `begin` method on the PJON object:
```cpp  
void setup() {
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

#### Known issues
- Firewall may block `GlobalUDP` packets, edit its configuration to allow them

#### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. When connecting a local bus to the internet using [EthernetTCP](/src/strategies/EthernetTCP) or [GlobalUDP](/src/strategies/GlobalUDP) all connected devices must be considered potentially compromised, potentially manipulated or remotely actuated against your will. It should be considered a good practice not to connect to the internet systems that may create a damage (fire, flood, data-leak) if hacked.
