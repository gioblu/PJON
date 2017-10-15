- PJON (Padded Jittering Operative Network) Protocol specification: **[v2.0](/specification/PJON-protocol-specification-v2.0.md)**
- Acknowledge specification: [v0.1](/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Dynamic addressing specification: [v1.0](/specification/PJON-dynamic-addressing-specification-v1.0.md)
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/strategies/OverSampling/specification/PJDLR-specification-v2.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v1.0](/strategies/ThroughSerial/specification/TSDL-specification-v1.0.md)
```cpp
/*
Milan, Italy - 28/03/2017
PJON™ protocol layer specification v2.0
Invented by Giovanni Blu Mitolo. Header driven configuration proposed
by Fred Larsen, released into the public domain

Related work: https://github.com/gioblu/PJON/
Compliant implementation versions: PJON 9.0 and following

Changelog:
- Initial meta-data CRC8 added
- Forced CRC32 for packets of length > 15
- Expanded unacceptable header configuration list
- CRC8/32 polynomial specification added
*/
```

### PJON™ Protocol specification v2.0
The PJON protocol layer has been vastly extended and generalized aiming to interoperability and to offer an alternative standards used for networking today. The strong plus of the approach used by the protocol is its high flexibility, low overhead thanks to the header driven packet format and strong reliability thanks to the application of a double redundancy check.

### Network protocol stack model
In the graph below it is shown the protocol stack model proposed; only layer 2 and 3 are specified by this document. The features distribution has been engineered, and strongly tested, to offer an easier and more flexible set of standards for computer networks. Those engineering choices are made to offer a scalable and low overhead stack of protocols easily applied as a set or individually. PJON has been engineered "bottom to top" and was originally applied as an alternative to 1-Wire or i2c, with its development, features has been extended enabling many use cases where more advanced network protocol stacks are generally applied.
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
| 4 Transport layer                             |
| Routing, segmentation                         |
|_______________________________________________|
| 3 Network layer: PJON                         |
| Addressing, error detection, reliable packet  |
| transmission, multiplexing, traffic control,  |
| asynchronous acknowledgement                  |
|_______________________________________________|
| 2 Data link layer: PJDL/PJDLR                 |
| Collision avoidance, simplex and half-duplex  |
| communication, framing, synchronous           |
| acknowledgement                               |
|_______________________________________________|
|                                               |
| 1 Physical layer: Cables, transceivers ecc.   |
|_______________________________________________|
```

### Basic concepts
* Transmission occurs only if the communication medium is not in use
* Packet transmission is regulated by a 1, 2 or 3 byte header
* Devices communicate through packets with a maximum length of 255 or 65535 bytes
* Every device has an equal right to transmit and receive
* Every device has a unique 1 byte id
* Every device can obtain an id if available (see [Dynamic addressing specification v1.0](/specification/PJON-dynamic-addressing-specification-v1.0.md))
* Every bus has a unique 4 bytes id
* Every device can be connected to n PJON buses
* Many buses can coexist on the same medium
* Synchronous and or asynchronous acknowledgement can be requested (see [Acknowledge specification v0.1](/specification/PJON-protocol-acknowledge-specification-v0.1.md))

The PJON protocol v2.0 handles internal bus connectivity and unique addressing for 254 devices, through bus communication with unique bus addressing for 4.294.967.295 buses and supports up to 1.090.921.692.930 devices. It regulates the exchange of packets with a configurable set of features driven by its header. Depending on the packet configuration a certain overhead is added to information varying from 5 up to 19 bytes.

### Bus
A PJON bus is made by a group of up to 254 devices transmitting and receiving on the same medium. Communication between devices occurs through packets and it is based on democracy: every device has the right to transmit on the common medium for up to `(1000 / devices number) milliseconds / second`.
```cpp
 _______     _______     _______     _______     _______
|       |   |       |   |       |   |       |   |       |
| ID 0  |   | ID 1  |   | ID 2  |   | ID 3  |   | ID 4  |
|_______|   |_______|   |_______|   |_______|   |_______|
____|___________|___________|___________|___________|___
       ___|___     ___|___     ___|___     ___|___
      |       |   |       |   |       |   |       |
      | ID 5  |   | ID 6  |   | ID 7  |   | ID 8  |
      |_______|   |_______|   |_______|   |_______|
```

### Bus network
A PJON bus network is the result of n PJON buses sharing the same medium and or being interconnected to other PJON buses through routers. In a shared medium scenario it is used a 4 bytes bus id to isolate devices from communication of other buses nearby, enabling many to coexist on the same communication medium.
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
### Router
A router is a device connected to n PJON buses with n dedicated, potentially different, data links on n dedicated media, able to route a packet from a bus to another. Thanks to this rule is not only possible to share the same medium with neighbours, but also network with them and enhance connectivity.
```cpp
TWO BUSES CONNECTED THROUGH A ROUTER

   BUS ID 0.0.0.1                   BUS ID 0.0.0.2
 _______     _______               _______     _______
