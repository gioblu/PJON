
**Medium:** Ethernet port, wired or WiFi

With the EthernetTCP PJON strategy, multiple devices with Ethernet ports can use PJON to communicate with each other on a LAN, WAN or across the Internet.

####Why PJON over Ethernet TCP/IP?
If a cabled or wireless Ethernet network exists, using this to let devices communicate can be easier than to pull new wires or utilize other radio communication modules.

It can also be useful for connecting physically separate clusters of devices that are connected wired with the SoftwareBitBang strategy, or wirelessly with the Oversampling strategy, when an Ethernet network is connecting the locations.

It also enables devices to be connected through the Internet if firewalls are configured to allow this.

####How to use EthernetTCP
Pass the `EthernetTCP` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
  PJON<EthernetTCP> bus(44); // Use EthernetTCP strategy with PJON device id 44
```
Set up the Ethernet card in the usual manner by calling `Ethernet.begin`, then call setter functions for the link object in the EthernetTCP strategy, and then call the `begin` method on the PJON object:
```cpp
  void setup() {
    Ethernet.begin(mac, local_ip, gateway, gateway, subnet);

    bus.strategy.link.set_id(bus.device_id());
    bus.strategy.link.add_node(44, remote_ip);
    bus.strategy.link.keep_connection(true);
    bus.strategy.link.single_socket(false);
    bus.strategy.link.start_listening(); // Do not call this if SINGLE_SOCKET and transmitter

    bus.set_receiver(receiver_function);
    bus.begin();
  }
```
Note that the device id of each PJON device to communicate with must be added with the `add_node` function along with the device's IP address. The IP addresses can be completely unrelated, allowing it to communicate with devices all over the world as if they were on a local bus. DNS is currently not supported (and would lower the reliability because communication would be depending on the DNS server to be reachable), so all devices to be reached must have static IP addresses. Usually a static IP addresse can be leased for an Internet connection, sometimes for as low as $1 a month.

If the device will only talk to other devices with the single-socket configuration (calling `bus.strategy.link.single_socket(true)`) it can have a DHCP assigned IP address itself.

####Keep connection
If this option is set, a socket connection is not closed after a packet has been transferred. Instead it will remain open as long as possible to speed up the packet delivery, as establishing a new socket connection takes time. The Ethernet shields used with Arduino cards have a very limited number of simultaneous socket connections (1, 4 or 8), limiting the usefulness of these persistent connections. So only when communicating with very few devices will it give maximum performance.

####Single socket
By default, each device will listen for incoming socket connections on one port while creating outgoing connections to other devices for outgoing packets. So there is one connection in each direction, allowing packets to be sent in parallel in different directions, as far as that goes with single-threaded programs. Establishing sockets in both directions requires both devices to have fixed IP addresses, and if there are firewalls in between, there must be openings or port-forwarding in both directions.

To simply traffic crossing firewalls, the use of a single-socket configuration, calling `bus.strategy.link.single_socket(true)` is available. It makes one device create a connection to another devices with a fixed IP and a firewall opening / port forwarding. Packets can be send in both directions on this single socket, even if one device has dynamic IP address and no incoming firewall openings.

A use case for this is having a master device and a collection of slave devices in different locations that connect to the master to send or receive packets. Only the master need a fixed IP address and one firewall opening / port forwarding.

Using the single-socket configuration, calling `bus.strategy.link.single_socket(true)`, will roughly halve the effective bandwidth compared to keeping one connection in each direction, and it will cause some traffic (poll requests) to flow each time PJON update() or receive() are called even when no packets are being sent.

####Use-cases
When communicating on a LAN, maximum performance is obtained by using multiple sockets and keeping them open as long as possible. This is obtained by setting `bus.strategy.link.keep_connection(true)` and `bus.strategy.link.single_socket(false)`.

When communicating through firewalls, or only one of the devices having a fixed IP address, the configuration should be single-socket or `bus.strategy.link.single_socket(true)`.

####EthernetLink worker class
The `EthernetTCP` strategy looks simple because most of the implementation is present in a worker class named `EthernetLink`. This class can be used standalone in some contexts.

All the other necessary information is present in the general [Documentation](https://github.com/gioblu/PJON/wiki/Documentation).
