## SoftwareBitBang

**Medium:** Wire |
**Pins used:** 1 / 2

`SoftwareBitBang` is a software implementation of [PJDL (Padded Jittering Data Link)](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.0.md). It supports simplex and half-duplex asynchronous serial communication for up to 254 devices over one common wire. This implementation can run on limited microcontrollers with low clock accuracy and can operate directly using a single input-output pin. It is a valid alternative to 1-Wire because of its flexibility and reliability. Fault tolerance schemes can be easily implemented because the communication pins can be configured at runtime. The maximum length of the bus can reach between 800 and 2000 meters depending on the mode used. Take a look at the [video introduction](https://www.youtube.com/watch?v=GWlhKD5lz5w) for a brief showcase of its features.
```cpp
PJDL SINGLE WIRE BUS                           ______
 ______    ______    ______    ______         |      |
|      |  |      |  |      |  |      |        |DEVICE|
|DEVICE|  |DEVICE|  |DEVICE|  |DEVICE|        |______|
|______|  |______|  |______|  |______|            |
___|__________|________|___________|______/\/\/\__| IO PIN
 ___|__    __|___    ___|__    ___|__  |  110-180 Ω
|      |  |      |  |      |  |      | |  
|DEVICE|  |DEVICE|  |DEVICE|  |DEVICE| |__/\/\/\__  GND
|______|  |______|  |______|  |______|    8 kΩ - 5 MΩ    
```
It is suggested to add 8kΩ-5MΩ pull-down resistor as shown in the graph above to reduce externally induced interference. The longer is the length of the cable and the higher is the amount of induced interference, the lower should be the resistance of the pull-down resistor. Pins can be optionally protected against overload adding a current limiting resistor to each connected pin. The resistor value can be obtained solving the following equation `R = (operating voltage / pin max current drain)`, for example to obtain the current limiting resistor value for an Arduino Uno simply substitute its characteristics: `R = (5v / 0.030A) = 166.66Ω`.

### Compatibility
| MCU              | Clock | Supported pins   |
| ---------------- |------ | ---------------- |
| ATtiny84/84A | 16MHz | 0, 1, 2, 3, 4 |
| ATtiny85 (Digispark development board) | 16MHz | 1, 2 |
| ATmega88/168/328 (Duemilanove, Uno, Nano, Pro) | 16MHz | 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 |
| ATmega328PB | 16MHz | ? |
| ATmega16u4/32u4 (Leonardo, Micro) | 16MHz | 2, 4, 8, 12 |
| ATmega2560 (Mega, Mega nano) | 16MHz | 3, 4, 7, 8, 9, 10, 12 |
| ATmega1284P | 16MHz | 18, 19, 20, 21, 22, 23, A0, A1, A2, A3, A4, A5, A6, A7 |
| SAMD (Arduino Zero) | 48MHz | D0, D1, D3, A0, A1 |
| STM32F1 | 72MHz | ? |
| MK20DX256 (Teensy 3.1) | 96MHz | All pins |
| ESP8266 (NodeMCU, AI-THINKER modules) | 80/160MHz | D1 or GPIO 5 |
| ESP32 (Heltech WiFi LoRa) | 160MHz | 12, 25 |

### Performance
`SWBB_MODE` can be configured in 4 different modes, `1`, `2`, `3` and `4`:

| Mode | Speed | Range | Supported MCUs   |
| ---- | ----- |------ | ---------------- |
| `1`  | 1.95kB/s 15625Bd | 2000m | ATtiny84/84A, ATtiny85, ATmega88/168/328, ATmega16u4/32u4, ATmega2560, ATmega1284P, SAMD, STM32F1, MK20DX256, ESP8266, ESP32 |  
| `2`  | 2.21kB/s 17696Bd | 1600m | ATtiny84/84A, ATtiny85, ATmega88/168/328, ATmega16u4/32u4, ATmega2560 |
| `3`  | 2.94kB/s 23529Bd | 1200m | ATtiny84/84A, ATtiny85, ATmega88/168/328 |
| `4`  | 3.40kB/s 27210Bd |  800m | ATtiny84/84A, ATtiny85, ATmega88/168/328 |

When including and using the `SoftwareBitBang` strategy you have the complete access to the microcontroller. This happens because `SoftwareBitBang` runs a completely software-defined implementation, transforming a painful walk in a nice flight.

Communication over a single wire enables quick and creative experimentation. The first suggested test, at the tester's risk, is to let two arduino boards communicate [through a living body](https://www.youtube.com/watch?v=caMit7nzJsM) touching with the left hand (should be harmless) the digital pin of the first board and with the right the pin of the other one. It is stunning to see highly accurate digital communication running through a living biological body. This opens the mind to creative solutions.

### Configuration
Before including `PJON.h` it is possible to configure `SoftwareBitBang` using predefined constants:

| Constant                | Purpose                             | Supported value                            |
| ----------------------- |------------------------------------ | ------------------------------------------ |
| `SWBB_MODE`             | Data transmission mode              | 1, 2, 3, 4                                 |
| `SWBB_RESPONSE_TIMEOUT` | Maximum response time-out           | Duration in microseconds (1500 by default) |
| `SWBB_BACK_OFF_DEGREE`  | Maximum back-off exponential degree | Numeric value (4 by default)               |
| `SWBB_MAX_ATTEMPTS`     | Maximum transmission attempts       | Numeric value (20 by default)              |

`SoftwareBitBang` supports the use of input and output pins because separated signals may be required if additional circuitry is used for amplification or noise filtering. It also works if pins are directly connected as a simple point-to-point null-modem or cross-over serial link.

```cpp  
#include <PJON.h>

PJON<SoftwareBitBang> bus;

void setup() {
  // Set the pin 12 as the communication pin
  bus.strategy.set_pin(12);
  // Set pin 11 as input pin and pin 12 as output pin
  bus.strategy.set_pins(11, 12);
}
```
After the PJON object is defined with its strategy it is possible to set the communication pin accessing to the strategy present in the PJON instance. All the other necessary information is present in the general [Documentation](/documentation).

### Why not interrupts?
In the Arduino environment the use of libraries is really extensive and often the end user is not able to go over collisions. Very often a library is using hardware resources of the microcontroller, colliding with other libraries. This happens because in general Arduino boards have limited hardware resources. Software-defined bit-banging, is a stable and reliable solution that leads to "more predictable" results than interrupt driven procedures coexisting on limited microcontrollers without the developer and the end user knowing about it.

![PJON - Michael Teeuw application example](http://33.media.tumblr.com/0065c3946a34191a2836c405224158c8/tumblr_inline_nvrbxkXo831s95p1z_500.gif)

PJON application example made by the user [Michael Teeuw](http://michaelteeuw.nl/post/130558526217/pjon-my-son)

### Known issues
- A 1-5 MΩ pull down resistor could be necessary to reduce interference, see [Mitigate interference](https://github.com/gioblu/PJON/wiki/Mitigate-interference).
- When using more than one instance of `SoftwareBitBang` in the same sketch always use pins connected to a different port group to avoid cross-talk.  
- During the execution of other tasks or delays a certain amount of packets could be potentially lost because transmitted out of the polling time of the receiver device. Thanks to the PJON packet handler after some retries the packet is received but a certain amount of bandwidth is wasted. If this situation occurs, try to reduce as much as possible the duration of other tasks and or use a longer polling time using `receive` and passing the requested amount of microseconds: `bus.receive(1000); // Poll for 1 millisecond`.
- `SoftwareBitBang` strategy can have compatibility issues with codebases that are using interrupts, reliability or bandwidth loss can occur because of the interruptions made by third party software.

### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. When a [SoftwareBitBang](/src/strategies/SoftwareBitBang) bus is installed each pin must be protected with a current limiting resistor as described above. If a common ground or power supply line is used its cable size and length must be carefully selected taking in consideration the overall application's power supply requirements and selected components' maximum rating.
