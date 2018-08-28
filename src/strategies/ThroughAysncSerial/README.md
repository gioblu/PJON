### ThroughAsyncSerial

**Medium:** Hardware/Software Serial port |
**Pins used:** 1 or 2

With `ThroughAsyncSerial` strategy, PJON can run through a software or hardware Serial port working out of the box with many Arduino compatible serial transceivers, like RS485 or radio modules like HC-12 (HCMODU0054). Take a look at the [video introduction](https://www.youtube.com/watch?v=H4jUsgvM-lw) for a brief showcase of its features. It complies with [TSDL v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md).  This strategy is based upon the ThroughSerial Strategy but uses the hardware serial buffers present on most Arduino devices (incuding ESP8266).  It is not required to call bus.receive() with any delay, just call is frequently to see if there are any packets that have been received.  Your loop can now run 10000's of times per second and speeds of 200 packets a second are possible between ESP32 and Leonado at 115200 baud. 

#### Why PJON over Serial?
Serial communication is fast and reliable but it is often useless without all the features PJON contains. `ThroughAsyncSerial` has been developed to enable PJON communication through a serial data link. Adding PJON on top of Serial it is possible to leverage of the PJON protocol layer features like acknowledge, addressing, multiplexing, packet handling, 8 or 32-bit CRC and traffic control.  This version is very suited to ESO8266 and ESP32 where polling using SWBB is not ideal.  Receive is entirely non-blocking.  Sending, however, and ACK are still blocking. There is a default timeout on checking data that is 100us.  This is to allow data to accumulate in the harware UART buffer.  This value is changable with `bus.strategy.set_timeout(0)` and may require adjustment depending on UART RX buffer size and baud rate.  

#### How to use ThroughAsyncSerial
Pass the `ThroughSerial` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
#include PJON_INCLUDE_TAS
PJON<ThroughAsyncSerial> bus;
```
Call the `begin` method on the `Serial` or `SoftwareSerial`  object you want to use for PJON communication and pass it to the `set_serial` method:
```cpp  
/* Set 5 milliseconds (default) as the maximum timeframe for
   byte reception. Always set:
   TX_MAX_RESPONSE_TIME > (byte transmission time + latency) */
#define TAS_BYTE_TIME_OUT      5000

/* Set 10 milliseconds as the maximum timeframe between
   transmission and synchronous acknowledgement response.
   This value is strictly related to the maximum time needed
   by receiver to receive and compute the packet.
   Higher if necessary. */
#define TAS_RESPONSE_TIME_OUT 10000

/* Set the back-off exponential degree (default 4) */
#define TAS_BACK_OFF_DEGREE 4

/* Set the maximum sending attempts (default 20) */
#define TAS_MAX_ATTEMPTS   20

#include <PJON.h>

PJON<ThroughAsyncSerial> bus;

void setup() {
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial);
  bus.strategy.set_timeout(0); 
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
See [RS485-Blink](../../examples/ARDUINO/Local/ThroughSerial/RS485-Blink) and [RS485-AsyncAck](../../examples/ARDUINO/Local/ThroughSerial/RS485-AsyncAck) examples.

HC-12 wireless module supports both synchronous and asynchronous acknowledgement, see [HC-12-Blink](../../examples/ARDUINO/Local/ThroughSerial/HC-12-Blink), [HC-12-SendAndReceive](../../examples/ARDUINO/Local/ThroughSerial/HC-12-SendAndReceive) and [HC-12-AsyncAck](../../examples/ARDUINO/Local/ThroughSerial/HC-12-AsyncAck) examples.

All the other necessary information is present in the general [Documentation](/documentation).

#### Known issues
- Send not ASYNC.
- ACK not ASYNC. 
