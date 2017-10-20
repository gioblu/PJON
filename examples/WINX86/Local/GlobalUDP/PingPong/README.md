This pair of examples demonstrates the use of the PJON GlobalUDP strategy between Windows computers.

This PingPong setup consists of a Transmitter example and a Receiver example. The Transmitter sends a PING packet to the Receiver via PJON a number of times per second, and the Receiver replies to each PING with a PONG.
A printout shows the number of transferred packets per second.

Note that the GlobalUDP strategy sends directed messages to specific IP addresses, so each device must be registered as a node with PJON id, IP address and potentially UDP port before starting. It does not broadcast like the LocalUDP strategy, so it needs fixed IP addresses on all devices. It is not limited to the LAN but can traverse network boundaries as long as there is a port forwarding / firewall opening for the selected UDP port.

A Visual Studio 2017 solution is present, so it is easy to open and build with this software. The Visual Studio Express edition is free.
