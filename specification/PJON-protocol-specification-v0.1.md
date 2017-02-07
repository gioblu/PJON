- PJON Protocol specification: **[v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md)** - [v0.2](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md) - [v0.3](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md) - [v1.0](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md)
- Acknowledge specification: [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Dynamic addressing specification: [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md)

```cpp
/*
Milan, Italy - 10/04/2010
The PJON™ protocol specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2017 All rights reserved

Related work: https://github.com/gioblu/PJON
Compliant implementation versions: PJON 1.0-3.0-beta
*/
```

###PJON™ Protocol specification v0.1
The first experimental specification of the PJON protocol has been drafted with the goal of offering a new open-source, multi-master communications bus system Standard. Its more common applications are in the field of internet of things and embedded systems. Extended tests proved its effectiveness on different media like electricity, radio frequency and light.

###Basic concepts
* Every device has an unique 1 byte ID (0-255)
* Every device transmits and receives on the same common medium
* Every device has an equal right to transmit and receive on the common medium
* Every device can be connected to n PJON network media (with n dedicated pins)
* Transmission occurs only if the communication medium is not in use
* Synchronization occurs every byte
* Devices communicate through packets

###Packet transmission
The concept of packet enables to send a communication payload to every connected device with correct reception certainty. A packet contains the recipient id, the length of the packet, its content and the CRC. Here is an example of a packet sending to device id 12 containing the string "@":
```cpp
 ID 12       LENGTH 4    CONTENT 64  CRC 130
 __________  __________  __________  ____________
| Byte     || Byte     || Byte     || Byte       |
|     __   ||      _   ||  _       ||  _      _  |
|    |  |  ||     | |  || | |      || | |    | | |
|0000|11|0 ||00000|1|00||0|1|000000||0|1|0000|1|0|
|____|__|__||_____|_|__||_|_|______||_|_|____|_|_|
```

A standard packet transmission is a bidirectional communication between two devices that can be divided in 3 different phases: channel **analysis**, **transmission** and **response**.
```cpp
  Channel analysis   Transmission                            Response
      _____           _____________________________           _____
     | C-A |         | ID | LENGTH | CONTENT | CRC |         | ACK |
  <--|-----|---------|----|--------|---------|-----|--> <----|-----|
     |  0  |         | 12 |   4    |   64    | 130 |         |  6  |
     |_____|         |____|________|_________|_____|         |_____|
```

In the first phase the bus is analyzed by transmitter reading 10 logical bits, if any logical high is detected, the channel is considered free and transmission phase starts in which the packet is entirely transmitted. Receiver calculates CRC and starts the response phase transmitting a single byte, `PJON_ACK` (dec 6) in case of correct reception or `PJON_NAK` (dec 21) if an error in the packet's content is detected. If transmitter receives no answer or `PJON_NAK` the packet sending has to be scheduled with a delay of `ATTEMPTS` * `ATTEMPTS` with a maximum of 250 `ATTEMPTS` to obtain data transmission quadratic backoff.

###Bus
A PJON Bus is made by a group of up to 255 devices transmitting and receiving on the same medium. Communication between devices occurs through packets and it is based on democracy: every device has the right to transmit on the common medium for up to (1000 / devices number) milliseconds / second.
```cpp
    _______     _______     _______     _______    _______
   |       |   |       |   |       |   |       |   |       |  
   | ID 0  |   | ID 1  |   | ID 2  |   | ID 3  |   | ID 4  |  
   |_______|   |_______|   |_______|   |_______|   |_______|    
 _____|___________|___________|___________|___________|_______
         ____|__     ____|__     ____|__     ____|__
        |       |   |       |   |       |   |       |   
        | ID 5  |   | ID 6  |   | ID 7  |   | ID 8  |
        |_______|   |_______|   |_______|   |_______|    
```
