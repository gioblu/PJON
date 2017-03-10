
**Medium:** Light |
**Pins used:** 1 / 2

AnalogSampling is designed to sample digital data using analog readings. It can be used to communicate data wirelessly through light. It is able to use a single LED for both photo-emission and photo-reception phases enabling an half-duplex connection between two devices with a range of maximum 5 meters in ideal conditions using only a pair of LEDs (visible light, infrared or ultraviolet) or with two pairs of any sort of tuned photo-emitters and photo-receivers. It was originally implemented it in the far 2011, here you can see the first [video documented experiment](https://www.youtube.com/watch?v=-Ul2j6ixbmE). It has been recently debugged and updated to act as a PJON Strategy. Take a look at the [video introduction](https://www.youtube.com/watch?v=yIncPe8OPpg) for a brief showcase of its features.

#### Compatibility
- ATmega88/168/328 16Mhz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)

#### Performance
PJON works in 1 communication mode, `STANDARD`:
- `STANDARD` runs at 1024Bd or 128B/s

#### How to use AnalogSampling
Pass the `AnalogSampling` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy. All the other necessary information is present in the general [Documentation](/documentation).
```cpp  
  /* Acknowledge latency maximum duration (1000 microseconds default).
     Could be necessary to higher SWBB_LATENCY if sending long packets because
     of the CRC computation time needed by receiver before transmitting its acknowledge  */
  #define AS_LATENCY 1000

  /* Set the back-off exponential degree */
  #define AS_BACK_OFF_DEGREE 5

  /* Set the maximum sending attempts */
  #define AS_MAX_ATTEMPTS 10

  /* The values set above are the default producing a 3.2 seconds
     back-off timeout with 20 attempts. Higher SWBB_MAX_ATTEMPTS to higher
     the back-off timeout, higher SWBB_BACK_OFF_DEGREE to higher the interval
     between every attempt. */

  #include <PJON.h>

  PJON<AnalogSampling> bus;

  void setup() {
    bus.strategy.set_pin(A0);        // Set the pin A0 as the communication pin
                                     // or
    bus.strategy.set_pins(A0, 12);   // Set pin A0 as input pin and pin 12 as output pin  

    bus.strategy.set_threshold(AS_THRESHOLD); // Set threshold (default value AS_THRESHOLD)
  }

```
After the PJON object is defined with its strategy it is possible to set the communication pin accessing to the strategy present in the PJON instance.

#### What can be done?
The most basic example is to connect two devices using a couple of visible light LEDs connected to the A0 pin used as wireless transceivers.

![PJON AnalogSampling LED wireless communication](http://www.pjon.org/assets/images/PJON-AnalogSampling-half-duplex-led-communication.png)

Infact, leveraging the duality of LEDs:

- Ability to emit photons if electrons are travelling through the junction
- Ability to emit electrons if photons are hitting the junction (photo-electric effect)

it is possibile to use them as wireless (bidirectional) transceivers!

In most cases it is necessary to add a pull-down resistor with a value around 1 mega-ohm connecting the A0 pin with ground to reduce the LED capacitance and so reduce bit transition slopes. Higher resistance can higher the range but can also higher background noise. Consider that depending on the default voltage of your system LEDs could be overpowered. Because of the non continuous power supply also if using a 5v system most LEDs should survive.  

The obtained range is related to:
- **Resolution** - use `analogReference` wiring method to configure `analogRead` resolution as necessary taking care that the input voltage is not higher than the configured voltage reference
- **Sensitivity** find a couple of identical LEDs, position one shining light directly on the other's junction and test the voltage produced by the unpowered one. Blue LEDs can produce more than 2 volts if hit by a twin LED light at full power
- **Transmission power** the more power you feed to your transmitter, the more range and reliability you can get
- **Signal-to-noise ratio or SNR** Noise level affects communication reliability specially when the signal magnitude is lowered by distance, filtering your input from background noise can higher maximum range and overall communication reliability. If your system has to operate with a strong background noise, consider using a hardware filter to cut the background noise frequencies and use a pull down resistor as described above.

With the necessary hardware choices and timing configuration `AnalogSampling` can be used to experiment with short range infrared or visible light communication (i.e. micro-robot swarm, DIY remote), medium range using lighting as transcevers (i.e. cars transmitting data through head and backlights) or long range laser communication (i.e. data between ground and LEO).  

#### Known issues
- Direct sunlight or other light sources can affect receiver's sensitivity and maximum communication range
- A pull-down resistor can be necessary to obtain nominal functionality, see above
