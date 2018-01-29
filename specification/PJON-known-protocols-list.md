#### Network layer
- PJON (Padded Jittering Operative Network) Protocol specification: [v3.0](/specification/PJON-protocol-specification-v3.0.md)
- Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- Dynamic addressing specification: [v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- PJON known protocols: **[list](/specification/PJON-known-protocols-list.md)**
#### Data link layer
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v2.0](/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v2.0](/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- SFSP (Secure Frame Separation Protocol) specification: [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

### Known protocols
This is the known protocols port identification list. If you have developed or have ported a protocol on top of PJON Feel free to open an [issue](https://github.com/gioblu/PJON/issues) to obtain a known protocol identification and add it here:  

| Port ID | Constant name               | Description/Link                    |
| ------- | --------------------------- | ----------------------------------- |
| `0`     | `PJON_BROADCAST`            | All ports are considered acceptable |
| `1`     | `PJON_DYNAMIC_ADDRESSING`   | PJON [dynamic addressing](/specification/PJON-dynamic-addressing-specification-v2.0.md) |
| `100`   | `MODULE_INTERFACE`          | [ModuleInterface](https://github.com/fredilarsen/ModuleInterface) automation protocol   |
