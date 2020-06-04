# Tunneler examples

You can connect multiple routers, or in this case switches with virtual bus support, in series, with or without devices connected to the buses between the switches.

This pair of examples, `SwitchA` and `SwitchB`, is named `Tunneler` because without changes it allows a collection of `SoftwareBitBang` devices to be connected to another collection of `SoftwareBitBang` devices through two switches connected via a LAN using the `DualUDP` strategy.
The `SoftwareBitBang` traffic is "tunnelled" through another bus and medium. 

This is not limited to two groups of devices. You can add another 'SwitchC' and a third group of devices connected with the other two groups by just copying SwitchA.ino and modifying the MAC address so that it is unique on the network.

You can also have a switch or other program using the DualUDP strategy and the same UDP port running on 
a Windows or Linux computer. This enables easy connectivity to programs running on a computer, and with the outside world.

This setup is running in local mode, not using any bus ids, so all device ids involved must be unique. The switches themselves work in router mode and do not have device ids.

These sketches do not use the default DualUDP port. Instead they use port 7200 (randomly selected) to have a "private" or "tunneled" connection, not mixing with any other DualUDP setups sharing the same LAN.

## SwitchA

The device is a switch with virtual bus support, meaning that multiple attached buses can use the same bus id or local mode, with the switch adapting to where devices are located. The user does not have to manually assign a specific device range to each attached bus.

It has two buses, a `SoftwareBitBang` bus named `A` and a `DualUDP` bus named `B`. 

The Ethernet setup is using DHCP to avoid manually configured IP addresses in the skethes. This means that you need to connect both devices to your LAN, not just connect a cable betwen them.

But because DualUDP is based on UDP broadcasts, it will actually start working with a direct Ethernet cable between `SwitchA` and `SwitchB` after the attempt to get an IP address times out after 60 seconds.

## SwitchB

This device is identical to `SwitchA`, except for a different MAC address which is needed to make the device unique on the LAN.
