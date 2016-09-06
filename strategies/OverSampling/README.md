
**Media:** Radio, Wire |
**Pins used:** 1 / 2

Oversampling strategy comes from the [PJON_ASK](https://github.com/gioblu/PJON_ASK) repository, and it was integrated in the PJON repository from version 3.0 beta, as a data link layer strategy. Bits are over-sampled to have high resilience in high interference scenarios, like using an ASK/FSK cheap radio transceivers in an urban environment. It is tested effectively with many versions of the ASK/FSK 315/433Mhz modules available on the market, but it works nominally also through wires and the human body.

####Compatibility
- ATmega88/168/328 16Mhz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16Mhz (Arduino Mega)
- ATmega16u4/32u4 16Mhz (Arduino Leonardo)

####Performance
- Transfer speed: 202 B/s or 1620 Baud
- Data throughput: 150 B/s
- Range: 250 meters in urban environment / 5km with LOS transmitting from a flying balloon

####How to use OverSampling
Pass the `OverSampling` type as PJON template parameter to instantiate a PJON object ready to communicate in this Strategy. All the other necessary information is present in the general [Documentation](https://github.com/gioblu/PJON/wiki/Documentation).
```cpp  
PJON<OverSampling> bus; // 2 pin over-sampled data link layer

void setup() {
  bus.strategy.set_pin(12);      // Set the pin 12 as the communication pin
                                 // or
  bus.strategy.set_pins(11, 12); // Set pin 11 as input pin and pin 12 as output pin  
}  
```
After the PJON object is defined with its strategy it is possible to set the communication pin accessing to the strategy present in the PJON instance.

####Use OverSampling with cheap 433Mhz transceivers
To build a real open-source PJON packet radio able to communicate up to 5km you need only a couple (for `SIMPLEX` mode) or two couples (for `HALF_DUPLEX` mode) of cheap 315/433Mhz ASK/FSK transmitter / receiver modules (with a cost around 2/3 dollars). Please be sure of the regulations your government imposes on radio transmission over these frequencies before use.

![PJON Oversampling packet radio](http://www.gioblu.com/PJON/PJON-OverSampling-packet-radio-STX882-SRX882.jpg)

The maximum detected range was experimented launching a balloon with a small payload containing the packet radio transmitting its position every minute. The maximum range obtained was slightly more than 5 kilometers. Two couples of STX882 and SRX882 were used as transceivers. If you choose these modules, remember to set `HIGH` the pin `CS` on the receiver before starting reception.

Using `OverSampling` data link layer, synchronous acknowledge can reduce the maximum range, on certain media, so if you detect reduced range performance in `HALF_DUPLEX` compared to a mono-directional or `SIMPLEX` communication, and you can do without `ACK`, configure the absence of it after the packet transmission:
```cpp  
  bus.set_acknowledge(false);
```

####Antenna design
Experiments in `HALF_DUPLEX` mode have shown that it seems better to keep isolated the two antennas, using two different, not connected elements to transmit and receive. The first suggested antenna design is a wide beam pseudo half-wavelength dipole antenna made by two 345mm long conductive elements, one connected to ground and the other connected to the input or output pin:
```cpp  

      345mm                    345mm   
 -------------------|--------------------   
                  __|__                                 
                 |tx/rx|                                  
                 |_____|                                

```
A more directional, compact and long range antenna design is the pseudo half wavelength wip antenna. Can be easily crafted with two 345mm long insulated wire sections wrapped with each other every 5mm, one is connected to ground and the other to the input or output pin. This design helps because of its strong ground plane, often necessary to have decent results with these modules.
