## Extender examples

This sketch allows a local bus to be segmented. It can be used for:  
1. Extending its medium as a repeater, for example if `SoftwareBitBang` bus wires are getting too long, they can be splitted in two sections connected by a repeater, extending range. This scheme can be repeated to enable really long distances.
2. Let devices on different media communicate with each other transparently, for example to let devices on a `SoftwareBitBang` bus wire communicate with devices using the `LocalUDP` strategy on a LAN.
3. Tunnel packets through another medium using two extenders mirroring each other. For example, two `SoftwareBitBang` buses that are far from each other can be joined transparently through a LAN using the `LocalUDP` strategy, or through a WAN or Internet using the `EthernetTCP` strategy.
4. Extend (3) to let packets flow freely between multiple `SoftwareBitBang` buses on different media with multiple extenders exchanging packets on a common medium like a LAN using `LocalUDP` strategy.

A non-local version of this sketch, connecting two buses with different bus ids, is available as the "Router" sketch among the Network examples. Extender examples contributed by Fred Larsen.

### Extender example

This example implements a device performing the first of the above roles, it simply repeats packets in both directions, allowing a bus to be segmented into multiple buses using the same media or different types.
Without changes, this example allows examples like local `BlinkWithResponse` to run on separate `SoftwareBitBang` buses on each side of the extender. If reaching the maximum length of a wire carrying a `SoftwareBitBang` bus, it can be extended with this example. It can also be used for connecting multiple media into a single local bus, for example a collection of wired `SoftwareBitBang` devices and a collection of wireless devices using the `OverSampling` strategy.

### Tunneler examples

You can connect multiple extenders in series, with or without devices connected to the buses between the extenders.
This pair or examples, `ExtenderA` and `ExtenderB`, is named `Tunneler` because without changes it allows a collection of `SoftwareBitBang` devices to be connected to another collection of `SoftwareBitBang` devices through two extenders connected via a Ethernet LAN using the `LocalUDP` strategy.
The `SoftwareBitBang` traffic is "tunnelled" through another bus and medium. This corresponds to role 3 in the above list.

By having different device lists in each Extender and connecting multiple of them, we can obtain role 4 in the above list.
An example is that several collections of `SoftwareBitBang` devices are connected together through one extender in each collection, communicating through a `LocalUDP` bus (Ethernet LAN). In this way, it possible to utilize existing infrastructure instead of pulling new cables or setting up new radio connections.

#### ExtenderA

The device has two buses, a `SoftwareBitBang` bus named `A` and a `LocalUDP` bus named `B`. It has a list of devices that belong to the `B` bus, and will forward packets from bus `A` to bus `B` for these devices.
All packets appearing on bus `B` that are _not_ sent to these devices, will be forwarded to bus `A`.
In this way, all devices on both buses can talk to each other without knowing that the other device is on another bus.

#### ExtenderB

This device is similar to `ExtenderA`, but has the `LocalUDP` bus as bus `A` and a `SoftwareBitBang` bus as bus `B`.
The list of devices on bus `B` is the same as in Extender `A`, meaning that all packets will be forwarded and that there are no devices on the `LocalUDP` bus connecting the two extenders.
By changing the device list, the `LocalUDP` bus (the Ethernet LAN) can also be populated with devices that will be able to communicate with all the other devices.
