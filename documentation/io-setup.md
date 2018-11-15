
### Documentation index
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- **[IO setup](/documentation/io-setup.md)**
- [Routing](/documentation/routing.md)

## IO setup
The physical layer configuration is handled by the [strategy](/src/strategies/README.md) entity, if you want to communicate bi-directionally on a single pin with [SoftwareBitBang](/src/strategies/SoftwareBitBang) or [AnalogSampling](/src/strategies/AnalogSampling) use:
```cpp  
 PJON<SoftwareBitBang> bus;
 bus.strategy.set_pin(12);
```

With this setup it is possible to communicate in `PJON_HALF_DUPLEX` mode with up to 254 different devices on the same physical pin. If for some reason you need to keep separate the two lines, for example if using [OverSampling](/src/strategies/OverSampling) with separate radio transmitter and receiver modules, you can pass 2 separate pins:
```cpp  
 PJON<OverSampling> bus;
 bus.strategy.set_pins(11, 12);
```

If you don't need bidirectional communication and you have only the transmitter on one side and the receiver on the other side you can use the `PJON_NOT_ASSIGNED` constant:
```cpp  
 PJON<OverSampling> bus;
 // Operate in simplex mode
 bus.set_communication_mode(PJON_SIMPLEX);
  // Use only receiver pin
 bus.strategy.set_pins(11, PJON_NOT_ASSIGNED);
```

See the readme of the strategy you are using to have additional information on its dedicated physical layer configuration.
