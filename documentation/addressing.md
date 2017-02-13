- **[Addressing](https://github.com/gioblu/PJON/tree/6.2/documentation/addressing.md)**
- [Configuration](https://github.com/gioblu/PJON/tree/6.2/documentation/configuration.md)
- [Data reception](https://github.com/gioblu/PJON/tree/6.2/documentation/data-reception.md)
- [Data transmission](https://github.com/gioblu/PJON/tree/6.2/documentation/data-transmission.md)
- [Error handling](https://github.com/gioblu/PJON/tree/6.2/documentation/error-handling.md)
- [IO setup](https://github.com/gioblu/PJON/tree/6.2/documentation/io-setup.md)

The PJON object can be instanced passing the predefined device id, or without passing any parameter relying on the [dynamic addressing procedure](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md). If your installation is private and not accessible from outside (wired network in business, home, industry) you can use any sort of bus indexing without worrying about bus id collision. If PJON is used on a shared medium, where other PJON users could network we strongly suggest users to register for an official PJON bus id to [get a unique bus id](http://www.pjon.org/get-bus-id.php) for your group of devices.
```cpp  
  PJON<SoftwareBitBang> bus;
  // PJON device with PJON_NOT_ASSIGNED id in a local mode

  PJON<SoftwareBitBang> bus(44);
  // PJON device with id 44 in a local mode

  uint8_t bus_id[4] = {1, 2, 3, 4};
  PJON<SoftwareBitBang> bus(bus_id, 44);
  // PJON device id 44 in bus id 1.2.3.4 in a shared mode
```
Device id can also be set afterwards through a setter:
```cpp  
  bus.set_id(44);  
```
Can be read through a getter:
```cpp  
  bus.device_id(); // Get device id
  bus.bus_id;      // Get bus id
```
