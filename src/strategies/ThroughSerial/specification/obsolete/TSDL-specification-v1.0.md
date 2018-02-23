- PJON (Padded Jittering Operative Network) Protocol specification:
[v2.0](/specification/PJON-protocol-specification-v2.0.md)
- Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- Dynamic addressing specification: [v1.0](/specification/PJON-dynamic-addressing-specification-v1.0.md)
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v1.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v1.0.md)
- TSDL (Tardy Serial Data Link) specification: **[TSDL v1.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v1.0.md)**

```cpp
/*
Milan, Italy
Originally published: 27/09/2017
TSDL (Tardy Serial Data Link) v1.0 specification
Invented by Giovanni Blu Mitolo,
released into the public domain

Related implementation: /src/strategies/ThroughSerial/
Compliant versions: PJON v9.0 and following
*/
```
### TSDL v1.0

TSDL (Tardy Serial Data Link) is a simplex or half-duplex serial data link that supports both master-slave and multi-master configuration. It supports collision avoidance, reliable frame separation through byte stuffing and optional synchronous response to frame transmissions.

### Basic concepts
* Use start, end and escape flags to support frame separation
* Support collision avoidance
* Support optional 1 byte synchronous response to frame transmissions

```cpp  
 ______ TX   RX ______
|      |-------|      |
|DEVICE|       |DEVICE|
|______|-------|______|
        RX   TX
```
TSDL can be used to establish a point-to-point link between two devices if used with a bare serial link, or supporting one or many to many communication with physical layers that are supporting this feature, like serial radio or RS485 transceivers.    

#### Frame transmission
Before a frame transmission, the serial buffer is read, if not empty ongoing communication is detected and collision avoided, if empty for a duration longer than the time-in before transmission, frame transmission starts with `START` flag, followed by data bytes, if necessary escaped with `ESC` flag and terminates the frame with an `END` flag.
```cpp
           ______________________________
          |  DATA 1-65535 bytes          |
 _______  |______  _____  _______  ______|  _____
| START | | BYTE || ESC || START || BYTE | | END |
|-------| |------||-----||-------||------| |-----|
|  149  | |  23  || 76  ||  149  ||  52  | | 234 |
|_______| |______||_____||_______||______| |_____|
                             |
                           Flags inside data are escaped

START:  149 - 10010101 - 0x95 - 
END:    234 - 11101010 - 0xea - ê
ESC:    187 - 10111011 - 0xBB - »
```
`START` and `END` flag bytes are special characters that signal when a frame begins and ends.
Whenever any of the special character appears in the data, transmitter inserts a special `ESC` character before it, that will be ignored and excluded from data during the reception process. Any corrupted special character or data byte causes the receiver to discard the frame and be ready to receive the next one nominally.   

#### Synchronous response
A frame transmission can be optionally followed by a synchronous response by its recipient.
```cpp  
Transmission                                    Response
 _______  ______  ______  _____                   _____
| START || BYTE || BYTE || END | CRC COMPUTATION | ACK |
|-------||------||------||-----|-----------------|-----|
|  149  ||  H   ||  I   || 234 | LATENCY         |  6  |
|_______||______||______||_____|                 |_____|
```
Between frame transmission and a synchronous response there is a variable timeframe influenced by latency and CRC computation time. The maximum time dedicated to potential acknowledgement reception must be shorter than the transmission time-in, to avoid other devices to disrupt a response exchange, and it is estimated adding the maximum frame length CRC computation time to the expected latency.
