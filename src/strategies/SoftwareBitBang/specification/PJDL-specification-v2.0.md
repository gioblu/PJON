#### Network layer
- PJON (Padded Jittering Operative Network) Protocol specification: [v3.0](/specification/PJON-protocol-specification-v3.0.md)
- Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- Dynamic addressing specification: [v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- PJON known protocols: [list](/specification/PJON-known-protocols-list.md)
#### Data link layer
- PJDL (Padded Jittering Data Link) specification:
**[PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md)** - [PJDLR v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- SFSP (Secure Frame Separation Protocol) specification: [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

```cpp
/*
Milan, Italy
Originally published: 10/04/2010
latest revision: 31/12/2017
PJDL (Padded Jittering Data Link) v2.0 specification
Invented by Giovanni Blu Mitolo,
released into the public domain

Related implementation: /src/strategies/SoftwareBitBang/
Compliant versions: PJON 9.0 and following

Changelog:
- Added frame separation
- Added communication modes specification
*/
```
### PJDL v2.0
PJDL (Padded Jittering Data Link) is a simplex or half-duplex data link layer, that can be easily software emulated, enabling one or many to many communication over a single conductive medium or bus, connected to device's input-output ports, in both master-slave and multi-master configuration. It has been engineered to have limited minimum requirements, and to be efficiently executed on limited microcontrollers with low clock accuracy. No additional hardware is required to apply PJDL, and, being implemented in less than 350 lines of code, it is easily portable to many different architectures. Bus maximum length is limited by its electric resistance; it has been tested with up to 50m long insulated wires and results demonstrate the same high performance achieved with shorter lengths.

### Basic concepts
* Define a synchronization pad initializer to identify a byte
* Use synchronization pad's falling edge to achieve byte level synchronization
* Use 3 consequent synchronization pads to identify a frame
* Detect interference or absence of communication at byte level
* Support collision avoidance and detection
* Support error detection
* Support 1 byte synchronous response to frame

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
Padding bits add a certain overhead but are reducing the need of precise timing because synchronization is renewed every byte. The first padding bit duration is the synchronization timeframe the receiver has to receive a byte. If the length of the first padding bit is less than the minimum acceptable duration, the received signal is considered interference. The minimum acceptable duration of the first padding bit must be lower than a padding bit duration; a large minimum acceptable duration reduces the chances of occurring false positives, a small minimum acceptable duration instead mitigates timing inaccuracy, for this reason it is suggested to evaluate its setting depending on requirements and available resources.

#### Frame transmission
Before a frame transmission, the communication medium is analysed, if logic 1 is present ongoing communication is detected and collision avoided, if logic 0 is detected for a duration longer than a byte transmission plus its synchronization pad and a small random timeframe, frame transmission starts with 3 synchronization pads, followed by data bytes. The presence of synchronization pads with their logic 1 between each byte ensures that also a frame composed of a series of bytes with decimal value 0 can be transmitted safely without risk of third-party collision.
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
In a scenario where a frame is received, low performance microcontrollers with inaccurate clock can correctly synchronize with transmitter during the frame initializer, and consequently each byte is received. The frame initializer is detected if 3 synchronizations occurred and if its duration is equal or higher than:

`initializer expected duration - (sync pad bit 1 duration - sync pad bit 1 minimum acceptable duration)`

To ensure 100% reliability separating frames the sync pad minimum acceptable duration must be higher than 1 standard bit duration. Selecting a correct `sync pad bit 1 / standard bit` ratio, called pad-bit ratio, frame initializer is 100% reliable, false positives cannot occur if not because of externally induced interference. Sync pad bit 1 duration must not be an exact multiple of a standard bit, for this reason pad-bit ratio of 2.0, 3.0 or 4.0 must be avoided because consecutive bits can be interpreted as a frame initializer.

#### Synchronous response
A frame transmission can be optionally followed by a synchronous response by its recipient. This feature is available for both master-slave and multi-master configuration.
```cpp  
Transmission                                    Response
 ______  ______  ______  ______                   _____
| INIT || BYTE || BYTE || BYTE | CRC COMPUTATION | ACK |
|------||------||------||------|-----------------|-----|
|      ||      ||      ||      | LATENCY         |  6  |
|______||______||______||______|                 |_____|
```

Between frame transmission and a synchronous response there is a variable timeframe influenced by latency and CRC computation time. In order to avoid other devices to consider the medium free and disrupt an ongoing exchange, sender cyclically transmits a shorter than one bit logic 1 (which exact length depends on practical requirements) and consequently attempts to receive a response. On the other side receiver can synchronize its response transmission after the last incoming high bit, detect if acknowledgement was lost by transmitter and try again if necessary.
```cpp  
Transmission                                      Response
 ______  ______  ______  ______   _   _   _   _   _ _____
| INIT || BYTE || BYTE || BYTE | | | | | | | | | | | ACK |
|------||------||------||------| | | | | | | | | | |-----|
|      ||      ||      ||      | | | | | | | | | | |  6  |
|______||______||______||______|_| |_| |_| |_| |_| |_____|

```

The maximum time dedicated to potential acknowledgement reception and consequent medium jittering is estimated adding the maximum frame length CRC computation time to the expected latency. Thanks to the presence of the jittering wave, many differently configured devices can coexist on the same medium with no risk of collision.

#### Communication modes
The proposed communication modes are the result of years of testing and optimization and have been selected to be easily supported by limited microcontrollers.  

| MODE | Bit timing | Sync bit timing | Pad-bit ratio | Speed               |
| ---- | ---------- | --------------- | ------------- | ------------------- |
| 1    | 40         | 112             | 2.8           | 2.118kB/s - 16944Bd |
| 2    | 32         | 84              | 2.625         | 2.688kB/s - 21504Bd |

Binary timing durations are expressed in microseconds.
