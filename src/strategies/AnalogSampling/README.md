## AnalogSampling

**Medium**: light pulses over air or optic-fibre | **Pins used**: 1 or 2

`AnalogSampling` strategy or data link complies with [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md), it is designed to communicate data wirelessly using light impulses and its sampling technique is based on analog readings. This strategy is able to use a single LED for both photo-emission and photo-reception phases providing with wireless half-duplex connectivity between devices with a range of up to 5 meters. Most appliances have at least a useless energy consuming LED on board, right?

`AnalogSampling` can also be used with separate emitter and receiver pins enabling cheap long range wireless communication using standard photo-diodes, light-emitting diodes or laser diodes. The proposed circuit, technique and codebase were originally implemented in the far 2011, see the first [video documented experiment](https://www.youtube.com/watch?v=-Ul2j6ixbmE). Take a look at the [video introduction](https://www.youtube.com/watch?v=1BeGYMjg-DI) for a brief showcase of its features.

### Compatibility
| MCU              | Clock | Supported pins   | Supported modes |
| ---------------- |------ | ---------------- | --------------- |
| ATmega88/168/328 (Duemilanove, Uno, Nano, Pro) | 16MHz | A0, A1, A2, A3, A4, A5 | `1`, `2`, `3`, `4`, `5` |
| ATmega2560 (Mega, Mega nano) | 16MHz | A0, A1, A2, A3, A4, A5 | `1`, `2`, `3` |

### Performance
`AnalogSampling` works with the following communication modes:
- `1` runs at 1024Bd or 128B/s (`AS_PRESCALE` 128)
- `2` runs at 1361Bd or 170B/s (`AS_PRESCALE` 128)
- `3` runs at 3773Bb or 471B/s (`AS_PRESCALE` 32)
- `4` runs at 5547Bb or 639B/s (`AS_PRESCALE` 16)
- `5` runs at 12658Bd or 1528B/s (`AS_PRESCALE` 8)

Caution, mode `5` sets the ADC clock prescale to a higher rate than the manufacturer recommends as maximum ADC sample rate (prescale 16).

### What can be done?
The most basic example is to connect two devices using a couple of visible light LEDs used as wireless transceivers.

![PJON AnalogSampling LED wireless communication](http://www.pjon.org/assets/images/PJON-AnalogSampling-half-duplex-led-communication.png)

Leveraging of the interesting features of LEDs:
- Emit light if powered by electricity
- Emit a small but detectable amount of electricity if hit by light (photo-electric effect)

It is possible to use LEDs as wireless (bidirectional) transceivers. This means that wireless half-duplex connectivity can be provided by a single LED per device. See the [LED selection](documentation/LED-selection.md) guide to know more about how to choose the right LEDs and keep in mind that is necessary to add a 75K-5MΩ pull-down resistor connecting the pin used with ground to reduce the LED capacitance and externally induced interference and that depending on the voltage level used LEDs could be overpowered, add a current limiting resistor if required.

`AnalogSampling` can be used to experiment with short range infrared or visible light communication (remote control, robot swarms, data streaming using lighting), medium range using light sources (cars transmitting data through front and backlights) or long range laser communication (data between ground and LEO).  

### Configuration
Before including `PJON.h` it is possible to configure `AnalogSampling` using predefined constants:

| Constant                  | Purpose                             | Supported value                            |
| ------------------------- |------------------------------------ | ------------------------------------------ |
| `AS_MODE`                 | Data transmission mode              | 1, 2, 3, 4, 5                              |
| `AS_RESPONSE_TIMEOUT`     | Maximum response time-out           | Duration in microseconds (15000 by default) |
| `AS_BACK_OFF_DEGREE`      | Maximum back-off exponential degree | Numeric value (5 by default)               |
| `AS_MAX_ATTEMPTS`         | Maximum transmission attempts       | Numeric value (10 by default)              |
| `AS_PRESCALE`             | Set ADC pre-scaler                  | 8, 16, 32                                  |

Pass the `AnalogSampling` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy. All the other necessary information is present in the general [Documentation](/documentation).
```cpp  
#include <PJON.h>

PJON<AnalogSampling> bus;

void setup() {
  // Set the pin A0 as the communication pin
  bus.strategy.set_pin(A0);

  // Set pin A0 as input pin and pin 12 as output pin
  bus.strategy.set_pins(A0, 12);

  // Set threshold (default value AS_THRESHOLD)
  bus.strategy.set_threshold(AS_THRESHOLD);
}
```
After the PJON object is defined with its strategy it is possible to set the communication pin accessing to the strategy present in the PJON instance.

### Known issues
- Direct sunlight or other light sources can affect receiver's sensitivity and maximum communication range
- Long wires can degrade performance
- Depending on the power supply voltage, LEDs could be overpowered, add a current limiting resistor if required
- Oscilloscope's probe acting as a pull down resistor influences results and the required pull down resistor's value
- A pull-down resistor is required to obtain optimal performance, see above

### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. When working with an [AnalogSampling](/src/strategies/AnalogSampling) LED or laser based setup safety glasses must be worn and transceivers must be operated cautiously to avoid potential eye injuries. Consider that with [AnalogSampling](/src/strategies/AnalogSampling) all LEDs that are physically connected to an ADC may be used maliciously to both download or upload data wirelessly, effectively circumventing many air-gapping techniques.   
