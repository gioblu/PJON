## OverSampling

**Medium:** ASK/FSK radio modules -
**Pins used:** 1 or 2 -
**Inclusion constant:** `PJON_INCLUDE_OS`

`OverSampling` is a software implementation of [PJDLR (Padded Jittering Data Link over Radio)](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md). It supports simplex and half-duplex asynchronous serial communication and implements a carrier-sense, non-persistent random multiple access method (non-persistent CSMA). This implementation can run on limited microcontrollers with low clock accuracy, supports communication for many devices connected to the same medium and stable operation in spite of interference. Its procedure is a more efficient alternative to the LoRa Open Standard (that specifies a variation of Slotted ALOHA) and it is designed to obtain long range and high reliability using FSK, ASK or OOK modulation radio transceivers. Take a look at the [video introduction](https://www.youtube.com/watch?v=G1ckfsMzPns) for a brief showcase of its features.

### Compatibility
| MCU              | Clock | Supported pins   |
| ---------------- |------ | ---------------- |
| ATmega88/168/328 (Duemilanove, Uno, Nano, Pro) | 16MHz | 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1 |
| ATmega16u4/32u4 (Leonardo, Micro) | 16MHz | 2, 4, 8, 12 |
| ATmega2560 (Mega, Mega nano) | 16MHz | 3, 4, 7, 8, 9, 10, 12 |
| ESP8266 | 80/160MHz | D0 or GPIO 16, D1 or GPIO 5 |

### Performance
- Transfer speed: 202 B/s or 1620 Baud
- Data throughput: 150 B/s
- Range: 250 meters with no line of sight, 5 km with line of sight and ideal atmospheric conditions

### Configuration
Before including `PJON.h` it is possible to configure `OverSampling` using predefined constants:

| Constant                  | Purpose                             | Supported value                             |
| ------------------------- |------------------------------------ | ------------------------------------------- |
| `OS_TIMEOUT`              | Maximum latency                     | Duration in microseconds (10000 by default) |
| `OS_BACK_OFF_DEGREE`      | Maximum back-off exponential degree | Numeric value (5 by default)                |
| `OS_MAX_ATTEMPTS`         | Maximum transmission attempts       | Numeric value (10 by default)               |

Pass the `OverSampling` type as PJON template parameter to instantiate a new PJON object. All the other necessary information is present in the general [Documentation](/documentation).
```cpp  

#include <PJON.h>

PJON<OverSampling> bus;

void setup() {
  // Set the pin 12 as the communication pin
  bus.strategy.set_pin(12);

  // Set pin 11 as input pin and pin 12 as output pin
  bus.strategy.set_pins(11, 12);
}  
```
After the PJON object is defined with its strategy it is possible to set the communication pins using the setters described in the example below.

### Use OverSampling with cheap 433MHz transceivers
To build an open-source PJON packet radio able to communicate up to 5 km away you need only a couple (for `PJON_SIMPLEX` mode) or two couples (for `PJON_HALF_DUPLEX` mode) of cheap 315/433MHz ASK/FSK/OOK transmitter and receiver modules (with a cost around 2/3 dollars each). Please be sure of the regulations your government imposes on radio transmission over these frequencies before use.

![PJON Oversampling packet radio](http://www.gioblu.com/PJON/PJON-OverSampling-packet-radio-STX882-SRX882.jpg)

The maximum detected range was experimented with a small packet radio transmitting its position every minute. The maximum range obtained was slightly more than 5 kilometers in line of sight in open area. Testing it instead in an urban environment the range is down to 250 meters. Two couples of STX882 and SRX882 were used as transceivers. If you choose these modules, remember to set `HIGH` the pin `CS` in the receiver module before starting reception.

### Antenna design
Experiments in `PJON_HALF_DUPLEX` mode have shown that it seems better to keep isolated the two antennas, using two different, not connected elements to transmit and receive. The first suggested antenna design is a wide beam dipole antenna made by two 173mm (quarter wavelength) or 345mm (half wavelength) long conductors, one connected to ground and the other connected to the input or output pin:
```cpp  
173mm quarter wavelength / 345mm half wavelength

-------------------|--------------------
                 __|__
                |TX/RX|
                |_____|
```
A more directional, compact and long range antenna design is the wip antenna. Can be easily crafted with two 173mm (quarter wavelength) / 345mm (half wavelength) long insulated wire sections wrapped with each other every 5mm, one is connected to ground and the other to the input or output pin. This design helps because of its strong ground plane, often necessary to have decent results with this sort of hardware.
```cpp  
       5mm
        || 173mm quarter wavelength / 345mm half wavelength
GND   --\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
RX/TX --/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/
```

### Known issues
- In older versions, `OverSampling` was affected by ineffective and short range if used in `PJON_HALF_DUPLEX` mode. This issue has been fixed by suggesting the use of pins part of 2 different port groups.

### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. Before any practical test or hardware purchase for a wireless [OverSampling](/src/strategies/OverSampling) radio setup, compliance with government requirements and regulations must be ensured.
