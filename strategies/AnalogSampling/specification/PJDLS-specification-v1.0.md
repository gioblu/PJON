- PJON (Padded Jittering Operative Network) Protocol specification:
[v2.0](/specification/PJON-protocol-specification-v2.0.md)
- Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- Dynamic addressing specification: [v1.0](/specification/PJON-dynamic-addressing-specification-v1.0.md)
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - **[PJDLS v1.0](/strategies/AnalogSampling/specification/PJDLS-specification-v1.0.md)**
- TSDL (Tardy Serial Data Link) specification: [TSDL v1.0](/strategies/ThroughSerial/specification/TSDL-specification-v1.0.md)

```cpp
/*
Milan, Italy - Originally published: 02/10/2017
PJDLS (Padded Jittering Data Link byte Stuffed) v1.0
Invented by Giovanni Blu Mitolo released into the public domain

Related implementation: /strategies/AnalogSampling/
Compliant implementation versions: PJON 9.0 and following
Changelog: Added frame separation
*/
```
### PJDLS (Padded Jittering Data Link byte Stuffed) v1.0
PJDLS is a simplex or half-duplex data link layer, that can be easily software emulated, enabling one or many to many communication in both master-slave and multi-master configuration, optimized for use cases where high quality synchronization cannot be achieved. Frame separation is obtained with the use of start and end flags along with byte stuffing. It has been engineered to have limited minimum requirements, and to be efficiently executed on limited microcontrollers with poor clock accuracy. No additional hardware is required to apply PJDLS, and, being implemented in c++, in less than 350 lines of code, it is easily portable to many different architectures.

#### Basic concepts
* Define a synchronization pad initializer to identify a byte
* Use synchronization pad's falling edge to achieve byte level synchronization
* Support frame separation using `START`, `END` flags and byte stuffing
* Detect interference or absence of communication at byte level
* Support collision avoidance
* Support 1 byte synchronous response to frame

#### Byte transmission
Each byte is prepended with a synchronization pad and transmission occurs LSB-first. The first bit is a longer than standard logic 1 followed by a standard logic 0. The reception method is based on finding a logic 1 as long as the first padding bit within a certain threshold, synchronizing to its falling edge and checking if it is followed by a logic 0. If this pattern is detected, reception starts, if not, interference, synchronization loss or simply absence of communication is detected at byte level.    
```cpp  
 __________ ___________________________
| SyncPad  | Byte                      |
|______    |___       ___     _____    |
|  |   |   |   |     |   |   |     |   |
|  | 1 | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|__|___|___|___|_____|___|___|_____|___|
   |
Minimum acceptable HIGH padding bit duration
```
Padding bits add a certain overhead but are reducing the need of precise timing because synchronization is renewed every byte. The first padding bit duration is the synchronization timeframe the receiver has to receive a byte. If the length of the first padding bit is less than the minimum acceptable duration, the received signal is considered interference.

#### Frame transmission
Before a frame transmission, the channel is analysed, if ongoing communication is detected collision is avoided, if free for a duration longer than the time-in before transmission, frame transmission starts with `START` flag, followed by data bytes, if necessary escaped with `ESC` flag and terminates the frame with an `END` flag.
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
A frame transmission can be optionally followed by a synchronous response by its recipient. This feature is available for both master-slave and multi-master. In multi-master configuration the maximum acceptable acknowledgement overall response time must be less than the initial channel analysis duration before frame transmission.
```cpp  
Transmission                                             Response
 _______  ______  ______  ______  _____                   _____
| START || BYTE || BYTE || BYTE || END | CRC COMPUTATION | ACK |
|-------||------||------||------||-----|-----------------|-----|
|  149  ||  H   ||  I   ||   !  || 234 | LATENCY         |  6  |
|_______||______||______||______||_____|                 |_____|
```

In master-slave configuration the maximum time dedicated to potential acknowledgement reception it is defined by the use case constraints like maximum packet length and latency or physical distance between devices.

#### Communication modes
The proposed communication modes are the result of years of testing and optimization for light pulses communication and have been selected to be easily supported also by low quality hardware.  

| MODE | Bit timing | Sync bit timing | Pad-bit ratio | Speed               |
| ---- | ---------- | --------------- | ------------- | ------------------- |
| 1    | 750        | 1050            | 1.4           | 128B/s  -  1024Bb   |
| 1    | 572        | 728             | 1.2727        | 170B/s  -  1361Bb   |
| 1    | 188        | 428             | 2.2765        | 471B/s  -  3773Bb   |
| 1    | 128        | 290             | 2.2656        | 471B/s  -  3773Bb   |
| 1    | 128        | 290             | 2.2656        | 639B/s  -  5547Bb   |
| 1    | 56         | 128             | 2.2857        | 1582B/s - 12658Bb   |

Binary timing durations are expressed in microseconds.
