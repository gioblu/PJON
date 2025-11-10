### Specifications index

#### Network layer
- **[PJON (Padded Jittering Operative Network) v4.0](/specification/PJON-protocol-specification-v4.0.md)**
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v5.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v5.0.md)
- [PJDLR (Padded Jittering Data Link over Radio) v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md)
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v3.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v3.0.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJON® v4.0
```
Invented by Giovanni Blu Mitolo
with the help of Fred Larsen
Originally published: 10/04/2010
Latest revision: 28/08/2020
Related work: https://github.com/gioblu/PJON/
Compliant implementations: PJON v13.0 and following
Released into the public domain

10/04/2010 0.1 - First local mode draft
19/08/2015 0.2 - First local and shared mode draft
04/09/2016 0.3 - Added header proposed by Fred Larsen
03/10/2016 1.0 - 16 bits length, CRC8 and CRC32 added
28/03/2017 1.1 - Removed NAK, added unacceptable headers
15/10/2017 2.0 - Length corruption vulnerability avoided
14/12/2017 3.0 - Port and packet id added
31/10/2018 3.1 - Model simplified
10/03/2020 3.2 - Missing encoding specification added
28/08/2020 4.0 - MAC and hops added, async-ack removed
```

The PJON protocol v4.0 in local mode supports connectivity for up to 254 devices, in shared mode supports connectivity for up to 4.294.967.295 buses (groups of devices) and up to 1.090.921.692.930 devices. The packet format is dynamic therefore meta-data can be optionally included using the header as a bitmap of selected features. It supports interoperability between systems that use a different configuration and provides with high efficiency including only the protocol's features used and the overhead effectively required (5-35 bytes). PJON can be used for low-data-rate applications as an alternative to 1-Wire, i2c or CAN but can also be applied in place of IP to interconnect more complex networks.   

The graph below shows the conceptual model that characterizes and standardizes the communication. Its goal is the interoperability of diverse systems on a wide range of media with the use of a new set of Open Standards. The graph partitions represent abstraction layers.

```
 ________________________________________________
| 3 Network layer: PJON                          |
|- Optional features ----------------------------|
| Routing and switching                          |
| Hop count (8 bits)                             |
| Hardware identification (48 bits)              |
| Service identification (16 bits)               |
| Packet identification (16 bits)                |
| Bus identification (32 bits)                   |
| Sender identification                          |
| Packet transmission, maximum length 65535B     |
|- Core features --------------------------------|
| Congestion control                             |
| Packet transmission, maximum length 255B       |
| Error detection (16 or 40 bits)                |
| Device identification (8 bits)                 |
| Broadcast                                      |
|________________________________________________|
| 2 Data link layer: PJDL, PJDLR, PJDLS, TSDL    |
| Acknowledgement                                |
| Frame transmission                             |
| Medium access control                          |
|________________________________________________|
| 1 Physical layer:                              |
| Electric, radio or light impulses              |
|________________________________________________|
```

### Basic concepts
* Devices are identified by a unique 8 bits device id
* Buses are identified with a 32 bits bus id
* Devices can be optionally identified with a 48 bits MAC address
* Devices communicate using packets with a maximum length of 255 or 65535 bytes
* Packet transmission is regulated by a 8 bits header
* An acknowledgement can be requested
* Packets can be optionally identified with a 16 bits packet id
* Network services are optionally identified with a 16 bits port id  

### Bus
A bus is a group of up to 254 devices. Devices use packets to communicate with each other. Devices can send packets and optionally request an acknowledgement. Devices can also broadcast packets. When the device id is not known, devices must use id 255.
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

ID 0   = BROADCAST
ID 255 = NOT ASSIGNED  
```

### Bus network
A bus network is composed by buses present on the same collision domain or interconnected through switches or routers. On a shared medium an additional 32 bits bus id must be used to isolate groups of devices from foreign traffic.
```cpp  
TWO BUSES SHARING THE SAME MEDIUM
1 collision domain

    BUS ID 0.0.0.1         BUS ID 0.0.0.2
 _______     _______     _______     _______
|       |   |       |   |       |   |       |
| ID 1  |   | ID 2  |   | ID 1  |   | ID 2  |
|_______|   |_______|   |_______|   |_______|
______|___________|__________|___________|___
       ___|___                 ___|___
      |       |               |       |
      | ID 3  |               | ID 3  |
      |_______|               |_______|
