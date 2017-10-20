This pair of examples demonstrates the use of the PJON LocalUDP strategy between Windows computers.

This PingPong setup consists of a Transmitter example and a Receiver example. The Transmitter sends a PING packet to the Receiver via PJON a number of times per second, and the Receiver replies to each PING with a PONG.
A printout shows the number of transferred packets per second.

Note that the LocalUDP strategy broadcasts packets to the LAN, and picks up packets for PJON to filter and process. This method means that no IP addresses need to be configured.

A Visual Studio 2017 solution is present, so it is easy to open and build with this software. The Visual Studio Express edition is free.
