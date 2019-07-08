### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v3.1](/specification/PJON-protocol-specification-v3.1.md)
- **[Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)**
- [Dynamic addressing specification v3.0](/specification/PJON-dynamic-addressing-specification-v3.0.md)
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v4.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.0.md)
- [PJDLR (Padded Jittering Data Link over Radio) v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md)
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJON® acknowledge v1.0
```
Invented by Giovanni Blu Mitolo
Originally published: 17/10/2016, latest revision: 31/10/2018
Related implementation: https://github.com/gioblu/PJON/
Compliant versions: PJON v9.0 and following
Released into the public domain
```
PJON supports both synchronous and asynchronous acknowledgement that can be used individually or together.

### Synchronous acknowledge
The synchronous acknowledgement is a blocking procedure, can be used only within one collision domain, although is efficient and has a short response delay because is composed by a single byte (decimal 6).
```cpp
Channel analysis       Transmission                 Response
 _____  ____________________________________________  _____
| C-A || ID |  HEADER  | LENGTH |CRC8| CONTENT |CRC8|| ACK |
|-----||----|----------|--------|----|---------|----||-----|
|  0  || 12 | 00000100 |   6    |    |    64   |    ||  6  |
|_____||____|__________|________|____|_________|____||_____|
```

The graph above contains a packet transmission where the character `@` (decimal 64) is sent to device id `12`. As defined by the [PJON protocol layer specification v3.1](/specification/PJON-protocol-specification-v3.1.md) the header's `ACK` bit with value 1 requests a synchronous acknowledgement response, see [PJDL v4.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.0.md), [PJDLR v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md), [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) and [TSDL v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md) specification for additional information.

### Asynchronous acknowledge
The asynchronous acknowledgement is a non-blocking procedure, it is packet based and can travel across a network composed by many collision domains. Between the packet reception and the asynchronous acknowledgement response the communication medium can be used by other devices.

```cpp
Channel analysis               Transmission
 ____________________________________________________________
|ID| HEADER |LENGTH|CRC|BUS ID|BUS ID|ID|PACKET ID|DATA|CRC32|
|--|--------|------|---|------|------|--|---------|----|-----|
|12|10001011|  20  |   | 0002 | 0001 |11|   99    | 64 |     |
|__|________|______|___|______|______|__|_________|____|_____|
                       |RXINFO| TX INFO |       
```
The graph above contains a standard packet transmission where the character `@` (decimal 64) is sent to device id `12`. As defined by the [PJON protocol specification v3.1](/specification/PJON-protocol-specification-v3.1.md) the header's `ACK MODE` bit with value 1 requests to the recipient an asynchronous acknowledgement response, the `TX INFO` bit with value 1 signals the presence of the sender's info required to respond and the `PACKET ID` bit signals the presence of the packet id used for asynchronous acknowledgement identification. Device `12` of bus `0.0.0.2` answers with an empty asynchronous acknowledgement packet to device `11` of bus `0.0.0.1`.
```cpp
 __ ________ ______ ___ _______ _______ __ _________ _____
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|CRC32|
|--|--------|------|---|-------|-------|--|---------|-----|
|11|00001011|  19  |   |0.0.0.1|0.0.0.2|12|   99    |     |
|__|________|______|___|_______|_______|__|_________|_____|
                       |RX INFO| TX INFO  |
```

### PJON® recursive acknowledgement pattern
The recursive acknowledgement pattern consists in the use of both synchronous and asynchronous acknowledgement.

```cpp
 BUS 0.0.0.1                      BUS 0.0.0.2
 1 collision domain        1 collision domain
 ______             ______             ______
|      |           |      |           |      |
| ID 0 |___________|ROUTER|___________| ID 0 |
|______|           |______|           |______|
```

A router in the center is connected with two network segments or buses part of different collision domains. Communication between device `0` of bus `0.0.0.1` and device `0` of bus `0.0.0.2` can be achieved only through the router.

```cpp        
BUS 0.0.0.1                                          BUS 0.0.0.2

1 Packet send - 2 sync ack, forward - 3 sync ack, async ack send
 ______                      ______                      ______
|      |                    |      |                    |      |
| ID 0 |____________________|ROUTER|____________________| ID 0 |
|______|                    |______|                    |______|
   5 sync ack                   4 sync ack, async ack forward
```

1) Device `0` sends the packet, the router has a route to device `0` of bus `0.0.0.2` so responds with a synchronous acknowledgement.
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ____ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|DATA|CRC||ACK|
|0 |00001111|  20  |   |0.0.0.2|0.0.0.1|0 |   99    | 64 |   || 6 |
|__|________|______|___|_______|_______|__|_________|____|___||___|
                       |RX INFO| TX INFO  |
```
2) Device `0` of bus `0.0.0.1` waits for an asynchronous acknowledgement of the packet sent. Router sends to device id `0` of bus `0.0.0.2` and receives a synchronous acknowledgement.
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ____ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|DATA|CRC||ACK|
|0 |00001111|  20  |   |0.0.0.2|0.0.0.1|0 |   99    | 64 |   || 6 |
|__|________|______|___|_______|_______|__|_________|____|___||___|
                       |RX INFO| TX INFO  |
```
3) Device `0` of bus `0.0.0.2` sends an empty asynchronous acknowledgement packet to device `0` of bus `0.0.0.1`. Router has a route to device `0` of bus `0.0.0.1` so responds with a synchronous acknowledgement on bus `0.0.0.2`. The exchange is ended after a synchronous acknowledgement is received by the router.
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|CRC||ACK|
|0 |00001111|  19  |   |0.0.0.1|0.0.0.2|0 |   99    |   || 6 |
|__|________|______|___|_______|_______|__|_________|___||___|
                       |RX INFO| TX INFO  |
```
4) Device `0` of bus `0.0.0.1` receives the empty asynchronous acknowledgement packet forwarded by the router and responds with a synchronous acknowledgement.

```cpp
 __ ________ ______ ___ _______ _______ __ _________ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|CRC||ACK|
|0 |00001111|  19  |   |0.0.0.1|0.0.0.2|0 |   99    |   || 6 |
|__|________|______|___|_______|_______|__|_________|___||___|
                       |RX INFO| TX INFO  |
```
This documents is not intended to specify the routing procedure but uses routing as a necessary example to showcase clearly the power of the recursive acknowledgement pattern.
