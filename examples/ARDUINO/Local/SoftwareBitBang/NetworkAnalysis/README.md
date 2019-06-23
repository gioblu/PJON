## NetworkAnalysis example
This example test the communication reliability between 2 devices using the `SoftwareBitBang` strategy. Two Arduino compatible devices are connected sharing ground and a single wire used for communication connected to pin `12`. `Transmitter` device sends packets at maximum speed keeping track of results. After 1 second results are printed in the Serial monitor. `Receiver` device is programmed to simply receive data continuously.

```cpp  
 Arduino UNO        Arduino UNO
  _________   wire   _________
 | ||   |_||___     |         |
 ||       ||   |    ||       ||
 ||       ||   |    ||       ||
 ||       ||   |____||       ||
 |_________|        |_|_|__||_|

- Devices must share common ground
```

The test result can be evaluated thanks to the following terms:
* `Packet overhead` or how many bytes are added by PJON to data
* `Bandwidth` or maximum amount of bytes transmitted in a second
* `Data throughput` or how many data bytes are transmitted every second
* `Packets sent` or number of packets transmitted in the test window
* `Mistakes` or how many errors detected with CRC
* `Fail` or number of transmission failures occurred in the test window
* `Busy` or how many times the channel is found busy
* `Accuracy` or ratio between correct packets and packets that contain mistakes

The result can be interpreted as follows:
- Many `Fail` and or `Mistakes` may mean that the maximum communication range between devices has been reached
- Many `Busy` and or `Mistakes` and or low `Bandwidth` may mean that interference is present, see [mitigate interference](https://github.com/gioblu/PJON/wiki/Mitigate-interference)
- Many `Fail` and or `Mistakes` and or low `Bandwidth` may indicate a bad timing configuration, if you are porting a new MCU/architecture to [SoftwareBitBang](src/strategies/SoftwareBitBang) consider that a different timing configuration may be required because of system discrepancies. Try tweaking `SWBB_BIT_WIDTH`, `SWBB_BIT_SPACER`, `SWBB_READ_DELAY` and `SWBB_ACCEPTANCE` in [src/strategies/SoftwareBitBang/Timing.h](src/strategies/SoftwareBitBang/Timing.h)
- Low performance also after painstaking timing tweaks may indicate that the new MCU/architecture may not be fast enough to run SoftwareBitBang at the mode you are working with, try using a faster clock or optimize digital I/O perfomance or choosing a slower mode.
