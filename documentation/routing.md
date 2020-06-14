
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
The first thing to do is to include `PJONSimpleSwitch` and the required strategy:
```cpp
#include <PJONSimpleSwitch.h>
#include <PJONSoftwareBitBang.h>
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
Then the `PJONSimpleSwitch` should work transparently. `PJONSimpleSwitch` can be used also in local mode, although, because the hop count field is not included, the network topology cannot include loops.

### Switch
[PJONSwitch](/examples/routing/ARDUINO/Network/Switch/Switch) transparently switches packets between locally attached buses also if different strategies or media are in use. It supports a default gateway to be able to act as a leaf in a larger network setup. Thanks to the `PJONSwitch` class, with few lines of code, a switch that operates multiple strategies can be created. In this example a `SoftwareBitBang` <=> `AnalogSampling` switch is created:
```cpp
/* Connect SoftwareBitBang bus with an AnalogSampling bus:

 _______     WIRED       ______    WIRELESS    _______
|       |SoftwareBitBang|      |AnalogSampling|       |
|DEVICE1|_______________|SWITCH| _ _ _ _ _ _ _|DEVICE2|
|_______|BUS ID 0.0.0.1 |______|BUS ID 0.0.0.2|_______| */
```
First include the `PJONSwitch` class and the strategies used:
```cpp
#include <PJONSwitch.h>
#include <PJONSoftwareBitBang.h>
#include <PJONAnalogSampling.h>
```
The simplest way to use the `PJONSwitch` class is to use `PJONSwitch2` that is able to handle up to 2 buses:
```cpp
PJONSwitch2<SoftwareBitBang, AnalogSampling> router;
```
Use `get_strategy_0` and `get_strategy_1` to access one of the two strategies:
```cpp
router.get_strategy_0().set_pin(12); // SoftwareBitBang pin used is 12
router.get_strategy_1().set_pin(A0); // AnalogSampling pin used is A0
```
Use `get_bus` to access one of the two instances:
```cpp
router.get_bus(0).set_bus_id((const uint8_t[4]){0, 0, 0, 1});
router.get_bus(1).set_bus_id((const uint8_t[4]){0, 0, 0, 2});
```
Call the `loop` function as often as possible to achieve optimal performance:
```cpp
void loop() {
  router.loop();
}
```

Consider that exists also `PJONSwitch3` able to handle up to 3 buses, and `PJONSwitch` able to handle an array of buses. `PJONSwitch` can be used also in local mode, although, because the hop count field is not included, the network topology cannot include loops.

### Router
The [PJONRouter](/examples/routing/ARDUINO/Network/Router/Router) class routes between both locally attached buses also if different strategies or media are in use, and remote buses reachable through the locally attached buses. In this example simple a router is created:
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
The first thing to do is to include the `PJONRouter` class and include the required strategy:
```cpp
#include <PJONRouter.h>
#include <PJONSoftwareBitBang.h>
```
The simplest way to use the `PJONRouter` class is to use `PJONRouter2` that is able to handle up to 2 buses:
```cpp
PJONRouter2<SoftwareBitBang, SoftwareBitBang> router;
```
Configure each strategy and the `router` instance as required:
```cpp
void setup() {
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_pin(12);
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
Consider that exists also `PJONRouter3` able to handle up to 3 buses, and `PJONRouter` able to handle an array of buses. `PJONRouter` can be used also in local mode, although, because the hop count field is not included, the network topology cannot include loops.

### DynamicRouter
The [PJONDynamicRouter](/examples/routing/ARDUINO/Network/DynamicRouter/DynamicRouter.ino) is a router that also populates a routing table of remote (not directly attached) buses observing traffic. It can offer the same features provided by the `PJONRouter` class with no need of manual configuration. To do so, the `PJONDynamicRouter` class uses a routing table that is dynamically updated, for this reason uses more memory if compared with `PJONRouter`. Use the `PJON_ROUTER_TABLE_SIZE` constant to configure the number of entries that are `100` by default.
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
The first thing to do is to include the `PJONDynamicRouter` class and the required strategies:
```cpp
#include <PJONDynamicRouter.h>
#include <PJONSoftwareBitBang.h>
#include <PJONOverSampling.h>
```
The simplest way to use the `PJONDynamicRouter` class is to use `PJONDynamicRouter2` that is able to handle up to 2 buses:
```cpp
PJONDynamicRouter2<SoftwareBitBang, SoftwareBitBang> router;
```
Configure each strategy and the `router` instance as required:
```cpp
void setup() {
  router.get_strategy_0().set_pin(7);
  router.get_strategy_1().set_pin(12);
  router.begin();
}
```
Unlike the `PJONRouter` class, `PJONDynamicRouter` does not need any configuration and will learn how to reach devices observing the incoming traffic. Call the `loop` function as often as possible to achieve optimal performance:
```cpp
void loop() {
  router.loop();
}
```

Consider that exists also `PJONDynamicRouter3` able to handle up to 3 buses, and `PJONDynamicRouter` able to handle an array of buses. `PJONDynamicRouter` can be used also in local mode, although, because the hop count field is not included, the network topology cannot include loops.

### InteractiveRouter
[Interactive router](/examples/routing/ARDUINO/Network/Switch/BlinkingSwitch) routes packets as a switch or router but it is also able to act as a device and have user-defined receive and error call-back.

### Virtual bus
[Virtual bus](/examples/routing/ARDUINO/Local/Tunneler) is a bus where multiple buses using potentially different media or strategies, connected through a router, have the same bus id (including the local bus case), and where the location of each device is automatically registered observing traffic.
