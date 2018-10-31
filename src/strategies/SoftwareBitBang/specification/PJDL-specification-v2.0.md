
### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v3.0](/specification/PJON-protocol-specification-v3.0.md)
- [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [Dynamic addressing specification v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- **[PJDL (Padded Jittering Data Link) v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md)**
- [PJDLR (Padded Jittering Data Link over Radio) v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md)
- [PJDLS (Padded Jittering Data Link Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJDL v2.0
```
Invented by Giovanni Blu Mitolo, released into the public domain
Originally published: 10/04/2010, latest revision: 30/10/2018
Related implementation: /src/strategies/SoftwareBitBang/
Compliant versions: PJON 9.0 and following
```
PJDL (Padded Jittering Data Link) is an asynchronous serial data link for low-data-rate applications that supports one or many to many communication over a common conductive medium. PJDL can be easily implemented on limited microcontrollers with low clock accuracy and can operate directly using a single input-output pin.

### Physical layer
The medium's maximum length is limited by the wiring resistance, by the voltage level used and by externally induced interference. It has been tested with up to 50 meters long insulated wires and results demonstrate the same performance achieved with shorter lengths. The maximum range is still unknown.
```cpp
PJDL SINGLE WIRE BUS                            ______
 ______    ______    ______    ______          |      |
|      |  |      |  |      |  |      |         |DEVICE|
|DEVICE|  |DEVICE|  |DEVICE|  |DEVICE|         |______|
|______|  |______|  |______|  |______|             |
___|__________|________|___________|_______/\/\/\__| IO PIN
 ___|__    __|___    ___|__    ___|__   |  110-180 Ω
|      |  |      |  |      |  |      |  |  
|DEVICE|  |DEVICE|  |DEVICE|  |DEVICE|  |__/\/\/\__  GND
|______|  |______|  |______|  |______|     1-5 MΩ    
```
It is suggested to add 1-5 MΩ pull-down resistor as shown in the graph above to reduce externally induced interference. Pins can be optionally protected against overload adding a current limiting resistor to each connected pin. The resistor value can be obtained solving the following equation `R = (operating voltage / pin max current drain)`, for example to obtain the current limiting resistor value for an Arduino Uno simply substitute its characteristics: `R = (5v / 0.030A) = 166.66Ω`.

### Communication modes
The proposed communication modes are the result of years of testing and optimization and have been selected to be easily supported by limited microcontrollers.  

| MODE | Data bit duration | Padding bit duration | Pad-data ratio  | Bandwidth           |
| ---- | ----------------- | -------------------- | --------------- | ------------------- |
| 1    | 40                | 112                  | 2.8             | 2.11kB/s  - 16944Bd |
| 2    | 36                | 88                   | 2.444           | 2.42kB/s  - 19417Bd |
| 3    | 24                | 80                   | 3.333           | 3.37kB/s  - 27027Bd |

Binary timing durations are expressed in microseconds.

### Medium access control
PJDL specifies a contention based random multiple access method that supports multi-master communication. Collisions can only occur when 2 or more devices start to transmit at the same time because devices can securely detect an ongoing transmission. When a collision occurs it can be detected by the receiver because of synchronization loss. In multi-master operation the maximum data throughput effectively available is 36.8% of the bandwidth (the same as slotted ALOHA). In master-slave operation the maximum data throughput is equal to the bandwidth.

### Byte transmission
Byte transmission is composed by 10 bits, the first two are called synchronization pad and are used to obtain sampling synchronization. The synchronization pad is composed by a logic 1 padding bit longer than data bits and a logic 0 data bit. The following 8 data bits contain information in LSB-first (least significant bit first) order.

The reception technique is based on 3 steps:
1. Find a logic 1 which duration is equal or acceptably shorter than the expected padding bit duration
2. Synchronize to its falling edge
3. Ensure it is followed by a logic 0 data bit

If this pattern is detected data reception starts, if not, interference, synchronization loss or simply absence of communication is detected.
```cpp  
 ___________ ___________________________
| SYNC-PAD  | DATA                      |
|_______    |___       ___     _____    |
|  |    |   |   |     |   |   |     |   |
|  | 1  | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|__|____|___|___|_____|___|___|_____|___|
   |
Minimum acceptable padding bit duration
```
The synchronization pad adds overhead although it includes synchronization along with the data and eliminates the need of a dedicated clock line. The minimum acceptable padding bit duration is the timeframe in which a receiver initiating polling can correctly receive a byte. If the duration of the padding bit is shorter than the minimum acceptable duration the received signal is discarded. The minimum acceptable duration of the padding bit must be shorter than a padding bit duration; a large minimum acceptable duration reduces the chances of false positive's occurrences, a small minimum acceptable duration instead mitigates timing inaccuracies. The presence of the synchronization pad between each byte also ensures that a frame composed of a series of bytes with decimal value 0 can be transmitted safely without risk of collision.

### Frame transmission
Before a frame transmission the communication medium is analysed, if logic 1 is present communication is detected and collision is avoided, if logic 0 is detected for a duration of one byte plus a small random time, frame transmission starts with an initializer composed by 3 consequent synchronization pads followed by data bytes. The synchronization pad is used for both byte and frame initialization to reduce the implementation complexity.  
```cpp  
 ________ _________________ __________________________________
|ANALYSIS|   FRAME INIT    | DATA 1-65535 bytes               |
|________|_____ _____ _____|________________ _________________|
|        |Sync |Sync |Sync |Sync | Byte     |Sync | Byte      |
|        |___  |___  |___  |___  |     __   |___  |      _   _|
|        |   | |   | |   | |   | |    |  |  |   | |     | | | |
|00000000| 1 |0| 1 |0| 1 |0| 1 |0|0000|11|00| 1 |0|00000|1|0|1|
|________|___|_|___|_|___|_|___|_|____|__|__|___|_|_____|_|_|_|
```
When a frame is received a low performance microcontroller with an inaccurate clock can correctly synchronize with transmitter during the frame initializer and consequently each byte is received. On receiver's side a frame reception starts if 3 synchronization pads are detected and if their duration is equal or higher than:

`frame initializer expected duration - (sync bit 1 duration - sync bit 1 minimum acceptable duration)`

To ensure 100% reliability the padding bit must be longer than data bits. Selecting a correct `padding bit / data bit` ratio, called pad-data ratio, frame initialization is 100% reliable, false positives can only occur because of externally induced interference. The padding bit duration must not be an exact multiple of the duration of one data bit, for this reason pad-data ratio of 1, 2, 3 or 4 must be avoided because a single or consecutive data bits may be erroneously interpreted as a padding bit.

### Synchronous response
A frame transmission can be optionally followed by a synchronous response of its recipient.
```cpp  
Transmission                                    Response
 ______  ______  ______  ______                   _____
| INIT || BYTE || BYTE || BYTE | CRC COMPUTATION | ACK |
|------||------||------||------|-----------------|-----|
|      ||      ||      ||      | LATENCY         |  6  |
|______||______||______||______|                 |_____|
```

Between frame transmission and a synchronous response there is a variable timeframe which duration is influenced by latency and CRC computation time. In order to avoid other devices to consider the medium free and disrupt an ongoing exchange, sender cyclically transmits a short logic 1 (1/4 data bit duration) and consequently attempts to receive a response. On the other side receiver can synchronize its response transmission after the last incoming high bit. If the acknowledgement is not correctly received the transmitter continues to keep busy the channel up to the maximum acceptable timeframe between transmission and response.
```cpp  
Transmission                                      Response
 ______  ______  ______  ______   _   _   _   _   _ _____
| INIT || BYTE || BYTE || BYTE | | | | | | | | | | | ACK |
|------||------||------||------| | | | | | | | | | |-----|
|      ||      ||      ||      | | | | | | | | | | |  6  |
|______||______||______||______|_| |_| |_| |_| |_| |_____|

```

The maximum time dedicated to potential acknowledgement reception for a given application is estimated by adding the CRC computation time of the longest supported frame to the maximum latency of the hardware and the medium used.
