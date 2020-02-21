### Specifications index

#### Network layer
- **[PJON (Padded Jittering Operative Network) v3.1](/specification/PJON-protocol-specification-v3.1.md)**
- [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v4.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.0.md)
- [PJDLR (Padded Jittering Data Link over Radio) v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md)
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJON® v3.1
```
Invented by Giovanni Blu Mitolo
Header feature proposed by Fred Larsen
Originally published: 10/04/2010
Latest revision: 19/02/2020
Related work: https://github.com/gioblu/PJON/
Compliant implementations: PJON v10.0 and following
Released into the public domain
```
The PJON protocol v3.1 in local mode supports connectivity for up to 254 devices, in shared mode supports connectivity for up to 4.294.967.295 buses (groups of devices) and up to 1.090.921.692.930 devices. The packet format is dynamic therefore meta-data can be optionally included using the header as a bitmap of selected features. It supports interoperability between systems that use a different configuration and provides with high efficiency including only the protocol's features used and the overhead effectively required (5-22 bytes). PJON can be used for simple low-data-rate applications as an alternative to 1-Wire, i2c or CAN but can also be applied in place of IP to interconnect more complex networks.   

The graph below shows the conceptual model that characterizes and standardizes the communication. Its goal is the interoperability of diverse systems on a wide range of media with the use of a new set of Open Standards. The graph partitions represent abstraction layers.

```
 ________________________________________________
| 3 Network layer: PJON                          |
|- Optional features ----------------------------|
| Routing and switching                          |
| Asynchronous acknowledgement                   |
| Service identification (16 bits)               |
| Packet identification (16 bits)                |
| Bus identification (32 bits)                   |
| Sender identification                          |
| Packet transmission, maximum length 65535B     |
|- Core features --------------------------------|
| Congestion control                             |
| Packet transmission, maximum length 255B       |
| Error detection                                |
| Device identification (8 bits)                 |
| Broadcast                                      |
|________________________________________________|
| 2 Data link layer: PJDL, PJDLR, PJDLS, TSDL    |
| Synchronous acknowledgement                    |
| Frame transmission                             |
| Medium access control                          |
|________________________________________________|
| 1 Physical layer:                              |
| Electric, radio or light impulses              |
|________________________________________________|
```

### Basic concepts
* Packet transmission is regulated by a 8 bits header
* Devices communicate through packets with a maximum length of 255 or 65535 bytes
* Devices are identified by a unique 8 bits device id
* Buses are identified with a 32 bits bus id
* Many buses can coexist on the same medium
* Synchronous and or asynchronous acknowledgement can be requested (see [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md))
* Network services are identified with a 16 bits port id  


### Bus
A PJON bus is made by a group of up to 254 devices transmitting and receiving on the same medium. Communication between devices occurs through packets and it is based on fairness or on the right of each device to equally share the bandwidth available.
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
A PJON bus network is the result of n buses sharing the same medium and or being interconnected with other buses through routers. On a shared medium it is required the use a 32 bits bus id to isolate a group of devices from communication of other buses nearby, enabling many to coexist and network on the same communication medium.
```cpp  
TWO BUSES SHARING THE SAME MEDIUM
1 collision domain

    BUS ID 0.0.0.1            BUS ID 0.0.0.2
 _______     _______        _______     _______
|       |   |       |      |       |   |       |
| ID 1  |   | ID 2  |      | ID 1  |   | ID 2  |
|_______|   |_______|      |_______|   |_______|
______|___________|_____________|___________|___
       ___|___                    ___|___
      |       |                  |       |
      | ID 3  |                  | ID 3  |
      |_______|                  |_______|
```

### Switch
A Switch is a device that forwards packets transparently between directly connected buses also if different physical layers or data-links are in use. It can rely on a default gateway to operate as a leaf in a larger tree network.
```cpp
 ______            ________             ______
|      | PJDL bus |        | PJDLR bus |      |
| ID 1 |__________| SWITCH |___________| ID 2 |
|______|          |________|           |______|
```

