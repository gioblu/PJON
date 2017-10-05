
**Medium:** Hardware/Software Serial port |
**Pins used:** 1 or 2

With `ThroughSerial` strategy, PJON can run through a software or hardware Serial port working out of the box with many Arduino compatible serial transceivers, like RS485 or radio modules like HC-12 (HCMODU0054). Take a look at the [video introduction](https://www.youtube.com/watch?v=H4jUsgvM-lw) for a brief showcase of its features.

#### Why PJON over Serial?
Serial communication is fast and quite reliable but it is often useless without all the features PJON contains. `ThroughSerial` has been developed to enable PJON communication through a serial data link. Adding PJON on top of Serial it is possible to leverage of the PJON protocol layer features like acknowledge, addressing, multiplexing, packet handling, 8 or 32-bit CRC and traffic control.

#### How to use ThroughSerial
Pass the `ThroughSerial` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
  PJON<ThroughSerial> bus; // 2 pin over-sampled data link layer
```
Call the `begin` method on the `Serial` or `SoftwareSerial`  object you want to use for PJON communication and pass it to the `set_serial` method:
```cpp  
  /* Set 5 milliseconds (default) as the maximum timeframe for byte reception.
     This value depends on the latency, baud rate and computation time.
     Always set TX_MAX_RESPONSE_TIME > (byte transmission time + latency) */
  #define TS_BYTE_TIME_OUT      5000

  /* Set 10 milliseconds as the maximum timeframe between transmission and
     synchronous acknowledgement response. This value is strictly related to
     the maximum time needed by receiver to receive and compute the packet.
     Higher if necessary. */
  #define TS_RESPONSE_TIME_OUT 10000

  /* Set the back-off exponential degree (default 4) */
  #define TS_BACK_OFF_DEGREE 4

  /* Set the maximum sending attempts (default 20) */
  #define TS_MAX_ATTEMPTS   20

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
  bus.strategy.set_RS485_rxe_pin(11); // Set RS485 reception enable pin
  bus.strategy.set_RS485_txe_pin(12); // Set RS485 transmission enable pin
```

For HC-12 wireless module see [HC-12-Blink](examples/ARDUINO/Local/ThroughSerial/HC-12-Blink), [HC-12-SendAndReceive](examples/ARDUINO/Local/ThroughSerial/HC-12-SendAndReceive) and [HC-12-AsyncAck](examples/ARDUINO/Local/ThroughSerial/HC-12-AsyncAck) examples.

All the other necessary information is present in the general [Documentation](/documentation).

#### Known issues
- HC-12 (HCMODU0054) radio transceiver supports only asynchronous acknowledgement, for this reason it is mandatory to disable synchronous acknowledgement using `set_synchronous_acknowledge(false)` to obtain nominal functionality.
- Being PJON not an interrupt driven, its communication can be affected and potentially disrupted by long delays added in the user's sketch. Try to reduce as possible the interval between every `receive` call.
