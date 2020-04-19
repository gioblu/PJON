
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
Transparent routing based on a tree topology has been implemented by Fred Larsen with the [PJONSimpleSwitch](#simpleswitch), [PJONSwitch](#switch), [PJONRouter](#router), [PJONDynamicRouter](#dynamicrouter), [PJONInteractiveRouter](#interactiverouter) and [PJONVirtualBusRouter](#virtual-bus).

### SimpleSwitch
The [PJONSimpleSwitch](/examples/ARDUINO/Local/SoftwareBitBang/Switch/SimpleSwitch) class connects two buses using the same strategy. In this example a `SoftwareBitBang` <=> `SoftwareBitBang` switch is created. It can be used to amplify signals and so extend the maximum range or in more complex setups selectively switch packets as requested by its configuration. It can be used instead of `PJONSwitch` to save memory when the same strategy is used in all buses. It avoids virtual inheritance so it is faster and has a smaller footprint.
```cpp
 __________             ________              __________
|          | SWBB bus  |        | SWBB bus   |          |
| DEVICE 1 |___________| SWITCH |____________| DEVICE 2 |
|__________|           |________|            |__________|
```
The first thing to do is to include the `PJONSimpleSwitch` class:
```cpp
#include <PJONSimpleSwitch.h>
```
The `SimpleSwitch` class provides with configurable transparent packet switching between buses using the same strategy. `SimpleSwitch` requires a tree topology (it does not contain any loop detection procedure). The `SimpleSwitch` class receives as a parameter the length and the array of 2 or more `PJONBus` instances:
```cpp
/* In both cases the switch does not have an assigned id it is
   transparently switching packets in both directions */
PJONBus<SoftwareBitBang> bus1(PJON_NOT_ASSIGNED);
PJONBus<SoftwareBitBang> bus2(PJON_NOT_ASSIGNED);
```
Polling time can be optionally configured:
```cpp
PJONBus<SoftwareBitBang> bus2(
  PJON_NOT_ASSIGNED, // Switch device id
  1000 // Polling time in microseconds
);
```
Device id ranges can be optionally configured:
```cpp
PJONBus<SoftwareBitBang> bus2(
  PJON_NOT_ASSIGNED, // Switch device id
  1000, // Polling time in microseconds
  2, // 2 ranges present (1-127, 128-254)
  0, // range 1 in use here (1-127)
);
```
After the `PJONBus` definitions, a `PJONSimpleSwitch` instance can be created:
```cpp
// PJONSimpleSwitch definition:
PJONSimpleSwitch<SoftwareBitBang> router(
  2, // Length of the bus array
  (PJONBus<SoftwareBitBang>*[2]){&bus1,&bus2} // Bus array
);
```
A default gateway can be optionally configured:
```cpp
PJONSimpleSwitch<SoftwareBitBang> router(
  2, // Length of the bus array
  (PJONBus<SoftwareBitBang>*[2]){&bus1,&bus2}, // Bus array
  122 // Statically defined default gateway device id
);
```
Configure each strategy and the `router` instance as required:
```cpp

void setup() {
  // Set each SoftwareBitBang bus pin connection
  bus1.strategy.set_pin(7);
  bus2.strategy.set_pin(12);
  router.begin();
}
```
Call the `loop` function as often as possible to achieve optimal performance:
```cpp

void loop() {
  router.loop();
};
```

### Switch
The [PJONSwitch](/examples/ARDUINO/Local/SoftwareBitBang/Switch/Switch) class transparently switches packets between locally attached buses also if different strategies or media are in use. It supports a default gateway to be able to act as a leaf in a larger network setup. Thanks to the `PJONSwitch` class, with few lines of code, a switch that operates multiple strategies can be created. In this example a `SoftwareBitBang` <=> `AnalogSampling` switch is created:
```cpp
/* Connect SoftwareBitBang bus with an AnalogSampling bus:

 _______     WIRED       ______    WIRELESS    _______
|       |SoftwareBitBang|      |AnalogSampling|       |
|DEVICE1|_______________|SWITCH| _ _ _ _ _ _ _|DEVICE2|
|_______|BUS ID 0.0.0.1 |______|BUS ID 0.0.0.2|_______| */
```
The first thing to do is to include the `PJONSwitch` class:
```cpp
#include <PJONSwitch.h>
```
Create `StrategyLink` instances with the selected strategies:
```cpp
StrategyLink<SoftwareBitBang> link1;
StrategyLink<AnalogSampling> link2;
```
Create `PJONAny` instances configuring the bus id:
```cpp
PJONAny bus1(&link1, (uint8_t[4]){0, 0, 0, 1});
PJONAny bus2(&link2, (uint8_t[4]){0, 0, 0, 2});
```
Polling time can be optionally configured:
```cpp
PJONAny bus1(
  &link1,
  (uint8_t[4]){0,0,0,1},
  PJON_NOT_ASSIGNED, // Switch device id
  1000 // Polling in microseconds
);
```
Device id ranges can be optionally configured:
```cpp
PJONAny bus1(
  &link1,
  (uint8_t[4]){0,0,0,1},
  PJON_NOT_ASSIGNED, // Switch device id
  1000, // Polling in microseconds
  2, // 2 ranges present (1-127, 128-254)
  0 // Range 1 in use (1-127)
);
```
Create the `PJONSwitch` instance passing the `PJONAny` instances:
```cpp
PJONSwitch router(2, (PJONAny*[2]){&bus1, &bus2});
```
Configure each strategy and the `router` instance as required:
```cpp
void setup() {
  link1.strategy.set_pin(12);
  link2.strategy.set_pin(A0);
  router.begin();
}
```
Call the `loop` function as often as possible to achieve optimal performance:
```cpp
void loop() {
  router.loop();
}
```
### Router
The [PJONRouter](/examples/ARDUINO/Network/SoftwareBitBang/Router) class routes between both locally attached buses also if different strategies or media are in use, and remote buses reachable through the locally attached buses. In this example simple a router is created:
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
The first thing to do is to include the `PJONRouter` class:
```cpp
#include <PJONRouter.h>
```
Create `StrategyLink` instances with the selected strategies:
```cpp
StrategyLink<SoftwareBitBang> link1;
StrategyLink<OverSampling> link2;
```
Create `PJONAny` instances configuring the bus id:
```cpp
PJONAny bus1(&link1, (uint8_t[4]){0, 0, 0, 3});
PJONAny bus2(&link2, (uint8_t[4]){0, 0, 0, 4});
```
Create the `PJONRouter` instance passing the `PJONAny` instances:
```cpp
PJONRouter router(2, (PJONAny*[2]){&bus1, &bus2});
```
Configure each strategy and the `router` instance as required:
```cpp
void setup() {
  link1.strategy.set_pin(7);
  link2.strategy.set_pin(12);

  router.add((const uint8_t[4]){0,0,0,1}, 0);
  router.add((const uint8_t[4]){0,0,0,2}, 1);

  router.begin();
}
```
Calling `router.add` and passing the bus id and the index of the attached bus, it is possible to configure which remote buses are reachable through locally attached buses, in the example below bus id `0.0.0.1` is configured as reachable through the local bus `0` or `link1`:
```cpp
router.add((const uint8_t[4]){0,0,0,1}, 0);
```
Call the `loop` function as often as possible to achieve optimal performance:
```cpp
void loop() {
  router.loop();
}
```
### DynamicRouter
The [PJONDynamicRouter](/examples/ARDUINO/Network/SoftwareBitBang/Router/DynamicRouter) is a router that also populates a routing table of remote (not directly attached) buses observing traffic. It can offer the same features provided by the `PJONRouter` class with no need of manual configuration. To do so, the `PJONDynamicRouter` class uses a routing table that is dynamically updated, for this reason uses more memory if compared with `PJONRouter`. Use the `PJON_ROUTER_TABLE_SIZE` constant to configure the number of entries that are `100` by default.
```cpp
                 ________
    Bus 0.0.0.3 |        | Bus 0.0.0.4
________________| ROUTER |________________
      |  Pin 7  |________|  Pin 12 |
      |                            |
      | Bus 0.0.0.1    Bus 0.0.0.2 |
 _____|____                   _____|____
|          |                 |          |
| DEVICE 1 |                 | DEVICE 2 |
|__________|                 |__________|
```
The first thing to do is to include the `PJONDynamicRouter` class:
```cpp
#include <PJONDynamicRouter.h>
```
Create `StrategyLink` instances with the selected strategies:
```cpp
StrategyLink<SoftwareBitBang> link1;
StrategyLink<OverSampling> link2;
```
Create `PJONAny` instances configuring the bus id:
```cpp
PJONAny bus1(&link1, (uint8_t[4]){0, 0, 0, 3});
PJONAny bus2(&link2, (uint8_t[4]){0, 0, 0, 4});
```
Create the `PJONRouter` instance passing the `PJONAny` instances:
```cpp
PJONDynamicRouter router(2, (PJONAny*[2]){&bus1, &bus2});
```
Configure each strategy and the `router` instance as required:
```cpp
void setup() {
  link1.strategy.set_pin(7);
  link2.strategy.set_pin(12);
  router.begin();
}
```
Unlike the `PJONRouter` class, `PJONDynamicRouter` does not need any configuration and will learn how to reach devices observing the incoming traffic. Call the `loop` function as often as possible to achieve optimal performance:
```cpp
void loop() {
  router.loop();
}
```

### InteractiveRouter
[Interactive router](/examples/ARDUINO/Network/SoftwareBitBang/Switch/BlinkingSwitch) routes packets as a switch or router but it is also able to act as a device and have user-defined receive and error call-back.

### Virtual bus
[Virtual bus](/examples/ARDUINO/Local/SoftwareBitBang/Tunneler) is a bus where multiple buses using potentially different media or strategies, connected through a router, have the same bus id (including the local bus case), and where the location of each device is automatically registered observing traffic.
