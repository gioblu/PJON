- PJON Protocol specification:
[v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md) - **[v0.2](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md)** - [v0.3](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md) - [v1.0](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md)
- Acknowledge specification: [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Dynamic addressing specification: [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md)

```cpp
/*
Milan, Italy - 19/08/2015
The PJON™ protocol specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2017 All rights reserved

Related work: https://github.com/gioblu/PJON/
Compliant implementation versions: PJON 3.0-beta-3.0

New feature: Bus identification
*/
```

###PJON™ Protocol Specification v0.2
After more than 4 years of testing, a second, experimental draft of the PJON protocol specification has been released to continue to pursue the goal of providing a new and open-source, multi-master communications bus system Standard. The main changes contains the addition of a IPv4 like 4 byte bus id. This extends the network capabilities of the PJON protocol from 254 devices of the 0.1 version to 1.090.921.692.930 devices supported by the 0.2 version. It is created to provide the community with a new and easy way to communicate data and build a network of devices. Its more common applications are in the field of the internet of things and embedded systems. Extended tests proved its effectiveness on different media like electricity, radio and light.

###Basic concepts
* Every bus has a unique IPv4 like 4 bytes id
* Many buses can coexist on the same medium
* Every device has a unique 1 byte id
* Every device transmits and receives on the same common medium
* Every device has an equal right to transmit and receive on the common medium
* Every device can be connected to n PJON buses (with n dedicated pins)
* Transmission occurs only if the communication medium is not in use
* Synchronization occurs every byte
* Devices communicate through packets

The PJON protocol v0.2 handles internal bus connectivity and unique addressing for 254 devices, through bus communication with unique bus addressing for 4.294.967.295 buses, supporting up to 1.090.921.692.930 devices.

###Bus
A PJON bus is made by a group of up to 254 devices transmitting and receiving on the same medium. Communication between devices occurs through packets and it is based on democracy: every device has the right to transmit on the common medium for up to (1000 / devices number) milliseconds / second.
```cpp
    _______     _______     _______     _______     _______
   |       |   |       |   |       |   |       |   |       |  
   | ID 0  |   | ID 1  |   | ID 2  |   | ID 3  |   | ID 4  |  
   |_______|   |_______|   |_______|   |_______|   |_______|    
 ______|___________|___________|___________|___________|______
          ___|___     ___|___     ___|___     ___|___
         |       |   |       |   |       |   |       |   
         | ID 5  |   | ID 6  |   | ID 7  |   | ID 8  |
         |_______|   |_______|   |_______|   |_______|    
```

###Packet transmission
The concept of packet enables to send a communication payload to every connected device with correct reception certainty. A packet contains the recipient id, the length of the packet, its content and the CRC. In this example is shown a packet sending to device id 12 on a local bus containing the string "@":
```cpp
 ID 12       LENGTH 4    CONTENT 64  CRC 130
 __________  __________  __________  ____________
| Byte     || Byte     || Byte     || Byte       |
|     __   ||      _   ||  _       ||  _      _  |
|    |  |  ||     | |  || | |      || | |    | | |
|0000|11|00||00000|1|00||0|1|000000||0|1|0000|1|0|
|____|__|__||_____|_|__||_|_|______||_|_|____|_|_|
```

A standard local packet transmission is a bidirectional communication between two devices that can be divided in 3 different phases: channel analysis, transmission and response.
```cpp
  Channel analysis   Transmission                            Response
      _____           _____________________________           _____
     | C-A |         | ID | LENGTH | CONTENT | CRC |         | ACK |
  <--|-----|---------|----|--------|---------|-----|--> <----|-----|
     |  0  |         | 12 |   4    |   64    | 130 |         |  6  |
     |_____|         |____|________|_________|_____|         |_____|
```

In the first phase the bus is analyzed by transmitter reading 10 logical bits, if any logical high is detected, the channel is considered free and transmission phase starts in which the packet is entirely transmitted. Receiver calculates CRC and starts the response phase transmitting a single byte, `ACK` (dec 6) in case of correct reception or `NAK` (dec 21) if an error in the packet's content is detected. If transmitter receives no answer or NAK the packet sending has to be scheduled with a delay of `ATTEMPTS` * `ATTEMPTS` * `ATTEMPTS` with a maximum of 125 `ATTEMPTS` to obtain data transmission 3rd degree polynomial backoff.

In a shared medium it is necessary to define a bus id to isolate devices from outcoming communication of other buses nearby. Below is shown the same local transmission (with the obvious `0.0.0.0` or localhost bus id omitted) used as an example before, in a shared environment instead the packet's content is prepended with the bus id:
```cpp
Channel analysis  Transmission                                Response
    _____          _______________________________________     _____
   | C-A |        | ID | LENGTH | BUS ID  | CONTENT | CRC |   | ACK |
<--|-----|--------|----|--------|---------|---------|-----|> <|-----|
   |  0  |        | 12 |   8    | 0.0.0.1 |   64    | 130 |   |  6  |
   |_____|        |____|________|_________|_________|_____|   |_____|
```
Thanks to this rule it is not only possible to share a medium with neighbors, but also network with them and enhance connectivity for free.

###Bus network
A PJON bus network is the result of n PJON buses sharing the same medium and or interconnection of PJON buses using routers. A router is a device connected to n PJON buses with n dedicated pins on n dedicated media, able to route a packet from a bus to anotherone.

```cpp  
   TWO BUSES CONNECTED THROUGH A ROUTER

       BUS ID 0.0.0.1                  BUS ID 0.0.0.2
    _______     _______              _______     _______
   |       |   |       |            |       |   |       |
   | ID 0  |   | ID 1  |            | ID 0  |   | ID 1  |
   |_______|   |_______|  ________  |_______|   |_______|
 ______|___________|_____| ROUTER |_____|___________|______
          ___|___        |  ID 3  |        ___|___
         |       |       |________|       |       |
         | ID 2  |                        | ID 2  |
         |_______|                        |_______|
```

In a shared medium it is necessary to define a bus id to isolate devices from outcoming communication of other buses nearby, enabling many to coexist on the same communication medium.
```cpp  
   TWO BUSES SHARING THE SAME MEDIUM

       BUS ID 0.0.0.1                  BUS ID 0.0.0.2
    _______     _______              _______     _______
   |       |   |       |            |       |   |       |
   | ID 0  |   | ID 1  |            | ID 0  |   | ID 1  |
   |_______|   |_______|            |_______|   |_______|
 ______|___________|___________________|___________|______
          ___|___                          ___|___
         |       |                        |       |
         | ID 2  |                        | ID 2  |
         |_______|                        |_______|

```
