- **[Addressing](/documentation/addressing.md)**
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)

PJON can be instantiated passing a predefined device id and optionally a bus id, if the network is private and not accessible from the outside world (wired network in home, business, industry) any sort of bus indexing can be used without worrying about bus id collision; if instead the network is connected to a shared medium, where other PJON users could transmit and receive data, it is strongly suggested to request a unique PJON bus id [here](http://www.pjon.org/get-bus-id.php) for your group of devices.
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
