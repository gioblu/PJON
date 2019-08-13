### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v3.1](/specification/PJON-protocol-specification-v3.1.md)
- [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- **[Network services list](/specification/PJON-network-services-list.md)**
#### Data link layer
- [PJDL (Padded Jittering Data Link) v4.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.0.md)
- [PJDLR (Padded Jittering Data Link over Radio) v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md)
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJON® network services list
This is the known network services port identification list:  

| Port ID | Constant name               | Description/Link                    |
| ------- | --------------------------- | ----------------------------------- |
| `0`     | `PJON_BROADCAST`            | All ports are acceptable            |
| `100`   | `MODULE_INTERFACE`          | [ModuleInterface](https://github.com/fredilarsen/ModuleInterface) automation protocol   |

If you have developed a network service on top of PJON feel free to open an [issue](https://github.com/gioblu/PJON/issues) to obtain a service identification.
