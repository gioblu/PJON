
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- **[Routing](/documentation/routing.md)**

### Routing
Transparent routing based on a tree topology has been implemented by Fred Larsen with the `PJONSimpleSwitch`, `PJONSwitch`, `PJONRouter`, `PJONDynamicRouter` and `PJONInteractiveRouter`. Those classes implement the following router roles:

- [SimpleSwitch](/examples/ARDUINO/Local/SoftwareBitBang/Switch/SimpleSwitch) connects two buses using the same strategy (`SoftwareBitBang` <=> `SoftwareBitBang`) primarily used to amplify signals and so extend the maximum range.
```cpp
 __________             ________              __________
|          | SWBB bus  |        | SWBB bus   |          |
| DEVICE 1 |___________| SWITCH |____________| DEVICE 2 |
|__________|           |________|            |__________|
```

- [Switch](/examples/ARDUINO/Local/SoftwareBitBang/Switch/Switch) routes packets between locally attached buses also if different strategies or media are in use. It supports a default gateway to be able to act as a leaf in a larger network setup.
```cpp
 __________             ________              __________
|          | SWBB bus  |        | TS bus     |          |
| DEVICE 1 |___________| SWITCH |____________| DEVICE 2 |
|__________|           |________|            |__________|
```
- [Router](/examples/ARDUINO/Network/SoftwareBitBang/Router) routes between locally attached buses also if different strategies or media are in use, and remote buses reachable through the locally attached buses.
```cpp
                 ________
    Bus 0.0.0.3 |        | Bus 0.0.0.4
________________| ROUTER |________________
      |         |________|         |
      |                            |
      | Bus 0.0.0.1    Bus 0.0.0.2 |
 _____|____                   _____|____
|          |                 |          |
| DEVICE 1 |                 | DEVICE 2 |
|__________|                 |__________|
```
- [Dynamic router](/examples/ARDUINO/Network/SoftwareBitBang/Router/DynamicRouter) is a router that also populates a routing table of remote (not directly attached) buses observing traffic.

Additional features can be added to the above classes by the classes `PJONVirtualBusRouter` and `PJONInteractiveRouter`:

- [Virtual bus](/examples/ARDUINO/Local/SoftwareBitBang/Tunneler) is a bus where multiple buses using potentially different media or strategies, connected through a router, have the same bus id (including the local bus case), and where the location of each device is automatically registered observing traffic.
- [Interactive router](/examples/ARDUINO/Network/SoftwareBitBang/Switch/BlinkingSwitch) routes packets as a switch or router but it is also able to act as a device and have user-defined receive and error call-back.