### Router
A router is a device connected to n PJON devices or buses on n dedicated media, able to route packets from a device, a bus or a medium to another. The router can operate in a network with a tree topology and no loops forwarding packets transparently. Packets can be routed between indirectly connected buses if a routing table or a default gateway is used.
```cpp
TWO BUSES CONNECTED THROUGH A ROUTER
2 collision domains

   BUS ID 0.0.0.1                 BUS ID 0.0.0.2
 _______     _______            _______     _______
|       |   |       |          |       |   |       |
| ID 1  |   | ID 2  |          | ID 1  |   | ID 2  |
|_______|   |_______|  ______  |_______|   |_______|
_____|___________|____|ROUTER|_____|___________|____
       ___|___        | ID 3 |        ___|___
      |       |       |______|       |       |
      | ID 3  |                      | ID 3  |
      |_______|                      |_______|
```

### Header configuration
The header is a bitmap of the meta-data contained and the configuration required. Unlike other protocols, PJON has a dynamic packet format designed to include in each packet only what is strictly required to carry out the exchange. Depending on the bitmap configuration a variable overhead (5-22 bytes) is added to information.

```cpp
HEADER BITMAP
    8      7     6    5     4     3     2      1
 ______ ______ ____ _____ _____ _____ _____ _____
|PACKET|EXT.  |CRC |PORT |ACK  | ACK |TX   |MODE |
|ID    |LENGTH|    |     |MODE |     |INFO |     |
|______|______|____|_____|_____|_____|_____|_____|
```
1. `MODE` bit informs if the packet is formatted in [shared](/specification/PJON-protocol-specification-v3.1.md#shared-mode) (value 1) or [local](/specification/PJON-protocol-specification-v3.1.md#local-mode) mode (value 0)  
2. `TX INFO` bit informs if the sender info are included (value 1) or not (value 0)
3. `ACK` bit informs if the [synchronous acknowledgement](/specification/PJON-protocol-acknowledge-specification-v1.0.md#synchronous-acknowledge) is requested (value 1) or not (value 0)
4. `ACK MODE` bit informs if the [asynchronous acknowledgement](/specification/PJON-protocol-acknowledge-specification-v1.0.md#asynchronous-acknowledge) is requested (value 1) or not (value 0)
5. `PORT` bit informs if a 16 bits [network service identifier](/specification/PJON-protocol-specification-v3.1.md#network-services) is contained (value 1) or not (value 0)
6. `CRC` bit signals which CRC is used, [CRC8](/specification/PJON-protocol-specification-v3.1.md#crc8-polynomial) (value 0) or [CRC32](/specification/PJON-protocol-specification-v3.1.md#crc32-polynomial) (value 1)
7. `EXT. LENGTH` bit informs if the packet contains 8 (value 0) or 16 bits (value 1) [length](/specification/PJON-protocol-specification-v3.1.md#extended-length)
8. `PACKET ID` bit informs if the packet contains (value 1) or not (value 0) a 16 bits [packet id](/specification/PJON-protocol-specification-v3.1.md#packet-identification)

Unacceptable header configuration states for standard transmission:
* `----1-0-` or `ACK MODE` bit high, and `TX INFO` bit low (requires transmitter info)
* `-10-----` or `EXT. LENGTH` bit high and `CRC` bit low (forced CRC32 for length > 15)

Unacceptable header configuration states for a broadcast transmission:
* `-----1--` or `ACK` bit high (acknowledgement not supported if broadcasting)
* `----1---` or `ACK MODE` bit high (acknowledgement not supported if broadcasting)

`-` symbol means irrelevant bit value

### Cyclic redundancy check
PJON supports both CRC8 and CRC32 to ensure safety on a wide range of use cases and packet lengths.

#### CRC8 polynomial
```cpp
0x97 = (x + 1)(x^7 + x^6 + x^5 + x^2 + 1)^2
```
`CRC8 C2`, discovered by Tsonka Baicheva, is used, in implicit +1 notation, because it has the longest possible length (119 bit) at which `HD=4` can be achieved with an 8-bit CRC. Other protocols specify the use of polynomials with much lower overall performance like `CRC-8 0xEA` or `DOWCRC 0x8C` used by 1-Wire.

#### CRC32 polynomial
```cpp
0x82608edb = x^32 + x^26 + x^23 + x^22 + x^16 +
             x^12 + x^11 + x^10 + x^8 + x^7 +
             x^5 + x^4 + x^2 + x + 1
```
`CRC32 IEEE 802.3` bit-reversed polynomial implicit +1 notation, or `0xedb88320`, selected for its high performance on a wide range of lengths, while also being widely evaluated and accepted as a good polynomial.

#### Initial meta-data CRC8
CRC8 is calculated and appended to the initial meta-data (id, header and length) to ensure consistency, avoid false positives and the length corruption vulnerability that affects CAN (Controlled Area Network) and many other protocols.

#### End CRC8/CRC32
CRC8 is calculated on both data and meta-data and it is appended at the end of packets of up to 15 bytes length (including overhead). CRC32 must be used if packet length exceeds 15 bytes but can be optionally used also for shorter packets if higher accuracy is required.

### Packet transmission
A local packet transmission is an optionally bidirectional communication between two devices that can be divided in 3 phases: **channel analysis**, **transmission** and optional **response**. In the channel analysis phase the medium's state is assessed before starting transmission to avoid collision. If the medium is free for use, the transmission phase starts in which the packet is entirely transmitted in network byte order. The receiving device computes the CRC and starts the response phase transmitting an acknowledge (decimal 6) in case of correct data reception. If no acknowledgement is received, after an exponential back-off delay, the transmitter retries until the acknowledgement is received or a maximum number of attempts is reached.

```cpp
Channel analysis            Transmission               Response
 _____  _______________________________________________  _____
| C-A || ID |  HEADER  | LENGTH | CRC8 |  DATA  | CRC8 || ACK |
|-----||----|----------|--------|------|--------|------||-----|
|  0  || 12 | 00000100 |   6    |      |   64   |      ||  6  |
|_____||____|__________|________|______|________|______||_____|
 BITS: | 8  |    8     |   8    |  8   |   8    |  8   |
       |____|__________|________|______|________|______|
```

#### Local mode
Depending on header's `MODE` bit, PJON packets can contain basic or extended support to identification. Local mode required by header's `MODE` bit low supports connectivity for up to 254 devices. In the graph below is represented the simplest local mode packet format sending `@` (decimal 64) to device `12`:

```cpp
 _________________________________
|ID| HEADER |LENGTH|CRC8|DATA|CRC8|
|--|--------|------|----|----|----|
|12|00000000|  6   |    | 64 |    |
|__|________|______|____|____|____|
|8 |   8    |  8   | 8  |  8 | 8  | 48 bits
|__|________|______|____|____|____|

```

In local mode packets can be broadcasted to all devices sending to device id `0`. Acknowledgement is not supported therefore any broadcasted packet that requests synchronous and or asynchronous acknowledgement is ignored by recipients.

```cpp
 _________________________________
|ID| HEADER |LENGTH|CRC8|DATA|CRC8|
|--|--------|------|----|----|----|
|0 |00000000|  6   |    | 64 |    |
|__|________|______|____|____|____|
|8 |   8    |  8   | 8  |  8 | 8  | 48 bits
|__|________|______|____|____|____|


```

If header's `TX INFO` bit is high the sender's device id is included in the packet.

```cpp
                        ____
                       | TX |
                       | ID |
 _______________________\__/_________
|ID| HEADER |LENGTH|CRC8|ID|DATA|CRC8|
|--|--------|------|----|--|----|----|
|12|00000010|  7   |    |11| 64 |    |
|__|________|______|____|__|____|____|
|8 |   8    |  8   | 8  |8 | 8  | 8  | 56 bits
|__|________|______|____|__|____|____|


```

#### Shared mode
If header's `MODE` bit is high [bus](/specification/PJON-protocol-specification-v3.1.md#bus) identification is added to the packet. Below, the same local transmission used as an example above is formatted to be sent in shared mode to device id `12` of bus id `0.0.0.1`. The packet's content is prepended with the bus id of the recipient as requested by header's `MODE` bit.
```cpp
 ________________________________________
|ID| HEADER |LENGTH|CRC8|BUS ID|DATA|CRC8|
|--|--------|------|----|------|----|----|
|12|00000001|  10  |    | 0001 | 64 |    |
|__|________|______|____|______|____|____|
|8 |   8    |  8   | 8  |  32  | 8  | 8  | 80 bits
|__|________|______|____|______|____|____|

```

In shared mode packets can be broadcasted to all devices sharing the same bus id sending to device id `0`. Acknowledgement is not supported, therefore any broadcasted packet that requests synchronous and or asynchronous acknowledgement is ignored by recipients.

```cpp
 ________________________________________
|ID| HEADER |LENGTH|CRC8|BUS ID|DATA|CRC8|
|--|--------|------|----|------|----|----|
|0 |00000001|  10  |    | 0001 | 64 |    |
|__|________|______|____|______|____|____|
|8 |   8    |  8   | 8  |  32  | 8  | 8  | 80 bits
|__|________|______|____|______|____|____|

```

If header's `TX INFO` bit is high the sender's device and bus id are included in the packet. In the example below device id `11` of bus id `0.0.0.1` send to device id `12` of bus id `0.0.0.1`.

```cpp
                                _________
                               | TX INFO |
 ______________________________|_________|_________
|ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|DATA|CRC8|
|--|--------|------|----|------|------|--|----|----|
|12|00000011|  15  |    | 0001 | 0001 |11| 64 |    |
|__|________|______|____|______|______|__|____|____|
|8 |   8    |  8   | 8  |  32  |  32  |8 | 8  | 8  | 120 bits
|__|________|______|____|______|______|__|____|____|
```

#### Extended length
The graph below shows a packet transmission where the length is represented with 16 bits supporting up to 65535 bytes length as requested by the header's `EXT. LENGTH` bit. If the extended length feature is used, CRC32 must be applied setting the header's `CRC` bit high.
```cpp
 _______________________________________
|ID| HEADER |LEN 1|LEN 2|CRC8|DATA|CRC32|
|--|--------|-----|-----|----|----|-----|
|12|01100000|  0  | 10  |    | 64 |     |
|__|________|_____|_____|____|____|_____|
|8 |   8    |  8  | 8   | 8  | 8  |  32 | 80 bits
|__|________|_____|_____|____|____|_____|

```

#### Packet identification
The graph below shows a packet in which a 16 bits packet identifier is added as requested by header's `PACKET ID` bit. This feature is provided to avoid duplications and guarantee packet uniqueness. The receiver filters packets containing a packet identifier and sender information that already appeared previously.
```cpp
                                _________
                               | TX INFO |
 ______________________________|_________|____________________
|ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|PACKET ID|DATA|CRC32|
|--|--------|------|----|------|------|--|---------|----|-----|
|12|10100011|  20  |    | 0002 | 0001 |11|   999   | 64 |     |
|__|________|______|____|______|______|__|_________|____|_____|
|8 |   8    |  8   | 8  |  32  |  32  |8 |   16    | 8  | 32  | 160 bits
|__|________|______|____|______|______|__|_________|____|_____|

```

#### Network services
PJON supports a network service identifier by using a 16 bits port id. Thanks to this feature many services can operate and be identified safely. Ports from `0` to `8000` are reserved to known services which index is present in the [known network services list](/specification/PJON-network-services-list.md), ports from `8001` to `65535` are free for custom use cases. The graph below shows a packet transmission where port 8002 is inserted in the packet and header's `PORT` bit is high to signal its presence.
```cpp
 _________________________________________
|ID| HEADER |LENGTH|CRC8|PORT ID|DATA|CRC8|
|--|--------|------|----|-------|----|----|
|12|00010000|  8   |    | 8002  | 64 |    |
|__|________|______|____|_______|____|____|
|8 |   8    |  8   | 8  |  16   | 8  | 8  | 64 bits
|__|________|______|____|_______|____|____|
```
