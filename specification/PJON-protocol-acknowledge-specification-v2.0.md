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
Latest revision: 30/04/2020
Related implementation: https://github.com/gioblu/PJON/
Compliant versions: PJON v13.0 and following
Released into the public domain
```
PJON supports the synchronous acknowledgement feature used to have reception certainty.

### Synchronous acknowledge
When the synchronous acknowledgement is requested, the transmitter has reception certainty. The acknowledgement is composed by a single byte (`ACK` or 6) at the network layer but each strategy may implement it according to their features and limitations.
```cpp
Channel analysis       Transmission                 Response
 _____  ____________________________________________  _____
| C-A || ID |  HEADER  | LENGTH |CRC8| CONTENT |CRC8|| ACK |
|-----||----|----------|--------|----|---------|----||-----|
|  0  || 12 | 00000100 |   6    |    |    64   |    ||  6  |
|_____||____|__________|________|____|_________|____||_____|
```

A receiver device responds `ACK` if transmission occurs correctly, instead does not respond nothing if an error is detected.

The graph above contains a packet transmission where the character `@` or 64 is sent to device id `12`. As defined by the [PJON protocol layer specification v4.0](/specification/PJON-protocol-specification-v4.0.md), the header's `ACK` bit high requests a synchronous acknowledgement response, see [PJDL v4.1](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.1.md), [PJDLR v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md), [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) and [TSDL v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md) specification for additional information.
