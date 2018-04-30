#### Network layer
- PJON (Padded Jittering Operative Network) Protocol specification: [v3.0](/specification/PJON-protocol-specification-v3.0.md)
- Acknowledge specification: **[v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)**
- Dynamic addressing specification: [v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- PJON known protocols: [list](/specification/PJON-known-protocols-list.md)
#### Data link layer
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- SFSP (Secure Frame Separation Protocol) specification: [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

```cpp
/*
Milan, Italy
Originally published: 17/10/2016
latest revision: 15/10/2017
PJON® protocol acknowledge specification v1.0
Invented by Giovanni Blu Mitolo,
released into the public domain

Related implementation: https://github.com/gioblu/PJON/
Compliant versions: PJON v9.0 and following
*/
```
### PJON® protocol acknowledge specification v1.0
The PJON Standard supports both **synchronous** and **asynchronous** acknowledgement. Those two mechanisms are defined to ensure that a packet transmission ended positively with no errors and they can be used individually or together.

#### Synchronous acknowledge
```cpp
Channel analysis       Transmission                 Response
 _____  ____________________________________________  _____
| C-A || ID |  HEADER  | LENGTH |CRC8| CONTENT |CRC8|| ACK |
|-----||----|----------|--------|----|---------|----||-----|
|  0  || 12 | 00000100 |   6    |    |    64   |    ||  6  |
|_____||____|__________|________|____|_________|____||_____|
```

The graph above contains a standard packet transmission containing a synchronous acknowledgment request where the character `@` (decimal 64) is sent to device id `12`. As defined by the [PJON protocol layer specification v3.0](/specification/PJON-protocol-specification-v3.0.md) the third bit from right up in the header requests a synchronous acknowledgment response. How the synchronous acknowledgement procedure works depends on the data-link used, see [PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md), [PJDLR v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md), [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) and [TSDL v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md) specification.

In a scenario where there is no direct connection between two devices, the synchronous acknowledgement procedure, that is based on sending a single byte `PJON_ACK` (decimal 6) cannot be applied successfully because of the presence of more than one collision domain and because of the impossibility for a switch or a router to safely transport a single byte response.

#### Asynchronous acknowledge
The asynchronous acknowledge is provided using an entire packet, that can safely travel across the network directly, repeated or routed. Between the packet reception and the asynchronous acknowledgment response packet arrival the communication medium can be used by other devices if required.

```cpp
Channel analysis               Transmission                Response
 ____________________________________________________________  ___
|ID| HEADER |LENGTH|CRC|BUS ID|BUS ID|ID|PACKET ID|DATA|CRC32||ACK|
|--|--------|------|---|------|------|--|---------|----|-----||---|
|12|10001111|  20  |   | 0002 | 0001 |11|   99    | 64 |     || 6 |
|__|________|______|___|______|______|__|_________|____|_____||___|
                       |RXINFO| TX INFO |       
```
The graph above contains a standard packet transmission containing an asynchronous acknowledgment request where the character `@` (decimal 64) is sent to device id `12` containing its packet id `99`. As defined by the [PJON protocol specification v3.0](/specification/PJON-protocol-specification-v3.0.md) the fourth bit from right up in the header requests to transmitter an asynchronous acknowledgment response. The second bit from right up signals the inclusion of the sender's info necessary to send back an asynchronous acknowledgment packet when received. The first bit on left signals the presence of the packet id.

#### PJON® recursive acknowledgement pattern
The recursive acknowledgment pattern consists in requesting the synchronous acknowledgment, the asynchronous acknowledgment and including the required meta-information to carry out the exchange.

```cpp
 BUS 0.0.0.1                      BUS 0.0.0.2
 ______             ______             ______
|      |           |      |           |      |
| ID 0 |___________|ROUTER|___________| ID 0 |
|______|           |______|           |______|
```

A router in the center is connected with two buses using different media. The two network segments or buses are two different collision domains, bus `0.0.0.1` and `0.0.0.2`. Communication between device `0` of bus `0.0.0.1` with device `0` of bus `0.0.0.2` can be achieved only with the support of the router.

```cpp        
BUS 0.0.0.1                                          BUS 0.0.0.2

1 Packet send - 2 sync ack, forward - 3 sync ack, async ack send
 ______                      ______                      ______
|      |                    |      |                    |      |
| ID 0 |____________________|ROUTER|____________________| ID 0 |
|______|                    |______|                    |______|
   5 sync ack                   4 ack, async ack forward

/* If packet length - its overhead is 2, it is an asynchronous
   acknowledgement packet containing only its packet id */
```

1) Device `0` sends the packet, the router has a route to device `0` of bus `0.0.0.2` so responds with a synchronous acknowledgement
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ____ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|DATA|CRC||ACK|
|0 |10001111|  20  |   |0.0.0.2|0.0.0.1|0 |   99    | 64 |   || 6 |
|__|________|______|___|_______|_______|__|_________|____|___||___|
                       |RX INFO| TX INFO  |
```
2) Device `0` of bus `0.0.0.1` waits for an asynchronous acknowledgement of the packet sent. Router sends to device id `0` of bus `0.0.0.2` and receives a synchronous acknowledgement
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ____ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|DATA|CRC||ACK|
|0 |10001111|  20  |   |0.0.0.2|0.0.0.1|0 |   99    | 64 |   || 6 |
|__|________|______|___|_______|_______|__|_________|____|___||___|
                       |RX INFO| TX INFO  |
```
3) Device `0` of bus `0.0.0.2` sends an asynchronous acknowledgement packet to device `0` of bus `0.0.0.1`. Router has a route to device `0` of bus `0.0.0.1` so responds with a synchronous acknowledgement and for device `0` of bus `0.0.0.2` the exchange is ended after receiving a synchronous acknowledgement by the router.
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|CRC||ACK|
|0 |10001111|  19  |   |0.0.0.1|0.0.0.2|0 |   99    |   || 6 |
|__|________|______|___|_______|_______|__|_________|___||___|
                       |RX INFO| TX INFO  |
```
4) Device `0` of bus `0.0.0.1` receives the asynchronous acknowledgement packet forwarded by the router and responds with a synchronous acknowledgement.

```cpp
 __ ________ ______ ___ _______ _______ __ _________ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|CRC||ACK|
|0 |10001111|  19  |   |0.0.0.1|0.0.0.2|0 |   99    |   || 6 |
|__|________|______|___|_______|_______|__|_________|___||___|
                       |RX INFO| TX INFO  |
```
This documents is not intended to specify the routing procedure but uses routing as a necessary example to showcase clearly the power of the recursive acknowledgement pattern.
