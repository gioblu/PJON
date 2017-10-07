
**Medium:** Ethernet port, wired or WiFi

With the `EthernetTCP` PJON strategy, multiple devices with Ethernet ports can use PJON to communicate with each other on a LAN, WAN or across the Internet. Take a look at the [video introduction](https://www.youtube.com/watch?v=DQzcAv38yxM) for a brief showcase of its features.

#### Why PJON over Ethernet TCP/IP?
If a cabled or wireless Ethernet network exists, using this to let devices communicate can be easier than to pull new wires or utilize other radio communication modules.

It can also be useful for connecting physically separate clusters of devices that are connected wired with the `SoftwareBitBang` strategy, or wirelessly with the `Oversampling` strategy, when an Ethernet network is connecting the locations.

It also enables devices to be connected through the Internet if firewalls are configured to allow this.

#### How to use EthernetTCP
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
    bus.strategy.link.start_listening(); // Do not call this if SINGLE_SOCKET and transmitter

    bus.set_receiver(receiver_function);
    bus.begin();
  }
```
Note that the device id of each PJON device to communicate with must be added with the `add_node` function along with the device's IP address. The IP addresses can be completely unrelated, allowing it to communicate with devices all over the world as if they were on a local bus. DNS is currently not supported (and would lower the reliability because communication would be depending on the DNS server to be reachable), so all devices to be reached must have static IP addresses. Usually a static IP address can be leased for an Internet connection, sometimes for as low as $1 a month.

If the device will only talk to one other device with the single-socket configuration (calling `bus.strategy.link.single_socket(true)`) or single-direction (calling `bus.strategy.link.single_initiate_direction(true)`) it can have a DHCP assigned IP address itself.

#### Single socket
In the standard mode, each device will listen for incoming socket connections on one port while creating outgoing connections to other devices for outgoing packets. So there is one connection in each direction, allowing packets to be sent in parallel in different directions, as far as that goes with single-threaded programs. Establishing sockets in both directions requires both devices to have fixed IP addresses, and if there are firewalls in between, there must be openings or port-forwarding in both directions.

To simplify traffic crossing firewalls, the use of a single-socket configuration, calling `bus.strategy.link.single_socket(true)` is available. It makes one device create a connection to another device with a fixed IP and a firewall opening / port forwarding. Packets can be send in both directions on this single socket, even if one device has dynamic IP address and no incoming firewall openings.

A use case for this is having a master device, and a slave device in a different location that connect to the master to send or receive packets. Only the master needs a fixed IP address and one firewall opening / port forwarding.

Using the single-socket configuration, calling `bus.strategy.link.single_socket(true)`, will cause some traffic (poll requests) to flow each time PJON update() or receive() are called even when no packets are being sent.

Unlike the standard mode, this mode will not establish and close a connection for each packet to be transferred, but keep the socket connection permanently. It will be re-connected if it fails.

#### Single initiate direction
Just like the single-socket mode, this mode allows one device to connect to another device, and let packets flow in both directions.

Instead of creating one single socket like the single-socket mode, this mode creates one socket for each packet direction, but both sockets are established in the same direction for easy firewall traversal.
This mode is not poll-based like the single-socket approach, so it will not generate network traffic when idle.

This mode is also based on permanently connected sockets.

#### Use-cases
When communicating on a LAN between multiple devices, the standard mode should be used. This will open a socket from a sender to a receiver for each packet to be sent, send the packet, receive an ACK, and close the socket.
In this scenario, the LocalUDP or GlobalUDP strategies may be smaller and more efficient strategies.

When communicating through firewalls, or only one of the devices having a fixed IP address, the configuration should be single-direction with `bus.strategy.link.single_Initiate_direction(true)` or single-socket with `bus.strategy.link.single_socket(true)`.

One use case is to have a PJON program on a computer not able to run a strategy like SWBB take part in a SWBB bus by using a Surrogate. So a Windows/Linux/Raspberry program can communicate with all devices on a SWBB bus via an Arduino running the Surrogate example sketch. The program will talk to the Surrogate via EthernetTCP in single-direction or single-socket mode, and the Surrogate will forward messages in both directions between the ETCP bus and the SWBB bus.

#### EthernetLink worker class
The `EthernetTCP` strategy looks simple because most of the implementation is present in a worker class named `EthernetLink`. This class can be used standalone in some contexts.

All the other necessary information is present in the general [Documentation](/documentation).
