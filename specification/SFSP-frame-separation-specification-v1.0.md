#### Network layer
- PJON (Padded Jittering Operative Network) Protocol specification: [v3.0](/specification/PJON-protocol-specification-v3.0.md)
- PJON Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- PJON Dynamic addressing specification: [v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- PJON Network services: [list](/specification/PJON-network-services-list.md)
#### Data link layer
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- SFSP (Secure Frame Separation Protocol) specification: **[SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)**

```cpp
/*
Milan, Italy
Originally published: 24/11/2017
Latest revision: 26/1/2018
SFSP v1.0
Invented by Giovanni Blu Mitolo,
released into the public domain

Related work: https://github.com/gioblu/PJON/
Compliant implementations: PJON v10.0 and following
*/
```

#### SFSP (Secure Frame Separation Protocol) v1.0
SFSP has been specified to obtain reliable frame separation using byte stuffing and xor based flags obfuscation. It is similar to SLIP and its variations, although uses a more cautious approach, appending at the end of the frame an `END` flag to exclude a set of vulnerabilities. Its overhead is 1.33 times higher if compared to SLIP's but it is more reliable thanks to the presence of the `END` flag. SFSP uses a xor based flag obfuscation method to avoid the occurrence of flags within a frame. Its procedure is lightweight and more efficient if compared with the flag transposition method used by SLIP that requires more memory and operations to be executed. The suggested sampling and transmission strategy is byte-by-byte. SFSP is designed to support fast on the fly encoding and decoding implementations with no need of additional buffers.

#### SFSP frame transmission
Frame transmission starts with `START` flag, followed by data bytes. When `START`, `END` or `ESC` flag occurs in data, it is prepended with a `ESC` flag and its value is xored with `ESC` flag to avoid the presence of `START`, `END` or `ESC` flag in data. `END` is appended at the end of the frame.

```cpp
           ______________________________
          |  DATA 1-65535 bytes          |
 _______  |______  _____  _______  ______|  _____
| START | | BYTE || ESC || START || BYTE | | END |
|-------| |------||-----||-------||------| |-----|
|  149  | |  23  || 76  ||149^ESC||  52  | | 234 |
|_______| |______||_____||_______||______| |_____|
                     |     |
                     |  2 START flag is xored with ESC flag
                     |
                  1 Flags inside data are escaped

START:  149 - 10010101 - 0x95 - 
END:    234 - 11101010 - 0xea - ê
ESC:    187 - 10111011 - 0xBB - »
```
#### SFSP frame reception
Receiver identifies the start of a frame when a `START` flag is received. If `ESC` flag occurs during reception, it is ignored and the next byte xored with `ESC` flag to get back its original value; any value out `START`, `END` or `ESC` flag causes the receiver to discard the frame and be ready to receive the next one nominally. The end of the frame is detected when an unescaped `END` flag is received.
