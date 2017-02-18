- [Addressing](https://github.com/gioblu/PJON/tree/7.0/documentation/addressing.md)
- [Configuration](https://github.com/gioblu/PJON/tree/7.0/documentation/configuration.md)
- [Data reception](https://github.com/gioblu/PJON/tree/7.0/documentation/data-reception.md)
- [Data transmission](https://github.com/gioblu/PJON/tree/7.0/documentation/data-transmission.md)
- [Error handling](https://github.com/gioblu/PJON/tree/7.0/documentation/error-handling.md)
- **[IO setup](https://github.com/gioblu/PJON/tree/7.0/documentation/io-setup.md)**

The physical layer configuration is handled by the Strategy entity, if you want to communicate bi-directionally on a single medium (using SoftwareBitBang, OverSampling or AnalogSampling), sharing transmission and reception line, use:
```cpp  
 PJON<SoftwareBitBang> bus;
 bus.strategy.set_pin(12);
```

With this setup it is possible to communicate in `PJON_HALF_DUPLEX` with up to 254 different devices on the same wire.
If for some reason you need to keep separate the two lines, for example if using cheap radio transmitter and receiver modules, to which you have to connect two wires:
```cpp  
 PJON<OverSampling> bus;
 bus.strategy.set_pins(11, 12);
```

If you don't need bidirectional communication and you have only the transmitter on one side and the receiver on the other side you can use the `PJON_NOT_ASSIGNED` constant:
```cpp  
 PJON<OverSampling> bus;
 bus.strategy.set_communication_mode(PJON_SIMPLEX); // Tell PJON we want to operate in simplex mode
 bus.strategy.set_pins(11, PJON_NOT_ASSIGNED); // Only receiver
```

See other strategies readme to have additional information on their dedicated physical layer configuration.
