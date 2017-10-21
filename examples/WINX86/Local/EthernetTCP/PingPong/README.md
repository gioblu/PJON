This pair of examples demonstrates the use of the PJON EthernetTCP strategy between Windows computers.

This PingPong setup consists of a Transmitter example and a Receiver example. The Transmitter sends a PING packet to the Receiver via PJON a number of times per second, and the Receiver replies to each PING with a PONG.
A printout shows the number of transferred packets per second.

Note that the EthernetTCP strategy sends directed messages to specific IP addresses, so each device must be registered as a node with PJON id, IP address and potentially TCP port before starting. It is not limited to the LAN but can traverse network boundaries as long as there is a port forwarding / firewall opening for the selected TCP port.

The standard mode allows a group of nodes to talk to each other like on a PJON bus, but it requires port forwarding in both directions between all node combinations if not running on a single LAN.

In addition to the standard mode, the _single-direction_ and _single-socket_ modes allow one-to-one bidirectional communication with port forwarding in only one direction. So only one site would need a fixed IP address. This is not demonstrated in this example, but look at the Surrogate example if interested.

A Visual Studio 2017 solution is present, so it is easy to open and build with this software. The Visual Studio Express edition is free.
