## NetworkAnalysis example
This example test the communication reliability between 2 devices using the `SoftwareBitBang` strategy. Two Arduino compatible devices are connected sharing ground and a single wire used for communication connected to pin `12`. `Transmitter` device sends packets at maximum speed keeping track of results. After 1 second results are printed in the Serial monitor. `Receiver` device is programmed to simply receive data continuously.

### Result
- **Packet overhead** or additional data added to handle transmission
- **Absolute communication speed** or available bandwidth
- **Data throughput** or `available bandwidth - packet overhead`  
- **Accuracy** or `packets sent / packets received with mistakes` ratio
- **Mistakes** or errors detected with CRC
- **Fail** or how many times the `receive` function ended without detecting a frame
- **Busy** How many times the medium is detected busy

### Interpreting results
Thanks to this sketch it is possible to test the effects of `PJON` and `SoftwareBitBang` configuration on communication speed and performance. It is also possible to use this sketch to test the signal-to-noise ratio or SNR of an existing network.

- If often **busy**, you may need to add a 1-5MΩ pull-down resistor to reduce externally induced interference on the bus.
- If **mistakes** and or **fail** are many, you are experiencing timing inconsistency, the devices you are testing may not have already a dedicated set of timing in `Timing.h`. Feel free to open an [issue}(https://github.com/gioblu/PJON/issues) here.
