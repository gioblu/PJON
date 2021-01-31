## ThroughSerial

| Medium | Communication mode | Pins used | Inclusion |
|--------|--------------------|-----------|-----------|
| wires  | master-slave, multi-master | 2 | `#include <PJONThroughSerial.h>`|

With `ThroughSerial` strategy, PJON can run through a software or hardware Serial port working out of the box with many Arduino compatible serial transceivers, like RS485 or radio modules like HC-12 (HCMODU0054). It complies with [TSDL v3.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v3.0.md).  

This strategy is based upon the obsolete blocking implementation although reception is now asynchronous and completely non-blocking. It is not required to call `bus.receive()` with any delay, just call it frequently to see if there are any packets that have been received.

### Why PJON over Serial?
Serial communication is fast and reliable but it is often useless without all the features PJON contains. `ThroughSerial` has been developed to enable PJON communication through a serial data link. Adding PJON on top of Serial it is possible to leverage of the PJON protocol layer features like acknowledge, addressing, multiplexing, packet handling, 8 or 32-bit CRC and traffic control.  

`ThroughSerial` implements a variation of CSMA (Carrier Sense Multiple Access). It is able to avoid collision with an ongoing frame transmission but cannot detect or avoid same-time collisions. The reception phase is entirely non-blocking although sending and acknowledgement are still blocking.

### Configuration
Before including the library it is possible to configure `ThroughSerial` using predefined constants:

| Constant                | Purpose                             | Supported value                            |
| ----------------------- |------------------------------------ | ------------------------------------------ |
| `TS_READ_INTERVAL`      | Minimum interval between receptions | Duration in microseconds (100 by default)  |
| `TS_BYTE_TIME_OUT`      | Maximum byte reception time-out     | Duration in microseconds (1000000 by default) |
| `TS_RESPONSE_TIME_OUT`  | Maximum response time-out           | Duration in microseconds (45000 by default) |
| `TS_BACK_OFF_DEGREE`    | Maximum back-off exponential degree | Numeric value (4 by default)               |
| `TS_MAX_ATTEMPTS`       | Maximum transmission attempts       | Numeric value (20 by default)              |

Use `PJONThroughSerial` to instantiate a PJON object ready to communicate using `ThroughSerial` strategy:
```cpp  
#include <PJONThroughSerial.h>
PJONThroughSerial bus;
```
Call the `begin` method on the `Serial` or `SoftwareSerial`  object you want to use for PJON communication and pass it to the `set_serial` method:
```cpp  
#include <PJONThroughSerial.h>
PJONThroughSerial bus;

void setup() {
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
}
```
There is a default reception interval of 100 microseconds used to allow data to accumulate in the hardware UART buffer. This value is configurable using `bus.strategy.set_read_interval(100)` passing an arbitrary interval in microseconds. The read interval may require adjustment depending on UART RX buffer size and baud rate.
```cpp
bus.strategy.set_read_interval(100);
```
For a simple use with RS485 serial modules a transmission enable pin setter has been added:
```cpp  
bus.strategy.set_enable_RS485_pin(11);
```
If separate enable setters are needed use:
```cpp  
// Set RS485 reception enable pin
bus.strategy.set_RS485_rxe_pin(11);
// Set RS485 transmission enable pin
bus.strategy.set_RS485_txe_pin(12);
```
See the [BlinkTest](../../../examples/ARDUINO/Local/ThroughSerial/BlinkTest) and [BlinkWithResponse](https://github.com/gioblu/PJON/tree/master/examples/ARDUINO/Local/ThroughSerial/BlinkWithResponse) examples, if you need to interface devices using RS485 see the [RS485-Blink](../../../examples/ARDUINO/Local/ThroughSerial/RS485-Blink) example. HC-12 wireless module supports the synchronous acknowledgement, see [HC-12-Blink](../../../examples/ARDUINO/Local/ThroughSerial/HC-12-Blink), [HC-12-SendAndReceive](../../../examples/ARDUINO/Local/ThroughSerial/HC-12-SendAndReceive) and [HC-12-LocalChat](../../../examples/ARDUINO/Local/ThroughSerial/HC-12-LocalChat) examples.

All the other necessary information is present in the general [Documentation](/documentation).

### Known issues
- Transmission is still blocking, will be made non-blocking in the next versions.
- acknowledgement procedure is still blocking, will be made non-blocking in the next versions.
