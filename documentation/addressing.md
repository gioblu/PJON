- **[Addressing](/documentation/addressing.md)**
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)

PJON can be instantiated passing a predefined device id and optionally a bus id, if your application is private and not accessible from the outside world (wired network in business, home, industry) you can use any sort of bus indexing without worrying about bus id collision. If PJON is used on a shared medium, where other PJON users could network we strongly suggest users to register for an official PJON bus id to [get a unique bus id](http://www.pjon.org/get-bus-id.php) for your group of devices.
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
