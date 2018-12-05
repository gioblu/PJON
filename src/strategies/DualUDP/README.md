### DualUDP

**Medium:** Ethernet port, wired or WiFi

With the `DualUDP` PJON strategy, multiple devices with Ethernet ports can use PJON to communicate with each other over an Ethernet network, wired or over WiFi or both. Like the `GlobalUDP` strategy, this strategy is not limited to the local network 
and can therefore reach devices farther away, to another LAN connected through VPN, or potentially across the Internet (beware of security issues). Like the `LocalUDP` strategy it will reach devices on the LAN without configuration.

Feature summery:
* Will autodiscover devices on the LAN. These can use DHCP assigned IP addresses.
* Can reach devices outside the LAN if they are added to the node table. These must have static IP, or send a packet to this device to be registered so that their IP is known.
* Steps from UDP broadcast to directed packets after device discovery.

#### Why PJON over DualUDP?
If a cabled or wireless Ethernet network exists, using this to let devices communicate can be easier than to pull new wires or utilize other radio communication modules.

It can also be useful for connecting physically separate clusters of devices that are connected wired with the `SoftwareBitBang` strategy, or wirelessly with the `Oversampling` strategy, when a LAN or WAN is connecting the locations.

#### How to use DualUDP
Pass the `DualUDP` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
  // Use LocalUDP strategy with PJON device id 44
  PJON<LocalUDP> bus(44);
```

Now the Ethernet card must be set up. This can be done by using a static IP address or by getting an address assigned by DHCP.

Set up the Ethernet card in the usual manner by calling `Ethernet.begin`, register the other devices to send to, then call the `begin` method on the PJON object:
```cpp  
void setup() {
  // Configure a fixed IP address
  Ethernet.begin(mac, local_ip, gateway, gateway, subnet);

  // Add two devices to the node table
  bus.strategy.add_node(45, remote_ip1);
  bus.strategy.add_node(46, remote_ip2);
  bus.begin();
}
```
or
```cpp  
void setup() {
  while (!Ethernet.begin(mac)); // Wait for DHCP
  bus.begin();
}
```

#### Node table
DualUDP has a "node" table that is a list of devices with PJON id, IP address and port number (usually all using the same standard port number). A device in this table can be contacted directly by sending a packet specifically to its IP and port. The core functionality requires all relevant devices to be registered here, otherwise they cannot be reached or replied to because the IP address is not known.

But since this is cumbersome, a couple of options have been added to make life simpler:
1. Adding senders of packets that are sent to this device automatically to the node table so that it is possible to reply. This option is named _auto_registration_ and can be disabled by a setter.
2. Trying to broadcast a packet when the receiver is not registered in the table. This will be picked up by the receiver if it is on the same network segment (LAN) and uses the same port number. If the device ACKs the broadcast packet, option 1 above will add it to the node table so that it can be contacted without broadcast from that point on. This is named _auto_discovery_ and can be disabled by a setter.

These two options are enabled by default, making it possible to use DHCP on every device on a LAN, and to have no devices in the node table at startup. This makes it equally simple to use as LocalUDP, but with the strengths of GlobalUDP, therefore the name DualUDP.
After starting, all packets will be broadcast, but as packets are ACKed or replied to, the table will be populated and directed packets will be used instead. This does not disturb other devices like broadcast does, and if the traffic is high broadcasts may also put a load on any WiFi network connected to the LAN because all broadcast packets are sent over air as well.

Note that the preprocessor define `DUDP_MAX_REMOTE_NODES` is important when using autoregistration. For a device it should be higher than the maximum number of other devices it will communicate with. Its default value of 10 is low to save memory, and in larger setups it must be increased, otherwise broadcast will be used for devices not registered in the table.

#### Remote devices
Devices not being present on the LAN will not be reached by broadcasts and will therefore not be automatically discovered unless they send a packet to this device. So if a master device has a fixed IP address and remote devices in different locations have the master in their node table and send a packet at startup and at regular intervals (in case master is restarted), communication will be established.

Remote devices not sending any packet to this device will be unreachable unless they are manually added to the node table.

The node table can have one or more remote devices added to it at startup, with all relevant devices on the LAN being added automatically as needed.

All the IP addresses of the registered nodes should be reachable. UDP port forwarding can be used to obtain this through firewalls.

#### Device removal from table
If a device cannot be reached for a number of times (`DUDP_MAX_FAILURES`), it will be removed from the node table if it has been added automatically. If it has been added programmatically before the communication starts, it is kept in a static part of the table and will never be removed.

If a device is removed from the table, it will be "downgraded" to be reached by broadcast instead of directed packets. If it does ACK or sends a packet, it will again be added to the node table and "upgraded" to directed packets.

#### Static IP address mode
DualUDP can work in a static mode by requiring all devices to have static IP addresses and all remote devices to be registered as 'nodes' at setup. This allows it to talk equally to devices on the LAN and on remote networks, and have full control of a closed group.

To work in full manual mode, disable the two options:
```
  bus.set_autoregistration(false);
  bus.set_autodiscovery(false);
```
All the other necessary information is present in the general [Documentation](/documentation).

#### Known issues
- Firewall may block `DualUDP` packets, edit its configuration to allow them

#### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. When connecting a local bus to the internet using [EthernetTCP](/src/strategies/EthernetTCP) or [DualUDP](/src/strategies/DualUDP) all connected devices must be considered potentially compromised, potentially manipulated or remotely actuated against your will. It should be considered a good practice not to connect to the internet systems that may create a damage (fire, flood, data-leak) if hacked.