```

### Switch
A switch is a device that forwards packets transparently between connected buses. The switch can operate also if different physical layers are in use and can rely on a default gateway.
```cpp
 ______            ________             ______
|      | PJDL bus |        | PJDLR bus |      |
| ID 1 |__________| SWITCH |___________| ID 2 |
|______|          |________|           |______|

ID 254 = DEFAULT GATEWAY  
```

### Router
A router is a device connected to more than one PJON bus that routes packets from a device, a bus or a medium to another. Packets can be routed between indirectly connected buses if a routing table or a default gateway is used.
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
The header is a bitmap of the meta-data contained and the configuration required. Unlike other protocols, PJON has a dynamic packet format designed to include in each packet only what is strictly required to carry out the exchange. Depending on the bitmap configuration a variable overhead (5-35 bytes) is added to information.
```cpp
HEADER BITMAP
    8      7     6    5     4     3     2      1
 ______ ______ ____ _____ _____ _____ _____ _____
|PACKET|EXT.  |CRC |PORT | MAC | ACK |TX   |MODE |
|ID    |LENGTH|    |     |     |     |INFO |     |
|______|______|____|_____|_____|_____|_____|_____|
```
1. `MODE` bit informs if the packet is formatted in [shared](/specification/PJON-protocol-specification-v4.0.md#shared-mode) (value 1) or [local](/specification/PJON-protocol-specification-v4.0.md#local-mode) mode (value 0)  
2. `TX INFO` bit informs if the sender info are included (value 1) or not (value 0)
3. `ACK` bit informs if the [acknowledgement](/specification/PJON-protocol-specification-v4.0.md#packet-transmission) is requested (value 1) or not (value 0)
4. `MAC` bit informs if sender's and recipient's [hardware identification](/specification/PJON-protocol-specification-v4.0.md#hardware-identification), or MAC address, are included (value 1) or not (value 0)
5. `PORT` bit informs if a 16 bits [network service identifier](/specification/PJON-protocol-specification-v4.0.md#network-services) is included (value 1) or not (value 0)
6. `CRC` bit signals which CRC is used, [CRC8](/specification/PJON-protocol-specification-v4.0.md#crc8-polynomial) (value 0) or [CRC32](/specification/PJON-protocol-specification-v4.0.md#crc32-polynomial) (value 1)
7. `EXT. LENGTH` bit informs if the packet contains 8 (value 0) or 16 bits (value 1) [length](/specification/PJON-protocol-specification-v4.0.md#extended-length)
8. `PACKET ID` bit informs if the packet contains (value 1) or not (value 0) a 16 bits [packet id](/specification/PJON-protocol-specification-v4.0.md#packet-identification)

Unacceptable header configuration states for standard transmission:
* `-10-----` or `EXT. LENGTH` bit high and `CRC` bit low (forced CRC32 for length > 15)

Unacceptable header configuration states for a broadcast transmission:
* `-----1--` or `ACK` bit high (acknowledgement not supported if broadcasting)

`-` symbol means irrelevant bit value

### Cyclic redundancy check
PJON supports both CRC8 and CRC32 to ensure safety on a wide range of use cases and packet lengths.

#### CRC8 polynomial
```cpp
0x97 = (x + 1)(x^7 + x^6 + x^5 + x^2 + 1)^2
```
`CRC8 C2`, discovered by Tsonka Baicheva, is used (in implicit +1 notation) because it has the longest possible length (119 bits) at which `HD=4` can be achieved with an 8-bit CRC. Other protocols specify the use of polynomials with much lower overall performance like `CRC-8 0xEA` or `DOWCRC 0x8C` used by 1-Wire.

#### CRC32 polynomial
```cpp
0x82608edb = x^32 + x^26 + x^23 + x^22 + x^16 +
             x^12 + x^11 + x^10 + x^8 + x^7 +
             x^5 + x^4 + x^2 + x + 1
