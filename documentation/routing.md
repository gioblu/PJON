
### Documentation index
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data structures](/documentation/data-structures.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- **[Routing](/documentation/routing.md)**

---

## Routing
Transparent routing based on a tree topology that may include loops has been implemented by Fred Larsen with the [PJONSimpleSwitch](#simpleswitch), [PJONSwitch](#switch), [PJONRouter](#router), [PJONDynamicRouter](#dynamicrouter), [PJONInteractiveRouter](#interactiverouter) and [PJONVirtualBusRouter](#virtual-bus).

### SimpleSwitch
The [PJONSimpleSwitch](/examples/routing/ARDUINO/Network/Switch/SimpleSwitch/SimpleSwitch.ino) class connects two or more buses using the same strategy. In this example a `SoftwareBitBang` <=> `SoftwareBitBang` switch is created. It can be used to amplify signals and so extend the maximum range or in more complex setups selectively switch packets as requested by its configuration. It can be used instead of `PJONSwitch` to save memory when the same strategy is used in all buses. It avoids virtual inheritance so it is faster and has a smaller footprint.
```cpp
/* __________                 ________                 __________
  |          |         Pin 7 |        | Pin 12        |          |
  | DEVICE 1 |_______________| SWITCH |_______________| DEVICE 2 |
  |__________|  Bus 0.0.0.1  |________|  Bus 0.0.0.2  |__________| */

```
The first thing to do is to include the `PJONSimpleSwitch` class:
```cpp
#include <PJONSimpleSwitch.h>
```
The `SimpleSwitch` class provides with configurable transparent packet switching between buses using the same strategy:
```cpp
PJON<SoftwareBitBang> bus1((const uint8_t[4]){0, 0, 0, 1}, PJON_NOT_ASSIGNED),
                      bus2((const uint8_t[4]){0, 0, 0, 2}, PJON_NOT_ASSIGNED);

PJONSimpleSwitch<SoftwareBitBang> router(bus1, bus2);

```
In the example above two PJON instances using SoftwareBitBang, operating in shared mode, with bus id `0.0.0.1` and `0.0.0.2` are merged with the `SimpleSwitch` instance. Packets are switched between the two buses. In the `setup` just include strategy related configuration, for example the pin used for communication:

```cpp

void setup() {
  bus1.strategy.set_pin(7);
  bus2.strategy.set_pin(12);
  router.begin();
}

void loop() {
  router.loop();
}
```
Then the `PJONSimpleSwitch` should work transparently. `PJONSimpleSwitch` can be used also in local mode although, because the hop count field is not included, the network topology cannot include loops.

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
