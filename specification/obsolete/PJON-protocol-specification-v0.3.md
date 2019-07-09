
```cpp
/*
Milan, Italy - 04/09/2016
The PJON® protocol specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2019 All rights reserved

Related work: https://github.com/gioblu/PJON/
Compliant implementation versions: PJON 4.0-5.0

New feature: Header driven dynamical configuration proposed and implemented by Fred Larsen
*/
```

### PJON® Protocol specification v0.3
A third draft of the PJON protocol has been released with the addition of a 1 byte header designed to contain the configuration for the packet's communication procedure and its meta-data. In versions 0.1 and 0.2, the PJON communication procedure configuration was static and the header not present, for this reason two differently configured devices, could not establish nominal communication, without configuration pairing. It is now possible for the packet's receiver to read the header, react as transmitter requested and parse the packet and its info correctly.


### Network protocol stack model
The actual PJON network protocol stack model lacks **segmentation** and **routing** procedures, dedicated to higher level layers to be added during development.
```cpp  
 ___________________________________________
| PJON Protocol layer                       |
| Addressing, acknowledgement               |
| multiplexing, traffic control             |
|-------------------------------------------|
| STRATEGY                                  |
| Data link, transmission of data frames    |
|-------------------------------------------|
| Physical layer                            |
| Cable, transceivers ecc                   |
|___________________________________________|
```

### Basic concepts
* Every bus has a unique IPv4 like 4 bytes id
* Many buses can coexist on the same medium
* Every device has a unique 1 byte id
* Every device has an equal right to transmit and receive on the common medium
* Every device can be connected to n PJON buses
* Transmission occurs only if the communication medium is not in use
* Devices communicate through packets
* Packet transmission is regulated by a 1 byte header

The PJON protocol v0.3 handles internal bus connectivity and unique addressing for 254 devices, through bus communication with unique bus addressing for 4.294.967.295 buses, supporting up to 1.090.921.692.930 devices.

### Bus
A PJON bus is made by a group of up to 255 devices transmitting and receiving on the same medium. Communication between devices occurs through packets and it is based on democracy: every device has the right to transmit on the common medium for up to `(1000 / devices number) milliseconds / second`.
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

### Packet transmission
A packet transmission is the exchange of a string to one of the devices connected to the bus with optional correct reception certainty. A packet contains the recipient id, its length, its header, its content and the CRC. In this example is shown a packet transmission in a local bus to device id 12 containing the string @ (decimal 64):
```cpp  

 ID 12       LENGTH 5     HEADER 1    CONTENT 64  CRC 72
 __________  ___________  __________  __________  ____________
| Byte     || Byte      || Byte     || Byte     || Byte       |
|     __   ||      _   _||      _   ||  _       ||  _    _    |
|    |  |  ||     | | | ||     | |  || | |      || | |  | |   |
|0000|11|00||00000|1|0|1||00000|1|00||0|1|000000||0|1|00|1|000|
|____|__|__||_____|_|_|_||_____|_|__||_|_|______||_|_|__|_|___|
```
A default local packet transmission is a bidirectional communication between two devices that can be divided in 3 different phases: **channel analysis**, **transmission** and **response**. The packet transmission procedure is regulated by its header.
```cpp  
Channel analysis  Transmission                                  Response
    _____          ________________________________________        _____
   | C-A |        | ID | LENGTH |  HEADER  | CONTENT | CRC |      | ACK |
<--|-----|---< >--|----|--------|----------|---------|-----|-> <--|-----|
   |  0  |        | 12 |   5    | 00000100 |    64   |  72 |      |  6  |
   |_____|        |____|________|__________|_________|_____|      |_____|
```
In the first phase the bus is analyzed by transmitter reading 10 logical bits, if any logical 1 is detected the channel is considered free, transmission phase starts in which the packet is entirely transmitted. Receiver calculates CRC and starts the response phase transmitting a single byte, `PJON_ACK` (decimal 6) in case of correct reception or `PJON_NAK` (decimal 21) if an error in the packet's content is detected. If transmitter receives no answer or `PJON_NAK` the packet sending is scheduled with a delay of `ATTEMPTS * ATTEMPTS * ATTEMPTS` with a maximum of 125 `ATTEMPTS` to obtain data transmission 3rd degree polynomial back-off.

Below is shown the same local transmission used as an example before, formatted to be sent in a shared environment, where device id `12` of bus `0.0.0.1` sends @ (decimal 64) to device id `11` in bus id `0.0.0.1`. The packet's content is prepended with the bus id of the recipient, and optionally the sender's bus and device id:
```cpp  
Channel analysis           Transmission                   Response
 ___     _____________________________________________     ___
|C-A|   |ID|LENGTH|HEADER|BUS ID|BUS ID|ID|CONTENT|CRC|   |ACK|
|---|< >|--|------|------|------|------|--|-------|---|> <|---|
| 0 |   |12|  14  | 111  | 0001 | 0001 |11|  64   |   |   | 6 |
|___|   |__|______|______|______|______|__|_______|___|   |___|
                         |RXINFO| TX INFO |
```

### Header configuration
The header bitmask let the packet's receiver handle the exchange as transmitter requested.
```cpp
 _______________________________________________________________________
|00000110|Acknowledge requested    |Sender info included    |Local bus |DEFAULT
|00000100|Acknowledge requested    |Sender info not included|Local bus |
|00000010|Acknowledge not requested|Sender info included    |Local bus |
|00000000|Acknowledge not requested|Sender info not included|Local bus |
|--------|-------------------------|------------------------|----------|
|00000111|Acknowledge requested    |Sender info included    |Shared bus|
|00000101|Acknowledge requested    |Sender info not included|Shared bus|
|00000011|Acknowledge not requested|Sender info included    |Shared bus|
|00000001|Acknowledge not requested|Sender info not included|Shared bus|
|________|_________________________|________________________|__________|
```
As you can see for now, only the uppermost bit states are used for packet transmission exchange configuration, the unused bits may be used in future to extend or optimize the PJON Standard, so it is suggested not make use of them on application level.


### Bus network
A PJON bus network is the result of n PJON buses sharing the same medium and or interconnection of PJON buses using routers. A router is a device connected to n PJON buses with n dedicated pins on n dedicated media, able to route a packet from a bus to anotherone. Thanks to this rule is not only possible to share a medium with neighbours, but also network with them and enhance connectivity for free.
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