```
`CRC32 IEEE 802.3` bit-reversed polynomial implicit +1 notation, or `0xedb88320`, is selected for its high performance on a wide range of lengths, while also being widely evaluated and accepted as a good polynomial.

#### Initial meta-data CRC8
CRC8 is calculated and appended to the initial meta-data (device id, header and length) to ensure consistency and avoid the length corruption vulnerability that affects CAN (Controlled Area Network) and many other protocols.

#### End CRC8/CRC32
CRC8 is appended at the end of packets of up to 15 bytes length (overhead included). CRC32 is instead used if the packet's length exceeds 15 bytes but can be optionally applied in shorter packets setting the `CRC` bit high if more secure error detection is required.

### Packet transmission
A local packet transmission is an optionally bidirectional communication between two devices that can be divided in 3 phases: **medium access**, **transmission** and optional **acknowledgement**. In the medium access phase the medium's state is assessed before starting transmission to avoid collision. If the medium is free for use, the transmission phase starts in which the packet is entirely transmitted in network byte order. The receiving device computes the CRC and starts the acknowledgement phase transmitting `ACK` (decimal 6) in case of correct data reception. If no acknowledgement is received, after an exponential back-off delay, the transmitter retries until the acknowledgement is received or a maximum number of attempts is reached.
```cpp
Medium access              Transmission                Response
 _____  _______________________________________________  _____
| M-A || ID |  HEADER  | LENGTH | CRC8 |  DATA  | CRC8 || ACK |
|-----||----|----------|--------|------|--------|------||-----|
|  0  || 12 | 00000100 |   6    |      |   64   |      ||  6  |
|_____||____|__________|________|______|________|______||_____|
 BITS: | 8  |    8     |   8    |  8   |   8    |  8   |
       |____|__________|________|______|________|______|
```
The acknowledgement ensures reliable delivery only when a packet transmission occurs directly without the intermediation of routers or switches. When a packet needs to traverse a network the acknowledegment is transmitted by the nearest intermediary therefore it ensures only best-effort delivery.     

The medium‑access, collision resolution, transmission and acknowledgement procedures are specified by the underlying layer, which also supplies the timing required for retransmissions.

#### Local mode
Depending on header's `MODE` bit packets can contain basic or extended support to identification. Local mode required by header's `MODE` bit low supports connectivity for up to 254 devices. In the graph below is represented the simplest local mode packet format sending `@` (decimal 64) to device `12`:

```cpp
 _________________________________
|ID| HEADER |LENGTH|CRC8|DATA|CRC8|
|--|--------|------|----|----|----|
|12|00000000|  6   |    | 64 |    |
|__|________|______|____|____|____|
|8 |   8    |  8   | 8  |  8 | 8  | 48 bits
|__|________|______|____|____|____|

```

In local mode a broadcast can be sent to all devices sending to device id `0`. Acknowledgement is not supported therefore any broadcast that requests an acknowledgement is ignored by recipients.

```cpp
 _________________________________
|ID| HEADER |LENGTH|CRC8|DATA|CRC8|
|--|--------|------|----|----|----|
|0 |00000000|  6   |    | 64 |    |
|__|________|______|____|____|____|
|8 |   8    |  8   | 8  |  8 | 8  | 48 bits
|__|________|______|____|____|____|


```

If header's `TX INFO` bit is high the sender's device id is included in the packet. In the example below device id `11` sends `@` to device id `12`.

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
If header's `MODE` bit is high [bus](/specification/PJON-protocol-specification-v4.0.md#bus) identification is added to the packet. The same local transmission used as an example above is formatted to be sent in shared mode to device id `12` of bus id `0.0.0.1`. The packet's payload is prepended with the bus id of the recipient and the hop count:
```cpp
 _____________________________________________
|ID| HEADER |LENGTH|CRC8|BUS ID|HOP|DATA|CRC8|
|--|--------|------|----|------|---|----|----|
|12|00000001|  11  |    | 0001 | 0 | 64 |    |
|__|________|______|____|______|___|____|____|
|8 |   8    |  8   | 8  |  32  | 8 | 8  | 8  | 88 bits
|__|________|______|____|______|___|____|____|

```

In shared mode a broadcast can be sent to all devices sharing the same bus id sending to device id `0`. Acknowledgement is not supported therefore any broadcast that requests an acknowledgement is ignored by recipients.

```cpp
 ____________________________________________
|ID| HEADER |LENGTH|CRC8|BUS ID|HOP|DATA|CRC8|
|--|--------|------|----|------|---|----|----|
|0 |00000001|  11  |    | 0001 | 0 | 64 |    |
|__|________|______|____|______|___|____|____|
|8 |   8    |  8   | 8  |  32  | 8 | 8  | 8  | 88 bits
|__|________|______|____|______|___|____|____|

```

If header's `TX INFO` bit is high the sender's device and bus id are included in the packet. In the example below device id `11` of bus id `0.0.0.1` sends to device id `12` of bus id `0.0.0.1`.

```cpp
                                _________
                               | TX INFO |
 ______________________________|_________|_____________
