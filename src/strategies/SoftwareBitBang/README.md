### SoftwareBitBang

**Medium:** Wire |
**Pins used:** 1 / 2

`SoftwareBitBang` is a simplex or half-duplex asynchronous serial strategy or data link that complies with [PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md), requires 1 wire and no additional hardware to handle one or many to many communication. It can be executed by limited microcontrollers with low clock accuracy sharing a common direct pin-to-pin connection or 2 using pins connected to external circuitry. It is a valid alternative to 1-Wire because of its flexibility and reliability. Fault tolerance schemes can be easily implemented because the communication pins can be configured at runtime. Bus maximum length is limited by cable's resistance, by the voltage level used and by externally induced interference. It has been tested with up to 50 meters long insulated wires and results demonstrate the same performance achieved with shorter lengths. The maximum range is still unknown. Take a look at the [video introduction](https://www.youtube.com/watch?v=Vg5aSlD-VCU) for a brief showcase of its features.
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

#### Compatibility
| MCU              | Clock | Supported pins   |
| ---------------- |------ | ---------------- |
| ATtiny84/84A | 16MHz | 0, 1, 2, 3, 4 |
| ATtiny85 (Digispark development board) | 16MHz | 1, 2 |
| ATmega88/168/328 (Duemilanove, Uno, Nano, Pro) | 16MHz | 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 |
| ATmega16u4/32u4 (Leonardo, Micro) | 16MHz | 2, 4, 8, 12 |
| ATmega2560 (Mega, Mega nano) | 16MHz | 3, 4, 7, 8, 9, 10, 12 |
| ATmega1284P | 16MHz | 18, 19, 20, 21, 22, 23, A0, A1, A2, A3, A4, A5, A6, A7 |
| SAMD (Arduino Zero) | 48MHz |  |
| MK20DX256 (Teensy 3.1) | 96MHz | All pins |
| ESP8266 (NodeMCU, AI-THINKER modules) | 80/160MHz | D1 or GPIO 5 |
| ESP32 (Heltech WiFi LoRa) | 160MHz | 12, 25 |

#### Performance
`SWBB_MODE` can be configured in 3 different modes, `1`, `2` and `3`:
- `1` runs at 16944Bd or 2.12kB/s cross-architecture, promiscuous clock/architecture compatible.
- `2` runs at 19417Bd or 2.42kB/s cross-architecture, promiscuous clock/architecture compatible.
- `3` runs a specific architecture at its maximum limits (non cross-architecture compatible), Arduino Duemilanove, Uno or nano for example runs at 27024Bd or 3378B/s, Arduino Zero can reach 48000Bd or 6000B/s.

When including and using `SoftwareBitBang`, as data link layer of a PJON bus, you have the complete access to the microcontroller ready to be used, as usual, untouched. This happens because `SoftwareBitBang` runs a completely software emulated implementation, transforming a painful walk in a nice flight.

Single wire simplicity let you to experiment quickly and with creativity. The first suggested test, at the tester's risk, is to let two arduino boards communicate [through a living body](https://www.youtube.com/watch?v=caMit7nzJsM) touching with the left hand the digital port of the first board (5v 40ma, harmless) and with the right the port of the other one. It is stunning to see highly accurate digital communication running through a living biological body. This opens the mind to possible creative solutions.

#### How to use SoftwareBitBang
Before including `PJON.h` it is possible to configure `SoftwareBitBang` using predefined constants:
```cpp  
/* SoftwareBitBang default SWBB_MODE: 1
   (Transfer speed: 16.949kBb or 2.11kB/s) */

// Set SWBB_MODE 2 before PJON.h inclusion
// (Speed: 21.505kBd or 2.68kB/s)
#define SWBB_MODE 2

// Set SWBB_MODE 3 before PJON.h inclusion
// (Architecture / Toolchain dependant)
#define SWBB_MODE 3

/* Synchronous acknowledgement response timeout
   (1.5 milliseconds by default) If latency + CRC
   computation > SWBB_RESPONSE_TIMEOUT
   synchronous acknowledgement reliability could
   be affected or disrupted higher
   SWBB_RESPONSE_TIMEOUT if necessary. */
#define SWBB_RESPONSE_TIMEOUT 1500

// Set the back-off exponential degree (default 4)
#define SWBB_BACK_OFF_DEGREE     4

// Set the maximum sending attempts (default 20)
#define SWBB_MAX_ATTEMPTS       20

#include <PJON.h>

PJON<SoftwareBitBang> bus;

void setup() {
  // Set the pin 12 as the communication pin
  bus.strategy.set_pin(12);
  // Set pin 11 as input pin and pin 12 as output pin
  bus.strategy.set_pins(11, 12);
}
```
The SoftwareBitBang implementation permits the use of input and output pins because separated signals may be required if additional circuitry is used for amplification or noise filtering. It also works if you directly connect the pins as a simple point-to-point null-modem or cross-over serial link.

After the PJON object is defined with its strategy it is possible to set the communication pin accessing to the strategy present in the PJON instance. All the other necessary information is present in the general [Documentation](/documentation).

#### Why not interrupts?
In the Arduino environment the use of libraries is really extensive and often the end user is not able to go over collisions. Very often a library is using hardware resources of the microcontroller as timers or interrupts, colliding or interrupting other libraries. This happens because in general Arduino boards have limited hardware resources. To have a universal and reliable communication medium in this sort of environment, software emulated bit-banging, is a good, stable and reliable solution that leads to "more predictable" results than interrupt driven systems coexisting on small microcontrollers without the original developer and the end user knowing about it.

![PJON - Michael Teeuw application example](http://33.media.tumblr.com/0065c3946a34191a2836c405224158c8/tumblr_inline_nvrbxkXo831s95p1z_500.gif)

PJON application example made by the user [Michael Teeuw](http://michaelteeuw.nl/post/130558526217/pjon-my-son)

#### Known issues
- A 1-5 MΩ pull down resistor could be necessary to reduce interference, see [Mitigate interference](https://github.com/gioblu/PJON/wiki/Mitigate-interference).
- When using more than one instance of `SoftwareBitBang` in the same sketch always use pins connected to a different port group to avoid cross-talk.  
- During the execution of other tasks or delays a certain amount of packets could be potentially lost because transmitted out of the polling timeframe of the receiver device. Thanks to the PJON packet handler after some retries the packet is received but a certain amount of bandwidth is wasted. If this situation occurs, try to reduce as much as possible the duration of other tasks and or use a longer polling timeframe using `receive` passing the requested amount of microseconds: `bus.receive(1000); // Poll for 1 millisecond`.
- `SoftwareBitBang` strategy can have compatibility issues with codebases that are using interrupts, reliability or bandwidth loss can occur because of the interruptions made by third party software.

#### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. When a [SoftwareBitBang](/src/strategies/SoftwareBitBang) bus is installed each pin must be protected with a current limiting resistor as described above. If a common ground or power supply line is used its cable size and length must be carefully selected taking in consideration the overall application's power supply requirements and selected components' maximum rating.
