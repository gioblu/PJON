
The `Any` strategy includes virtual inheritance and let a PJON object switch from a strategy to another when required or a collection of PJON objects with different strategies to be treated dynamically.  

#### Why Any strategy?
This strategy is required For example if you need to handle an array of PJON objects that are using different strategies or if you need to use a single PJON instance using more than one strategy.

#### How to use Any
Define a `StrategyLink` template class passing the desired strategy, then pass the type `Any` as PJON template parameter to instantiate a PJON object ready to communicate using this strategy.
```cpp  
StrategyLink<SoftwareBitBang> link;
PJON<Any> bus;
```
Call the `set_link` method passing the `StrategyLink` instance:
```cpp  
#include <PJON.h>

StrategyLink<SoftwareBitBang> link;
PJON<Any> bus;

void setup() {
  Serial.begin(9600);
  bus.strategy.set_link(&link);
}
```

See [MultiStrategyLink](../../examples/ARDUINO/Local/Any/MultiStrategyLink) and [StrategyLinkNetworkAnalysis](../../examples/ARDUINO/Local/Any/StrategyLinkNetworkAnalysis) examples.

All the other necessary information is present in the general [Documentation](/documentation).
