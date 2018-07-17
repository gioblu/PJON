### LocalUDP

**Medium:** Ethernet port, wired or WiFi

With the `LocalUDP` PJON strategy, multiple devices with Ethernet ports can use PJON to communicate with each other on a local subnet, wired or over WiFi or both. Take a look at the [video introduction](https://www.youtube.com/watch?v=cxEUqkK5BQg) for a brief showcase of its features.

#### Why PJON over UDP?
If a cabled or wireless Ethernet network exists, using this to let devices communicate can be easier than to pull new wires or utilize other radio communication modules.

It can also be useful for connecting physically separate clusters of devices that are connected wired with the SoftwareBitBang strategy, or wirelessly with the Oversampling strategy, when a LAN is connecting the locations.

#### How to use LocalUDP
Pass the `LocalUDP` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
  // Use LocalUDP strategy with PJON device id 44
  PJON<LocalUDP> bus(44);
```
Set up the Ethernet card in the usual manner by calling `Ethernet.begin`, then call the `begin` method on the PJON object:
```cpp  
  void setup() {
    Ethernet.begin(mac, local_ip, gateway, gateway, subnet);
    bus.begin();
  }
```
The IP address used is irrelevant as long as it is on a subnet common with the other devices it shall communicate with.
Using DHCP assigned IP addresses is fine, and the strategy does not need to relate to it.
The strategy will broadcast the packets, and the correct receiver will pick them up and ACK if requested. Other devices will observe but ignore packets not meant for them.

All the other necessary information is present in the general [Documentation](/documentation).

#### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. When connecting a local bus to the internet using [EthernetTCP](/src/strategies/EthernetTCP) or [GlobalUDP](/src/strategies/GlobalUDP) all connected devices must be considered potentially compromised, potentially manipulated or remotely actuated against your will. It should be considered a good practice not to connect to the internet systems that may create a damage (fire, flood, data-leak) if hacked.
