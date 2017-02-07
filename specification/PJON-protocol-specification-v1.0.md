- PJON Protocol specification:
[v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md) - [v0.2](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md) - [v0.3](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md) - **[v1.0](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md)**
- Acknowledge specification: [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Dynamic addressing specification: [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md)


```cpp
/*
Milan, Italy - 3/10/2016
The PJON™ protocol layer specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2016 All rights reserved

Related work: https://github.com/gioblu/PJON/
Compliant implementation versions: PJON 6.0 and following

New features:
- Header switched position with length to enable more than one byte length
- Optional extended header by Fred Larsen
- Header bits function definition
- Configurable 1 or 2 bytes length (max packet length 255/65535 bytes)
- Configurable CRC used (8/32 bit)
- Configurable auto-addressing
*/
```

###PJON™ Protocol specification v1.0
With this release, the PJON protocol layer has been vastly extended and generalized aiming to interoperability and to offer a real and complete alternative to the actual set of standards used for networking today. The strong plus of the approach used by the protocol mechanism is high efficiency and low overhead thanks to the configuration driven packet format, enabling easy constrain to application needs.

###Network protocol stack model
In the graph below is shown the protocol stack model proposed. The differences between the OSI model and the PJON stack is the presence or absence of some features and how they are distributed in its layers. Those engineering choices are made to offer a scalable and low overhead stack of protocols able to be used as a set or individually. This stack has been engineered "bottom to top" and was originally applied as an alternative to 1-Wire or i2c, with this release features has been extended enabling many use cases where the OSI model is generally applied.
```cpp  
 -----------------------------------------------
| 7 Application layer                           |
| APIs, data sharing, remote access             |
|-----------------------------------------------|
| 6 Presentation layer                          |
| Encryption, encoding, data compression        |
|-----------------------------------------------|
| 5 Session layer                               |
| Session                                       |
|-----------------------------------------------|
| 4 Network layer                               |
| Segmentation, routing                         |
|-----------------------------------------------|
| 3 Protocol layer                              |
| Addressing, asynchronous acknowledgement,     |
| multiplexing, traffic control                 |
|-----------------------------------------------|
| 2 Data link layer                             |
| Data link, transmission of packets,           |
| synchronous acknowledgment                    |
|-----------------------------------------------|
| 1 Physical layer                              |
| Cable, transceivers ecc                       |
|_______________________________________________|
```

###Basic concepts
* Every bus has a unique IPv4 like 4 bytes id
* Many buses can coexist on the same medium
* Every device has an equal right to transmit and receive
* Every device can be connected to n PJON buses
* Every device has a unique 1 byte id
* Every device can obtain an id if available (see [Dynamic addressing specification v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md))
* Transmission occurs only if the communication medium is not in use
* Devices communicate through packets with a maximum length of 255 or 65535 bytes
* Packet transmission is regulated by a 1, 2 or 3 byte header
* Synchronous and or asynchronous acknowledgement can be requested  (see [Acknowledge specification v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md))

The PJON protocol v1.0 handles internal bus connectivity and unique addressing for 254 devices, through bus communication with unique bus addressing for 4.294.967.295 buses and supports up to 1.090.921.692.930 devices. It regulates the exchange of packets with a configurable set of features driven by its header. Depending on the packet configuration a certain overhead is added to information varying from 3 up to 19 bytes.

###Bus
A PJON bus is made by a group of up to 254 devices transmitting and receiving on the same medium. Communication between devices occurs through packets and it is based on democracy: every device has the right to transmit on the common medium for up to `(1000 / devices number) milliseconds / second`.
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

###Bus network
A PJON bus network is the result of n PJON buses sharing the same medium and or being interconnected to other PJON buses through routers. A router is a device connected to n PJON buses with n dedicated, potentially different, data link layers on n dedicated media, able to route a packet from a bus to another. Thanks to this rule is not only possible to share the same medium with neighbours, but also network with them and enhance connectivity.
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
In a shared medium it is defined a IPv4 like bus id to isolate devices from outcoming communication of other buses nearby, enabling many to coexist on the same communication medium.
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

###Packet transmission
A packet transmission is the exchange of a string to one or many of the devices connected to the bus with optional correct reception certainty. The simplest form of packet is constructed by a recipient id, a header, the length, the content and its CRC. In this example is shown a packet transmission in a local bus to device id 12 containing the string @ (decimal 64):
```cpp  

 ID 12       HEADER 1    LENGTH 5     CONTENT 64  CRC 72
 __________  __________  ___________  __________  ____________
| Byte     || Byte     || Byte      || Byte     || Byte       |
|     __   ||      _   ||      _   _||  _       ||  _    _    |
|    |  |  ||     | |  ||     | | | || | |      || | |  | |   |
|0000|11|00||00000|1|00||00000|1|0|1||0|1|000000||0|1|00|1|000|
|____|__|__||_____|_|__||_____|_|_|_||_|_|______||_|_|__|_|___|
```
A default local packet transmission is a optionally bidirectional communication between two devices that can be divided in 3 different phases: **channel analysis**, **transmission** and optional **response**. The packet transmission procedure is regulated by its header:
```cpp
HEADER BITMASK

     1          2          3         4         5         6         7         8
 __________ __________ _________ _________ _________ __________ _________ _________
|          |          |         |         |         |          |         |         | byte 1
| EXTENDED | EXTENDED |   CRC   | ADDRESS | ACKMODE |   ACK    | TX INFO |   MODE  |
|  HEADER  |  LENGTH  |         |         |         |          |         |         |
|__________|__________|_________|_________|_________|__________|_________|_________|
|          |          |         |         |         |          |         |         | byte 2
| EXTENDED | ROUTING  | SEGMENT | SESSION | PARITY  | ENCODING |  DATA   |  ENCRY  | OPTIONAL
|  HEADER  |          |  ATION  |         |         |          |  COMP.  |  PTION  |
|__________|__________|_________|_________|_________|__________|_________|_________|
|          |          |         |         |         |          |         |         | byte 3
|   N/A    |   N/A    |   N/A   |   N/A   |   N/A   |   N/A    |   N/A   |   N/A   | OPTIONAL
|          |          |         |         |         |          |         |         |
|__________|__________|_________|_________|_________|__________|_________|_________|
```
Header byte 1 bits roles:
* **Extended header** bit informs if the header is composed by 1 (value 0) or 2 bytes (value 1)
* **Extended Length** bit informs the receiver if the packet contains 1 (value 0) or 2 (value 1) bytes length
* **CRC** bit informs the receiver which CRC was used as check for the packet, CRC 8 (value 0) or CRC 32 (value 1)
* **Address** bit informs if the packet is part of an addressing procedure (value 1) or not (value 0)
* **Acknowledge mode** bit informs if the requested acknowledge is synchronous (value 0) or asynchronous (value 1)
* **Acknowledge** bit informs if acknowledge is requested (value 1) or not (value 0)
* **Transmitter info** bit informs if the sender info are included (value 1) or not (value 0)
* **Mode** bit informs if the packet is a shared mode (value 1) or if local mode formatted (value 0)

Header byte 2 bits roles:
* **Extended header** bit informs if the header is composed by 2 (value 0) or 3 bytes (value 1)
* **Routing** bit informs if routing meta-data is included (value 1) or not (value 0)
* **Segmentation** bit informs it part of a segmented transmission (value 1) or not (value 0)
* **Session** bit informs if session meta-data is included (value 1) or not (value 0)
* **Parity** bit informs if parity information for auto-correction is included (value 1) or not (value 0)
* **Encoding** bit informs if encoding meta-data is included (value 1) or not (value 0)
* **Data compression** bit informs if data compression meta-data is included (value 1) or not (value 0)
* **Encryption** bit informs if encryption meta-data is included (value 1) or not (value 0)

```cpp  
Channel analysis   Transmission                                     Response
    _____           ________________________________________           _____
   | C-A |         | ID |  HEADER  | LENGTH | CONTENT | CRC |         | ACK |
<--|-----|---< >---|----|----------|--------|---------|-----|--> <----|-----|
   |  0  |         | 12 | 00000100 |   5    |    64   |  72 |         |  6  |
   |_____|         |____|__________|________|_________|_____|         |_____|
```
In the first phase the bus is analyzed by transmitter reading 10 logical bits, if any logical 1 is detected the channel is considered free and transmission phase starts in which the packet is entirely transmitted. Receiver calculates CRC and starts the response phase transmitting a single byte, `PJON_ACK` (decimal 6) in case of correct reception or `PJON_NAK` (decimal 21) if an error in the packet's content is detected. If transmitter receives no answer or `PJON_NAK` the packet sending is scheduled with a delay of `ATTEMPTS * ATTEMPTS * ATTEMPTS * ATTEMPTS` with a maximum of 42 `ATTEMPTS` to obtain data transmission 4rd degree polynomial backoff.

Below is shown the same local transmission used as an example before, formatted to be sent over a shared medium, where device id `12` of bus `0.0.0.1` sends @ (decimal 64) to device id `11` in bus id `0.0.0.1`. The packet's content is prepended with the bus id of the recipient, and optionally the sender's bus and device id:
```cpp  
Channel analysis                     Transmission                                 Response
 _____     ___________________________________________________________________     _____
| C-A |   | ID |  HEADER  | LENGTH |   BUS ID   | BUS ID | ID | CONTENT | CRC |   | ACK |
|-----|< >|----|----------|--------|------------|--------|----|---------|-----|> <|-----|
|  0  |   | 12 | 00000111 |   14   |    0001    |  0001  | 11 |   64    |     |   |  6  |
|_____|   |____|__________|________|____________|________|____|_________|_____|   |_____|
                                   |  RX INFO   |   TX INFO   |
```

Configuring the header it is possible to leverage of the extended features of the protocol:
```cpp  
Channel analysis                     Transmission                                             Response
 _____     ________________________________________________________________________________     _____
| C-A |   | ID |  HEADER  | LENGTH 1 | LENGTH 2 |  BUS ID  | BUS ID | ID | CONTENT | CRC32 |   | ACK |
|-----|< >|----|----------|----------|----------|----------|--------|----|---------|-------|> <|-----|
|  0  |   | 12 | 01100111 |  byte 1  |  byte 2  |   0001   |  0001  | 11 |         |1|2|3|4|   |  6  |
|_____|   |____|__________|__________|__________|__________|________|____|_________|_|_|_|_|   |_____|
                                                | RX INFO  |   TX INFO   |          
```
The graph above shows a packet transmission where the length is of 2 bytes supporting up to 65.535kB packet length. Receiver is able to parse the packet correctly reading the header, where `B01000000` up signals a 2 bytes length format and `B00100000` up signals CRC32 use.
