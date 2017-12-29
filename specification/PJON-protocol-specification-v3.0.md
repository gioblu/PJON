#### Network layer
- PJON (Padded Jittering Operative Network) Protocol specification: **[v3.0](/specification/PJON-protocol-specification-v3.0.md)**
- Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- PJON Dynamic addressing specification: [v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- PJON known protocols: [list](/specification/PJON-known-protocols-list.md)
#### Data link layer
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v2.0](/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v2.0](/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- SFSP (Secure Frame Separation Protocol) specification: [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

```cpp
/*
Milan, Italy
Originally published: 10/04/2010
latest revision: 14/12/2017
PJON™ protocol layer specification v3.0
Invented by Giovanni Blu Mitolo,
header driven configuration proposed
by Fred Larsen, released into the public domain

Related work: https://github.com/gioblu/PJON/
Compliant implementations: PJON v10.0 and following

Changelog:
- Initial meta-data CRC8 added
- Forced CRC32 for packets of length > 15
- Expanded unacceptable header configuration list
- CRC8/32 polynomial specification added
*/
```

### PJON™ Protocol specification v3.0
The graph below shows the proposed protocol stack model. The features distribution have been engineered, and strongly tested, to offer an easier and more flexible set of standards for computer networks. Those engineering choices are made to offer a scalable and low overhead stack of protocols easily applied as a set or individually. PJON has been engineered "bottom to top" and was originally created to cover use cases where 1-Wire or i2c were generally applied, with its development, features have been extended enabling more complex use cases where more advanced network protocols like TCP-IP are generally applied.

An iterative experimentation process of 8 years led to the development of the following minimalistic set of rules covering most of the features provided by TCP-IP although requiring slightly half of its overhead (IPv4 40 bytes, PJON 22 bytes). Being designed to support features modularity, PJON can be used on extremely simple systems excluding unused features reducing its implementation's memory footprint and its overhead up to 5 bytes, finally providing the community with a common protocol able to cover a very wide range of use cases.  
```cpp  
 _______________________________________________
| 7 Application layer                           |
| APIs, data sharing, remote access             |
|_______________________________________________|
| 6 Presentation layer                          |
| Encryption, encoding, data compression        |
|_______________________________________________|
| 5 Session layer                               |
| Session                                       |
|_______________________________________________|
| 4 Transport layer: OSPREY                     |
| segmentation                                  |
|_______________________________________________|
| 3 Network layer: PJON                         |
| Addressing, error detection, reliable packet  |
| transmission, multiplexing, traffic control,  |
| asynchronous acknowledgement, dynamic         |
| protocol encapsulation                        |
|_______________________________________________|
| 2 Data link layer: PJDL/PJDLR/PJDLS/TSDL      |
| Frame separation, collision avoidance,        |
| simplex and half-duplex frame communication,  |
| synchronous response                          |
|_______________________________________________|
| 1 Physical layer: Wires, radio, light or      |
| Ethernet, WiFi, serial ecc. used by data link |
| as physical layer                             |
|_______________________________________________|
```

### Basic concepts
* Transmission occurs only if the communication medium is not in use
* Packet transmission is regulated by a 1, 2 or 3 byte header
* Devices communicate through packets with a maximum length of 255 or 65535 bytes
* Every device has an equal right to transmit and receive
* Every device has a unique 1 byte id
* Every device can obtain an id if available (see [Dynamic addressing specification v1.0](/specification/PJON-dynamic-addressing-specification-v2.0.md))
* Every bus has a unique 4 bytes id
* Every device can be connected to n PJON buses
* Many buses can coexist on the same medium
* Synchronous and or asynchronous acknowledgement can be requested (see [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md))
* Encapsulated protocol identification using a 2 bytes port id  

The PJON protocol v2.0 handles internal bus connectivity and unique addressing for 254 devices, through bus communication with unique bus addressing for 4.294.967.295 buses and supports up to 1.090.921.692.930 devices. It regulates the exchange of packets with a configurable set of features driven by its header. Depending on the packet configuration a certain overhead is added to information varying from 5 up to 22 bytes.

### Bus
A PJON bus is made by a group of up to 254 devices transmitting and receiving on the same medium. Communication between devices occurs through packets and it is based on democracy: every device has the right to transmit on the common medium for up to `(1000 / devices number) milliseconds / second`.
```cpp
 _______     _______     _______     _______
|       |   |       |   |       |   |       |
| ID 1  |   | ID 2  |   | ID 3  |   | ID 4  |
|_______|   |_______|   |_______|   |_______|
____|___________|___________|___________|___
      ___|___     ___|___     ___|___
     |       |   |       |   |       |
     | ID 5  |   | ID 6  |   | ID 7  |
     |_______|   |_______|   |_______|
```

### Bus network
A PJON bus network is the result of n PJON buses sharing the same medium and or being interconnected to other PJON buses through routers. In a shared medium scenario it is used a 4 bytes bus id to isolate devices from communication of other buses nearby, enabling many to coexist on the same communication medium.
```cpp  
TWO BUSES SHARING THE SAME MEDIUM

    BUS ID 0.0.0.1             BUS ID 0.0.0.2
 _______     _______         _______     _______
|       |   |       |       |       |   |       |
| ID 0  |   | ID 1  |       | ID 0  |   | ID 1  |
|_______|   |_______|       |_______|   |_______|
______|___________|______________|___________|______
       ___|___                     ___|___
      |       |                   |       |
      | ID 2  |                   | ID 2  |
      |_______|                   |_______|
```
### Router
A router is a device connected to n PJON buses with n dedicated, potentially different, data links on n dedicated media, able to route a packet from a bus to another. Thanks to this rule is not only possible to share the same medium with neighbours, but also network with them and enhance connectivity.
```cpp
TWO BUSES CONNECTED THROUGH A ROUTER

   BUS ID 0.0.0.1                  BUS ID 0.0.0.2
 _______     _______             _______     _______
|       |   |       |           |       |   |       |
| ID 0  |   | ID 1  |           | ID 0  |   | ID 1  |
|_______|   |_______|   ______  |_______|   |_______|
_____|___________|_____|ROUTER|_____|___________|____
       ___|___         | ID 3 |        ___|___
      |       |        |______|       |       |
      | ID 2  |                       | ID 2  |
      |_______|                       |_______|
```

### Header configuration
The header is a bit-mask or binary map of meta-data contained and configuration required.
```cpp
HEADER BITMASK
    1      2     3    4    5     6     7     8
 ______ ______ ____ _____ _____ _____ _____ _____
|PACKET|EXT.  |CRC |PORT |ACK  | ACK |TX   |MODE |
|ID    |LENGTH|    |     |MODE |     |INFO |     |
|______|______|____|_____|_____|_____|_____|_____|
```
#### Header byte 1
* `PACKET ID` or packet id bit informs if the packet contains (value 1) or not (value 0) a 2 bytes packet id
* `EXT. LENGTH` or extended length bit informs if the packet contains 1 (value 0) or 2 (value 1) bytes length
* `CRC` bit informs the receiver which CRC was used as check for the packet, CRC 8 (value 0) or CRC 32 (value 1)
* `PORT` or port bit informs if the packet contains (value 1) a 2 bytes port identifier or not (value 0)
* `ACK MODE` or acknowledge mode bit requests synchronous (value 0) or asynchronous (value 1) acknowledge mode
* `ACK` or acknowledge bit informs if acknowledge is requested (value 1) or not (value 0)
* `TX INFO` or transmitter info bit informs if the sender info are included (value 1) or not (value 0)
* `MODE` bit informs if the packet is a shared mode (value 1) or if local mode formatted (value 0)

#### Unacceptable header configuration states for standard transmission
* `----1-0-` or `ACK MODE` bit up, and `TX INFO` down (asynchronous acknowledgement needs transmitter info)
* `-10-----` or `EXT. LENGTH` bit up and `CRC` down (CRC32 forced for packet length > 15)
* `--01----` or `ADDR.` bit up and `CRC` bit down (CRC32 forced for addressing)
* `---1--0-` or `ADDR.` bit up and `TX INFO` bit down (transmitter info necessary for addressing)

#### Unacceptable header configuration states sending a BROADCAST
* `-----1--` or `ACK` bit up (no acknowledgement supported if BROADCAST)
* `----1---` or `ACK MODE` bit up (no acknowledgement supported if BROADCAST)

`-` symbol means irrelevant bit value

### Cyclic redundancy check
PJON supports both CRC8 and CRC32, to cover a wide range of use cases and packet lengths.

#### CRC8 polynomial
```cpp
0x97 = (x + 1)(x^7 + x^6 + x^5 + x^2 + 1)^2
```
`CRC8 C2`, implicit +1 notation, source Baicheva98, selected because it has the largest possible length (119 bit) at which `HD=4` can be achieved with 8-bit CRC. Other protocols specify the use of polynomials with much lower performance like `CRC-8 0xEA` or `DOWCRC 0x8C` used by 1-Wire.

#### CRC32 polynomial
```cpp
0x82608edb = x^32 + x^26 + x^23 + x^22 + x^16 +
             x^12 + x^11 + x^10 + x^8 + x^7 +
             x^5 + x^4 + x^2 + x + 1
```
`CRC32 IEEE 802.3` bit-reversed polynomial implicit +1 notation, or `0xedb88320`, selected for its high performance on a wide range of lengths, while also being widely evaluated and accepted as a good polynomial.

#### Initial meta-data CRC8
CRC8 is calculated and appended to initial meta-data to ensure consistency and avoid false positives. In case of length corruption because of an error, other protocols expose a major vulnerability, having the protocol layer looking for the CRC present at the end of the packet, at the wrong address, having higher chances of failing detecting errors. PJON having the first CRC8 present in a known, fixed position offers higher reliability than CAN (Controlled Area Network) and many other alternatives.

#### End CRC8/CRC32
CRC8 is calculated on both data and meta-data and appended at the end, for up to 15 bytes (including CRC) packets. CRC32 is automatically used if packet length exceeds 15 bytes but can be optionally used for shorter than 15 bytes packets if higher accuracy is required.

### Packet transmission
A default local packet transmission is an optionally bidirectional communication between two devices that can be divided in 3 different phases: **channel analysis**, **transmission** and optional **response**.
```cpp
Channel analysis            Transmission                Response
 _____  ________________________________________________  _____
| C-A || ID |  HEADER  | LENGTH | CRC8 | CONTENT | CRC8 || ACK |
|-----||----|----------|--------|------|---------|------||-----|
|  0  || 12 | 00000100 |   6    |      |    64   |      ||  6  |
|_____||____|__________|________|______|_________|______||_____|
```
In the channel analysis phase transmitter assess the medium's state before starting transmission to avoid collision. If the medium is considered free, transmission phase starts where the packet is entirely transmitted. The receiving device calculates CRC and starts the response phase transmitting a single byte, `PJON_ACK` (decimal 6) in case of correct data reception. If no acknowledgement is received, after an exponential back-off delay the transmitter device retries until acknowledgement is received or a maximum number of attempts is reached and packet transmission discarded.     

Below is shown the same local transmission used as an example before, formatted to be sent over a shared medium, where device id `12` of bus `0.0.0.1` sends @ (decimal 64) to device id `11` in bus id `0.0.0.1`. The packet's content is prepended with the bus id of the recipient, and optionally the sender's bus and device id:
```cpp
Channel analysis              Transmission           Response
 __  __________________________________________________  ___
|CA||ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|DATA|CRC8||ACK|
|--||--|--------|------|----|------|------|--|----|----||---|
|00||12|00000111|  15  |    | 0001 | 0001 |11| 64 |    || 6 |
|__||__|________|______|____|______|______|__|____|____||___|
                            |RXINFO| TX INFO |
```

Configuring the header it is possible to leverage of the extended features of the protocol:

```cpp
 ________________________________________________________
|ID| HEADER |LEN 1|LEN 2|CRC8|BUS ID|BUS ID|ID|DATA|CRC32|
|--|--------|-----|-----|----|------|------|--|----|-----|
|12|01100111|  0  | 19  |    | 0001 | 0001 |11|    |     |
|__|________|_____|_____|____|______|______|__|____|_____|
                             |RXINFO| TX INFO |
```
The graph above shows a packet transmission where the length is of 2 bytes supporting up to 65535 bytes packet length. Receiver is able to parse the packet correctly reading the header, where `B01000000` up signals a 2 bytes length format and `B00100000` up signals CRC32 use.

```cpp
 _____________________________________________________________
|ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|PACKET ID|DATA|CRC32|
|--|--------|------|----|------|------|--|---------|----|-----|
|12|10001111|  20  |    | 0002 | 0001 |11|   999   | 64 |     |
|__|________|______|____|______|______|__|_________|____|_____|
                        |RXINFO| TX INFO |            
```
The graph above shows a packet transmission where the [recursive acknowledgement ](/specification/PJON-protocol-acknowledge-specification-v1.0.md#pjon-recursive-acknowledgement-pattern) pattern is applied: device `11` sends to device `12` of bus `0.0.0.2` a packet with header `ACK MODE` bit up requesting an asynchronous acknowledgement response, and so identifying the packet with the unique id `999` and `ACK` bit up requesting a synchronous acknowledgement response. `12` receives the packet and replies with a synchronous acknowledgement, or sending `PJON_ACK` (decimal 6), subsequently `12` sends also an asynchronous acknowledgement, that is instead an entire packet, back to device `11` containing only packet id `999` and the necessary configuration, that will be also synchronously acknowledged by device `11`:
```cpp
 ________________________________________________________
|ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|PACKET ID|CRC32|
|--|--------|------|----|------|------|--|---------|-----|
|11|10001111|  19  |    | 0001 | 0002 |12|   999   |     |
|__|________|______|____|______|______|__|_________|_____|
                        |RXINFO| TX INFO |       
```
See the [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md) to have more detailed info of its procedure.

### Protocol encapsulation
PJON supports higher level protocols transport and identification by using a 2 bytes unsigned integer port number as a protocol identifier. Thanks to this feature devices can handle safely many different protocols at the same time. Ports from `0` to `8000` are reserved to known protocols which index is present in the [known protocols list](/specification/PJON-known-protocols-list.md), ports from `8001` to `65535` are free for custom use cases.

The graph below shows a packet transmission where port 8002 is inserted in the packet and header bit `B00010000` is set up to signal its presence.
```cpp
 _________________________________________
|ID| HEADER |LENGTH|CRC8|PORT ID|DATA|CRC8|
|--|--------|------|----|-------|----|----|
|12|10001111|  8   |    | 8002  | 64 |    |
|__|________|______|____|_______|____|____|

```
