# Surrogate and RemoteWorker concept
Instead of having a worker device connected directly to a PJON SoftwareBitBang bus, this concept allows for it to run
somewhere else, connected via Ethernet to a surrogate on the bus. The RemoteWorker acts as if it was on the actual
SoftwareBitBang bus, while actually using a EthernetTCP bus in a special mode. It can send packets to specific device ids,
and will receive packets that are sent to its device id.

The Surrogate runs on for example an Arduino Uno or Nano with an Ethernet card.
The RemoteWorker can do the same, just being located in another physical location connected through a network.
But things get more interesting when running the RemoteWorker on hardware not capable of using SoftwareBitBang,
enabling for example a Raspberry PI to parttake in a SWBB bus via a Surrogate.

Also, the RemoteWorker can run on a PC. This allows for a lot of possibilities, allowing a PC (any OS) to
parttake in a SoftwareBitBang bus.

In addition to the C++ PJON implementation, there is also a java class that can be used so that any java program can
be part of the bus via an Ethernet or Serial connection to a Surrogate.

## The Surrogate device
The surrogate will have the same bus id on the SWBB bus as the RemoteWorker has on the ETCP bus (which is private between the
Surrogate and RemoteWorker devices).
It will pass any packets to that id on the SWBB bus to the RemoteWorker, and forward all packets from the RemoteWorker to
the correct device on the SWBB bus, acting as a transparent link.

## The RemoteWorker device
The RemoteWorker name is just a concept for the role this device plays in relation to the SWBB bus. It can be named anything
and do whatever work is wanted.

Not relevant for an Arduino, but for a RemoteWorker program running on a multi-threaded OS, is that the program may be a
"master" that parttakes in multiple buses via one Surrogate in each bus, all connected through Ethernet. Fred Larsen's
ModuleInterface library has a java implementation which uses this approach for controlling and gathering information from
many devices spread on different PJON buses. One threaded object is used for each remote bus, so it is easy to add new buses
without meetings contraints or lowering speed.

## Ethernet
The EthernetTCP mode single_initiate_direction used in this scheme establishes one socket for each packet direction, but
initiates these socket in the direction from the Surrogate to the RemoteWorker.
The reason is that the RemoteWorker can be in a central position, with port forwarding for Surrogates on potentially
multiple remote buses.

This allows for a "master" to be in a central position behind a router that just forwards a TCP port.

## Security
There is no built-in security except for handshaking to make sure unrelated socket connections are not accepted, or
created to non-related services. If an incoming connection does not deliver the expected response at any point, the
connection is immediately terminated. Incoming packets have a maximum allowed size to avoid buffer overflow.

So this should be used:
* On secure LANs and WANs alone
* Through LANs connected through Internet with VPN
* Through Internet directly for testing, only if transferring non-important data and interruptions are not critical.
Limiting incoming TCP port forwardings in a router to specific remote IP addresses reduces the probability of unwanted
interruptions. The Cape library can be used to encrypt packets on each end.