|       |   |       |             |       |   |       |
| ID 0  |   | ID 1  |             | ID 0  |   | ID 1  |
|_______|   |_______|   ________  |_______|   |_______|
_____|___________|_____| ROUTER |_____|___________|____
       ___|___         |  ID 3  |        ___|___
      |       |        |________|       |       |
      | ID 2  |                         | ID 2  |
      |_______|                         |_______|
```

### Header configuration
The header is a bit-mask or binary map of meta-data contained and configuration required.
```cpp
HEADER BITMASK
    1      2     3     4     5     6     7     8
 ______ ______ _____ _____ _____ _____ _____ _____
|EXT.  |EXT.  |CRC  |ADDR.|ACK  |ACK  |TX   |MODE |
|HEADER|LENGTH|     |     |MODE |     |INFO |     |
|______|______|_____|_____|_____|_____|_____|_____|
|EXT.  |ROUT. |SEGM.|SESS.|PAR. |ENCO |DATA |ENCRY|
|HEADER|      |     |     |     |DING |COMP.|PTION|
|______|______|_____|_____|_____|_____|_____|_____|
|      |      |     |     |     |     |     |     |
| N/A  | N/A  | N/A | N/A | N/A | N/A | N/A | N/A |
|______|______|_____|_____|_____|_____|_____|_____|
```
#### Header byte 1
* `EXT. HEADER` or extended header bit informs if the header is composed by 1 (value 0) or 2 bytes (value 1)
* `EXT. LENGTH` or extended length bit informs if the packet contains 1 (value 0) or 2 (value 1) bytes length
* `CRC` bit informs the receiver which CRC was used as check for the packet, CRC 8 (value 0) or CRC 32 (value 1)
* `ADDR.` or addressing bit informs if the packet is part of an addressing procedure (value 1) or not (value 0)
* `ACK MODE` or acknowledge mode bit requests synchronous (value 0) or asynchronous (value 1) acknowledge mode
* `ACK` or acknowledge bit informs if acknowledge is requested (value 1) or not (value 0)
* `TX INFO` or transmitter info bit informs if the sender info are included (value 1) or not (value 0)
* `MODE` bit informs if the packet is a shared mode (value 1) or if local mode formatted (value 0)

#### Header byte 2
* `EXT. HEADER` bit informs if the header is composed by 2 (value 0) or 3 bytes (value 1)
* `ROUT.` or routing bit informs if routing meta-data is included (value 1) or not (value 0)
* `SEGM.` or segmentation bit informs if part of a segmented transmission (value 1) or not (value 0)
* `SESS.` or session bit informs if session meta-data is included (value 1) or not (value 0)
* `PAR.` or parity bit informs if parity information for auto-correction is included (value 1) or not (value 0)
* `ENCODING` bit informs if encoding meta-data is included (value 1) or not (value 0)
* `DATA COMP.` or data compression bit informs if data compression meta-data is included (value 1) or not (value 0)
* `ENCRYPTION` bit informs if encryption meta-data is included (value 1) or not (value 0)

#### Unacceptable header configuration states for standard transmission
* `----1-0-` or `ACK MODE` bit up, and `TX INFO` down (asynchronous acknowledgement needs transmitter info)
* `-10-----` or `EXT. LENGTH` bit up and `CRC` down (CRC32 forced for packet length > 15)
* `--01----` or `ADDR.` bit up and `CRC` bit down (CRC32 forced for addressing)
* `---1--0-` or `ADDR.` bit up and `TX INFO` bit down (transmitter info necessary for addressing)

#### Unacceptable header configuration states sending a BROADCAST
* `-----1--` or `ACK` bit up (no acknowledgement supported if BROADCAST)
* `----1---` or `ACK MODE` bit up (no acknowledgement supported if BROADCAST)
* `-10-----` or `EXT. LENGTH` bit up and `CRC` down (CRC32 forced for packet length > 15)
* `--01----` or `ADDR.` bit up and `CRC` bit down (CRC32 forced for addressing)
* `---1--0-` or `ADDR.` bit up and `TX INFO` bit down (transmitter info necessary for addressing)

`-` symbol means irrelevant bit value

### Cyclic redundancy check
PJON supports both CRC8 and CRC32, to cover a wide range of use cases and packet lengths.

#### CRC8 polynomial
```cpp
0x97 = (x + 1)(x^7 + x^6 + x^5 + x^2 + 1)^2
```
`CRC8 C2`, implicit +1 representation, source Baicheva98, selected because it has the largest possible length (119 bit) at which `HD=4` can be achieved with 8-bit CRC. Other protocols specify the use of polynomials with much lower performance like `CRC-8 0xEA` or `DOWCRC 0x8C` used by 1-Wire.

#### CRC32 polynomial
```cpp
0x82608edb = x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
```
`IEEE 802.3; CRC-32` bit-reversed polynomial implicit +1 notation, or `0xedb88320`, selected for its high performance on a wide range of lengths, while also being widely evaluated and accepted as a good polynomial.

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
|  0  || 12 | 00000100 |   5    |      |    64   |      ||  6  |
|_____||____|__________|________|______|_________|______||_____|
```
In the channel analysis phase transmitter assess if the medium's state before starting transmission to avoid collision. If the medium is considered free, transmission phase starts where the packet is entirely transmitted. The receiving device calculates CRC and starts the response phase transmitting a single byte, `PJON_ACK` (decimal 6) in case of correct data reception. If no acknowledgement is received, after an exponential back-off delay the transmitter device retries until acknowledgement is received or a maximum number of attempts is reached and packet transmission discarded.     

