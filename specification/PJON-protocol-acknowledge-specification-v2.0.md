### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v4.0](/specification/PJON-protocol-specification-v4.0.md)
- **[Acknowledge specification v2.0](/specification/PJON-protocol-acknowledge-specification-v2.0.md)**
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v4.1](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.1.md)
- [PJDLR (Padded Jittering Data Link over Radio) v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md)
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJON® acknowledge v2.0
```
Invented by Giovanni Blu Mitolo
Originally published: 17/10/2016
Latest revision: 15/04/2020
Related implementation: https://github.com/gioblu/PJON/
Compliant versions: PJON v13.0 and following
Released into the public domain
```
PJON supports the synchronous acknowledgement feature used to have reception certainty.

### Synchronous acknowledge
The synchronous acknowledgement is a blocking procedure, can be used only within one collision domain, although it is efficient and has a short response delay and is composed by a single byte (decimal 6) at the network layer but each data link may implement it differently according to it features and limitations.
```cpp
Channel analysis       Transmission                 Response
 _____  ____________________________________________  _____
| C-A || ID |  HEADER  | LENGTH |CRC8| CONTENT |CRC8|| ACK |
|-----||----|----------|--------|----|---------|----||-----|
|  0  || 12 | 00000100 |   6    |    |    64   |    ||  6  |
|_____||____|__________|________|____|_________|____||_____|
```

The graph above contains a packet transmission where the character `@` (decimal 64) is sent to device id `12`. As defined by the [PJON protocol layer specification v4.0](/specification/PJON-protocol-specification-v4.0.md), the header's `ACK` bit high requests a synchronous acknowledgement response, see [PJDL v4.1](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.1.md), [PJDLR v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md), [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) and [TSDL v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md) specification for additional information.
