## Extender examples

This sketch will allow a local bus to be segmented. It can be used for:  
1. Extending a media like a repeater. For example if SWBB wires are getting too long, it can connect two SWBB wires, extending the range. This can be repeated to enable  really long distances.
2. Having devices on different media types communicate with each other transparently, for example having devices on a SWBB wire communicate with devices using the LocalUDP strategy on a LAN. Or wired <-> wireless.
3. Tunnel packets through another media type by using two extenders mirroring each other. For example, two SWBB buses that are far from each other can be joined transparently through a LAN using the LocalUDP strategy, or through a WAN or Internet using the EthernetTCP strategy. Or join two SWBB local bus segments wirelessly using the OS strategy.
4. Extend (3) to let packets flow freely between multiple SWBB buses on different media with multiple extenders exchanging the packets on a common media like a LAN with LocalUDP. The buses being connected this way can use a common strategy like SWBB, or different strategies.
      
A non-local version of this sketch, connecting two buses with different bus ids, is available as the "Router" sketch among the Network examples.
Extender examples contributed by Fred Larsen.
   
### Extender example

This example implements a device performing the first of the above roles, it simply repeats packets in both directions, allowing a bus to be segmented into multiple media of the same type or of different types.
Without changes, this example allows examples like local BlinkWithResponse to run on separate SWBB buses on each side of the extender.
If reaching the maximum length of a wire carrying a SWBB bus, it can be extended with this example.
It can also be used for connecting multiple media into a single local bus, for example a collection of wired SWBB devices and a collection of wireless devices using the OS strategy.

### Tunneler examples

You can connect multiple extenders in series, with or without devices connected to the buses between the extenders.
This pair or examples, ExtenderA and ExtenderB, is named Tunneler because without changes it allows a collection of SWBB devices to be connected to another collection of SWBB devices through two extenders connected via a Ethernet LAN using the LocalUDP strategy.
The SWBB traffic is "tunnelled" through another bus and medium. This corresponds to role 3 in the above list.

By having different device lists in each Extender and connecting multiple of them, we can obtain role 4 in the above list.
An example is that several collections of SWBB devices are connected together through one extender in each collection, communicating through a LocalUDP bus (Ethernet LAN). In this way, it possible to utilize existing infrastructure instead of pulling new cables or setting up new radio connections.

#### ExtenderA

The device has two buses, a SWBB bus named A and a LocalUDP bus named B. It has a list of devices that belong to the B bus, and will forward packets from bus A to bus B for these devices.
All packets appearing on bus B that are _not_ sent to these devices, will be forwarded to bus A.
In this way, all devices on both buses can talk to each other without knowing that the other device is on another bus.

#### ExtenderB

This device is similar to ExtenderA, but has the LocalUDP bus as bus A and a SWBB bus as bus B.
The list of devices on bus B is the same as in Extender A, meaning that all packets will be forwarded and that there are no devices on the LocalUDP bus connecting the two extenders.
By changing the device list, the LocalUDP bus (the Ethernet LAN) can also be populated with devices that will be able to communicate with all the other devices.
