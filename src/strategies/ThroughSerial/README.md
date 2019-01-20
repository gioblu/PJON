## ThroughSerial

**Medium:** Hardware/Software Serial port |
**Pins used:** 1 or 2

With `ThroughSerial` strategy, PJON can run through a software or hardware Serial port working out-of-the-box with many Arduino compatible serial transceivers, like RS485 or radio modules like HC-12 (HCMODU0054). Take a look at the [video introduction](https://www.youtube.com/watch?v=H4jUsgvM-lw) for a brief showcase of its features. It complies with [TSDL v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md).

### Why PJON over Serial?
Serial communication is fast and reliable but it is often useless without all the features PJON contains. `ThroughSerial` has been developed to enable PJON communication through a serial data link. Adding PJON on top of Serial it is possible to leverage of the PJON protocol layer features like acknowledge, addressing, multiplexing, packet handling, 8 or 32-bit CRC and traffic control.

Being impossible to detect or avoid collisions over a serial port, `ThroughSerial` has been developed primarily to be used in master-slave mode. `ThroughSerial` in multi-master mode, being unable to detect or avoid collisions, operates using the slotted ALOHA medium access method. Of all contention based random multiple access methods, slotted ALOHA, which maximum data throughput is only 36.8% of the available bandwidth, is one of the least efficient and should not be applied in networks where many devices often need to arbitrarily transmit data.

### Configuration
Before including `PJON.h` it is possible to configure `ThroughSerial` using predefined constants:

| Constant                | Purpose                             | Supported value                            |
| ----------------------- |------------------------------------ | ------------------------------------------ |
| `TS_BYTE_TIME_OUT`      | Maximum byte response time-out      | Duration in microseconds (50000 by default) |
| `TS_RESPONSE_TIME_OUT`  | Maximum response time-out           | Duration in microseconds (10000 by default) |
| `TS_BACK_OFF_DEGREE`  | Maximum back-off exponential degree | Numeric value (4 by default)               |
| `TS_MAX_ATTEMPTS`     | Maximum transmission attempts       | Numeric value (20 by default)              |


Pass the `ThroughSerial` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
PJON<ThroughSerial> bus;
```
Call the `begin` method on the `Serial` or `SoftwareSerial`  object you want to use for PJON communication and pass it to the `set_serial` method:
```cpp  
#include <PJON.h>

PJON<ThroughSerial> bus;

void setup() {
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
}
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
In some cases it may be required to add a delay before disabling the RS485 pins txe and or rxe:
```cpp
// Set RS485 txe/rxe pins disable delay in milliseconds
bus.strategy.set_RS485_delay(1);
```

See [RS485-Blink](../../../examples/ARDUINO/Local/ThroughSerial/RS485-Blink) and [RS485-AsyncAck](../../../examples/ARDUINO/Local/ThroughSerial/RS485-AsyncAck) examples.

HC-12 wireless module supports both synchronous and asynchronous acknowledgement, see [HC-12-Blink](../../../examples/ARDUINO/Local/ThroughSerial/HC-12-Blink), [HC-12-SendAndReceive](../../../examples/ARDUINO/Local/ThroughSerial/HC-12-SendAndReceive), [HC-12-LocalChat](../../../examples/ARDUINO/Local/ThroughSerial/HC-12-LocalChat) and [HC-12-AsyncAck](../../../examples/ARDUINO/Local/ThroughSerial/HC-12-AsyncAck) examples.

All the other necessary information is present in the general [Documentation](/documentation).

### Known issues
- Being PJON not an interrupt driven, its communication can be affected and potentially disrupted by long delays added in the user's sketch. Try to reduce as possible the interval between every `receive` call.

### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. Before any practical test or hardware purchase for a wireless [ThroughSerial](/src/strategies/ThroughSerial) radio setup, compliance with government requirements and regulations must be ensured.
