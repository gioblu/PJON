
# Surrogate and RemoteWorker concept
Instead of having a worker device connected directly to a PJON `SoftwareBitBang` bus, this concept allows for it to run
somewhere else, connected via Ethernet to a surrogate on the bus. The `RemoteWorker` acts as if it was on the actual
`SoftwareBitBang` bus, while actually using a `EthernetTCP` bus in a special mode. It can send packets to specific device ids,
and will receive packets that are sent to its device id.

The Surrogate runs on for example an Arduino Uno or Nano with an Ethernet card.
The `RemoteWorker` can do the same, just being located in another physical location connected through a network.
But things get more interesting when running the `RemoteWorker` on hardware not capable of using `SoftwareBitBang`,
enabling for example a Raspberry PI to communicate in a `SoftwareBitBang` bus through a `Surrogate`.

Also, the `RemoteWorker` can run on a computer, allowing a lot of possibilities, for example a personal computer running whatever operative system to be part of a `SoftwareBitBang` bus.

In addition to the C++ PJON implementation, there is also a java class that can be used so that any java program can
be part of the bus via an Ethernet or Serial connection to a Surrogate.

## The Surrogate device
The surrogate will have the same bus id on the `SoftwareBitBang` bus as the `RemoteWorker` has on the `EthernetTCP` bus (which is private between the `Surrogate` and `RemoteWorker` devices). It will pass any packets to that id on the `SoftwareBitBang` bus to the `RemoteWorker`, and forward all packets from the `RemoteWorker` to the correct device on the `SoftwareBitBang` bus, acting as a transparent link.

## The RemoteWorker device
The `RemoteWorker` name is just a concept for the role this device plays in relation to the SWBB bus. It can be named anything
and do whatever work is wanted.

Not relevant for an Arduino, but for a `RemoteWorker` program running on a multi-threaded OS, is that the program may be a
"master" that interacts in multiple buses via one `Surrogate` in each bus, all connected through Ethernet. Fred Larsen's [ModuleInterface](https://github.com/fredilarsen/ModuleInterface) library has a java implementation which uses this approach for controlling and gathering information from many devices spread on different PJON buses. One threaded object is used for each remote bus, so it is easy to add new buses without meetings constraints or lowering speed.

## Ethernet
The `EthernetTCP` mode `single_initiate_direction` used in this scheme establishes one socket for each packet direction, but
initiates these socket in the direction from the Surrogate to the `RemoteWorker`. The `RemoteWorker` can be in a central position, with port forwarding for `Surrogates` on potentially multiple remote buses. This allows for a "master" to be in a central position behind a router that just forwards a TCP port.

## Security
There is no built-in security except for handshaking to make sure unrelated socket connections are not accepted, or
created to non-related services. If an incoming connection does not deliver the expected response at any point, the
connection is immediately terminated. Incoming packets have a maximum allowed size to avoid buffer overflow.

So this should be used:
* On secure LANs and WANs alone
* Through LANs connected through Internet with VPN
* Through Internet directly for testing, only if transferring non-important data and interruptions are not critical.
Limiting incoming TCP port forwarding in a router to specific remote IP addresses reduces the probability of unwanted
interruptions. The Cape library can be used to encrypt packets on each end.
