
### Documentation index
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- **[Routing](/documentation/routing.md)**

---

## Routing
Transparent routing based on a tree topology has been implemented by Fred Larsen with the `PJONSimpleSwitch`, `PJONSwitch`, `PJONRouter`, `PJONDynamicRouter` and `PJONInteractiveRouter`. Those classes implement the following router roles:

### SimpleSwitch
[SimpleSwitch](/examples/ARDUINO/Local/SoftwareBitBang/Switch/SimpleSwitch) connects two buses using the same strategy (`SoftwareBitBang` <=> `SoftwareBitBang`) primarily used to amplify signals and so extend the maximum range.
```cpp
 __________             ________              __________
|          | SWBB bus  |        | SWBB bus   |          |
| DEVICE 1 |___________| SWITCH |____________| DEVICE 2 |
|__________|           |________|            |__________|
```
The `SimpleSwitch` class provides with configurable transparent packet switching between buses using the same strategy. `SimpleSwitch` requires a tree topology (it does not contain any loop detection procedure). The `SimpleSwitch` class receives as a parameter the length and the array of 2 or more `PJONBus` instances:
```cpp
/* In both cases the switch does not have an assigned id it is
   transparently switching packets in both directions */
PJONBus<SoftwareBitBang> bus1(PJON_NOT_ASSIGNED);
PJONBus<SoftwareBitBang> bus2(PJON_NOT_ASSIGNED);

// Optionally polling time can be configured:
PJONBus<SoftwareBitBang> bus2(
  PJON_NOT_ASSIGNED, // Switch device id
  1000               // Polling time in microseconds
);

// Optionally ranges can be statically configured:
PJONBus<SoftwareBitBang> bus2(
  PJON_NOT_ASSIGNED, // Switch device id
  1000,              // Polling time in microseconds
  2,                 // 2 ranges present (1-127, 128-254)
  0,                 // range 1 in use here (1-127)
);

// PJONSimpleSwitch definition:
PJONSimpleSwitch<SoftwareBitBang> router(
  2, // Length of the bus array
  (PJONBus<SoftwareBitBang>*[2]){&bus1,&bus2} // Bus array
);

// Can be optionally defined a default gateway:
PJONSimpleSwitch<SoftwareBitBang> router(
  2, // Length of the bus array
  (PJONBus<SoftwareBitBang>*[2]){&bus1,&bus2}, // Bus array
  122 // Statically defined default gateway device id
);
```
The program should only contain the following methods to provide packet switching:
```cpp

void setup() {
  // Set each SoftwareBitBang bus pin connection
  bus1.strategy.set_pin(7);
  bus2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
};
```

### Switch
[Switch](/examples/ARDUINO/Local/SoftwareBitBang/Switch/Switch) routes packets between locally attached buses also if different strategies or media are in use. It supports a default gateway to be able to act as a leaf in a larger network setup.
```cpp
 __________             ________              __________
|          | SWBB bus  |        | TS bus     |          |
| DEVICE 1 |___________| SWITCH |____________| DEVICE 2 |
|__________|           |________|            |__________|
```

### Router
[Router](/examples/ARDUINO/Network/SoftwareBitBang/Router) routes between locally attached buses also if different strategies or media are in use, and remote buses reachable through the locally attached buses.
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

### DynamicRouter
[Dynamic router](/examples/ARDUINO/Network/SoftwareBitBang/Router/DynamicRouter) is a router that also populates a routing table of remote (not directly attached) buses observing traffic.

### Virtual bus
[Virtual bus](/examples/ARDUINO/Local/SoftwareBitBang/Tunneler) is a bus where multiple buses using potentially different media or strategies, connected through a router, have the same bus id (including the local bus case), and where the location of each device is automatically registered observing traffic.

### Interactive router
[Interactive router](/examples/ARDUINO/Network/SoftwareBitBang/Switch/BlinkingSwitch) routes packets as a switch or router but it is also able to act as a device and have user-defined receive and error call-back.
