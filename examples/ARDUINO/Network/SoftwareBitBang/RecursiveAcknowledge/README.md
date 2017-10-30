## RecursiveAcknowledge example
This example implements the [recursive acknowledgement ](/specification/PJON-protocol-acknowledge-specification-v1.0.md#pjon-recursive-acknowledgement-pattern) pattern. The setup is made by 2 buses:
- `0.0.0.1` that connects device `1` to the router
- `0.0.0.2` that connects device `2` to the router

Device `1` and device `2` are indirectly connected by the router.

```cpp  
  DEVICE 1  0.0.0.1   ROUTER    0.0.0.2  DEVICE 2
  _________          _________          _________
 | ||   |_||___     |         |     ___| ||   |_||
 ||    P12||   |    ||       ||    |   ||P12    ||
 ||       ||   | P11||       ||P12 |   ||       ||
 ||       ||   |____||       ||____|   ||       ||
 |_________|        |_|_|__||_|        |_________|

- Devices must share common ground
```
When running device `2` LED should blink (pin 13) every second. If LED (pin 13) of device `1` or `2` lights up an error occurred.
If the example is run removing the router and physically connecting bus `0.0.0.1` and `0.0.0.2`, the end result is the same. The router is trasparently handling the synchronous acknowledgement and forwarding data in both directions.

The asynchronous acknowledgement used along with the synchronous acknowledgement can be useful to send information if absolute certainty of delivery and duplication avoidance is required, although it consumes more bandwidth than using only the synchronous acknowledgement. It can be a solution (also in a local bus) in critical cases like door lock control.
