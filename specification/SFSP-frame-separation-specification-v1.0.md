
- PJON (Padded Jittering Operative Network) Protocol specification: [v3.0](/specification/PJON-protocol-specification-v3.0.md)
- Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- Dynamic addressing specification: [v1.0](/specification/PJON-dynamic-addressing-specification-v1.0.md)
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v1.0](/strategies/AnalogSampling/specification/PJDLS-specification-v1.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v1.0](/strategies/ThroughSerial/specification/TSDL-specification-v1.0.md)
- SFSP (Secure Frame Separation Protocol) specification: **[SFSP](/specification/SFSP-frame-separation-specification-v1.0.md)**

```cpp
/*
Milan, Italy
Originally published: 24/11/2017
SFSP v1.0
Invented by Giovanni Blu Mitolo

Related work: https://github.com/gioblu/PJON/
Compliant implementations: PJON v10.0 and following
*/
```

#### SFSP (Secure Frame Separation Protocol) v1.0
SFSP has been specified to obtain reliable frame separation using byte stuffing and XOR based flags obfuscation. It is similar to SLIP and its variations, although a more cautious approach has been applied, appending at the end of the frame a `END` flag to exclude a set of vulnerabilities in case of externally induced error. Its overhead is 1.33 times higher if compared to SLIP's but it is more reliable thanks to the presence of the `END` flag. SFSP's XOR based flags obfuscation method is lightweight and more efficient if compared with the flags transposition method used by SLIP that requires more memory and operations to be executed. The suggested sampling and transmission strategy is byte-by-byte. SFSP is designed to support fast on the fly encoding and decoding implementations with no need of additional buffers.

#### SFSP frame transmission
Frame transmission starts with `START` flag, followed by data bytes. When `START`, `END` or `ESC` occurs in data, it is prepended with `ESC` and its value is XOR-ed with `ESC` to enforce the absence of `START` or `END` in the data stream. `END` is appended at the end of the frame.

```cpp
           ______________________________
          |  DATA 1-65535 bytes          |
 _______  |______  _____  _______  ______|  _____
| START | | BYTE || ESC || START || BYTE | | END |
|-------| |------||-----||-------||------| |-----|
|  149  | |  23  || 76  ||149^ESC||  52  | | 234 |
|_______| |______||_____||_______||______| |_____|
                     |     |
                     |  2 START flag is XOR-ed with ESC flag
                   1 Flags inside data are escaped

START:  149 - 10010101 - 0x95 - 
END:    234 - 11101010 - 0xea - ê
ESC:    187 - 10111011 - 0xBB - »
```
#### SFSP frame reception
Receiver identifies the start of a frame when a `START` is received. If `ESC` occurs during reception, it is ignored and the next byte XOR-ed with `ESC` to get back its original value; any value out `ESC`, `START` or `END` causes the receiver to discard the frame and be ready to receive the next one nominally. The end of the frame is detected when an unescaped `END` is received. The `END` flag can be used to verify that both frame and packet end are coincident: if an abstraction of higher level is requesting to read the last byte of a frame and that is not followed by `END` a parsing error occurred at one or both levels and the frame is discarded.
