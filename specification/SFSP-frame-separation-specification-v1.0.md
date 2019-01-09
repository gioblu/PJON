### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v3.1](/specification/PJON-protocol-specification-v3.1.md)
- [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [Dynamic addressing specification v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v3.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v3.0.md)
- [PJDLR (Padded Jittering Data Link over Radio) v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md)
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md)
- **[SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)**

---

## SFSP (Secure Frame Separation Protocol) v1.0
```
Invented by Giovanni Blu Mitolo
Originally published: 24/11/2017, latest revision: 31/10/2018
Related work: https://github.com/gioblu/PJON/
Compliant implementations: PJON v10.0 and following
Released into the public domain
```
SFSP has been specified to obtain reliable frame separation using byte stuffing and xor based flag obfuscation. It is similar to SLIP and its variations, although uses a more cautious approach, appending at the end of the frame an `END` flag to exclude a set of vulnerabilities. Its overhead is 1.33 times higher if compared to SLIP's but it is more reliable thanks to the presence of the `END` flag. SFSP uses a xor based flag obfuscation method to avoid the occurrence of flags within a frame. Its procedure is lightweight and more efficient if compared with the flag transposition method used by SLIP that requires more memory and operations to be executed. The suggested sampling and transmission strategy is byte-by-byte. SFSP is designed to support fast on the fly encoding and decoding implementations with no need of additional buffers.

### Frame transmission
Frame transmission starts with a `START` flag (decimal 149), followed by data bytes. When `START`, `END` (decimal 234) or `ESC` flag (decimal 76) occurs in data, it is prepended with an `ESC` flag and its value is xored with the `ESC` flag to avoid the presence of `START`, `END` or `ESC` flag in data. `END` is appended at the end of the frame.

```cpp
START:  149 - 10010101 - 0x95 - 
END:    234 - 11101010 - 0xea - ê
ESC:    187 - 10111011 - 0xBB - »

           ______________________________
          |  DATA 1-65535 bytes          |
 _______  |______  _____  _______  ______|  _____
| START | | BYTE || ESC || BYTE  || BYTE | | END |
|-------| |------||-----||-------||------| |-----|
|  149  | |  23  || 76  ||149^ESC||  52  | | 234 |
|_______| |______||_____||_______||______| |_____|
                     |     |
                     |  2 Data byte containing START is xored with ESC
                     |
                  1 Flags inside data are escaped

```

Receiver identifies the start of a frame when a `START` flag is received. If `ESC` flag occurs during reception, it is ignored and the next byte xored with `ESC` flag to get back its original value; any value out `START`, `END` or `ESC` flag causes the receiver to discard the frame and be ready to receive the next one nominally. The end of the frame is detected when an unescaped `END` flag is received.