Below is shown the same local transmission used as an example before, formatted to be sent over a shared medium, where device id `12` of bus `0.0.0.1` sends @ (decimal 64) to device id `11` in bus id `0.0.0.1`. The packet's content is prepended with the bus id of the recipient, and optionally the sender's bus and device id:
```cpp
Channel analysis              Transmission               Response
 ___  _____________________________________________________  ___
|C-A||ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|CONTENT|CRC8||ACK|
|---||--|--------|------|----|------|------|--|-------|----||---|
| 0 ||12|00000111|  14  |    | 0001 | 0001 |11|  64   |    || 6 |
|___||__|________|______|____|______|______|__|_______|____||___|
                             |RXINFO| TX INFO |
```

Configuring the header it is possible to leverage of the extended features of the protocol:

```cpp
Channel analysis                Transmission                      Response
 ___  ______________________________________________________________  ___
|C-A||ID| HEADER |LENGTH 1|LENGTH 2|CRC8|BUS ID|BUS ID|ID|CONT|CRC32||ACK|
|---||--|--------|--------|--------|----|------|------|--|----|-----||---|
| 0 ||12|01100111| byte 1 | byte 2 |    | 0001 | 0001 |11|    |     || 6 |
|___||__|________|________|________|____|______|______|__|____|_____||___|
                                        |RXINFO| TX INFO |
```
The graph above shows a packet transmission where the length is of 2 bytes supporting up to 65535 bytes packet length. Receiver is able to parse the packet correctly reading the header, where `B01000000` up signals a 2 bytes length format and `B00100000` up signals CRC32 use.

```cpp
Channel analysis               Transmission                     Response
 ___  ____________________________________________________________  ___
|C-A||ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|PACKET ID|CONT|CRC8||ACK|
|---||--|--------|------|----|------|------|--|---------|----|----||---|
| 0 ||12|00001111|  16  |    | 0002 | 0001 |11|   999   | 64 |    || 6 |
|___||__|________|______|____|______|______|__|_________|____|____||___|
                             |RXINFO| TX INFO |       
```
The graph above shows a packet transmission where the [recursive acknowledgement ](/specification/PJON-protocol-acknowledge-specification-v0.1.md#pjon-recursive-acknowledgement-pattern) pattern is applied: device `11` sends to device `12` of bus `0.0.0.2` a packet with header `ACK MODE` bit up requesting an asynchronous acknowledgement response, and so identifying the packet with the unique id `999` and `ACK` bit up requesting a synchronous acknowledgement response. `12` receives the packet and replies with a synchronous acknowledgement, or sending `PJON_ACK` (decimal 6), subsequently `12` sends also an asynchronous acknowledgement, that is instead an entire packet, back to device `11` containing only packet id `999` and the necessary configuration, that will be also synchronously acknowledged by device `11`:
```cpp
Channel analysis               Transmission                 Response
 ___  ________________________________________________________  ___
|C-A||ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|PACKET ID| CRC ||ACK|
|---||--|--------|------|----|------|------|--|---------|-----||---|
| 0 ||11|00001111|  15  |    | 0001 | 0002 |12|   999   |     || 6 |
|___||__|________|______|____|______|______|__|_________|_____||___|
                             |RXINFO| TX INFO |        
```
See the [Acknowledge specification v0.1](/specification/PJON-protocol-acknowledge-specification-v0.1.md) to have more detailed info of its procedure.