|ID| HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|HOP|DATA|CRC8|
|--|--------|------|----|------|------|--|---|----|----|
|12|00000011|  16  |    | 0001 | 0001 |11| 0 | 64 |    |
|__|________|______|____|______|______|__|___|____|____|
|8 |   8    |  8   | 8  |  32  |  32  |8 | 8 | 8  | 8  | 128 bits
|__|________|______|____|______|______|__|___|____|____|
```

The hop count must be incremented each time the packet is forwarded and routers must ignore packets when a maximum amount of hops is reached.

#### Extended length
if the header's `EXT. LENGTH` bit is high the length of the packet is represented with 16 bits supporting a maximum length of up to 65535 bytes. If the `EXT. LENGTH` bit is low the packet length is represented with 8 bits supporting a maximum length of up to 255 bytes. If the extended length feature is used, CRC32 must be applied setting the header's `CRC` bit high.
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
if the header's `PACKET ID` bit is high a 16 bits packet identifier is added to the packet. The graph below shows a packet in which a 16 bits packet identifier is included. This feature is provided to avoid duplications and guarantee packet uniqueness. The receiver discards packets containing a packet identifier and sender information already appeared previously.
```cpp
 ____________________________________________
|ID| HEADER |LENGTH|CRC8|PACKET ID|DATA|CRC32|
|--|--------|------|----|---------|----|-----|
|12|10000000|  11  |    |   999   | 64 |     |
|__|________|______|____|_________|____|_____|
|8 |   8    |  8   | 8  |   16    | 8  | 32  | 88 bits
|__|________|______|____|_________|____|_____|

```

#### Network services
if the header's `PORT` bit is high a 16 bits port id is added to the packet. Thanks to this feature different services, protocols or formats can coexist and be identified safely. Ports from `0` to `8000` are reserved for the [known network services](/specification/PJON-network-services-list.md), ports from `8001` to `65535` are free for use. The graph below shows a packet transmission where port 8002 is inserted in the packet and header's `PORT` bit is high to signal its presence.
```cpp
 _________________________________________
|ID| HEADER |LENGTH|CRC8|PORT ID|DATA|CRC8|
|--|--------|------|----|-------|----|----|
|12|00010000|  8   |    | 8002  | 64 |    |
|__|________|______|____|_______|____|____|
|8 |   8    |  8   | 8  |  16   | 8  | 8  | 64 bits
|__|________|______|____|_______|____|____|
```

#### Hardware identification
If the header's `MAC` bit is high both recipient's and sender's 48 bits MAC addresses are included in the packet. The recipient's MAC address must match the MAC address of the receiver otherwise the packet is discarded. When the hardware identification is present the packet is received even if the bus id and or the device id don't match. If a packet contains an empty recipient's MAC or `0.0.0.0.0.0` it is not discarded even if the MAC address do not match. The graph below shows a broadcast packet transmission where MAC address `1.1.1.1.1.1` sends to MAC address `2.2.2.2.2.2` the payload 64.
```cpp
 ________________________________________________
|ID| HEADER |LENGTH|CRC8|RX MAC|TX MAC|DATA|CRC32|
|--|--------|------|----|------|------|----|-----|
|0 |00001000|  12  |    |222222|111111| 64 |     |
|__|________|______|____|______|______|____|_____|
|8 |   8    |  8   | 8  |  48  |  48  | 8  | 32  | 168 bits
|__|________|______|____|______|______|____|_____|
```
When a device knows its own MAC address but doesn't know its own device id it must use device id 255. To reach another device in the same configuration, it can transmit packets to device id 255 and include the MAC addresses setting the `MAC` bit high. In this case even if many devices could be present using device id 255, only the one with the matching MAC address receives the packet. If the `ACK` bit is high the receiver can send an acknowledge, although also a broadcast can be used if the acknowledgement is not required.

```cpp
 _________________________________________________  ___
|ID | HEADER |LENGTH|CRC8|RX MAC|TX MAC|DATA|CRC32||ACK|
|---|--------|------|----|------|------|----|-----||---|
|255|00001100|  12  |    |222222|111111| 64 |     || 6 |
|___|________|______|____|______|______|____|_____||___|
| 8 |   8    |  8   | 8  |  48  |  48  | 8  | 32  | 168 bits
|___|________|______|____|______|______|____|_____|
```
