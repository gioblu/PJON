
### Documentation index
- **[Addressing](/documentation/addressing.md)**
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

---

## Addressing
PJON objects can operate in local or shared mode. The PJON protocol v3.1 in [local](/specification/PJON-protocol-specification-v3.1.md#local-mode) mode supports connectivity for up to 254 devices using a 1 byte device identifier, in [shared](/specification/PJON-protocol-specification-v3.1.md#shared-mode) mode supports connectivity for up to 4.294.967.295 buses (groups of devices) and up to 1.090.921.692.930 devices using an additional 4 bytes bus identifier.

Instantiation in local mode:
```cpp  
  PJON<SoftwareBitBang> bus;
  // Device id PJON_NOT_ASSIGNED in local mode

  PJON<SoftwareBitBang> bus(44);
  // Device id 44 in local mode
```
Instantiation in shared mode:
```cpp
uint8_t bus_id[4] = {1, 2, 3, 4};
PJON<SoftwareBitBang> bus(bus_id, 44);
// Device id 44, bus id 1.2.3.4 in shared mode
```
if the network is private and not accessible from the outside world (wired network in home, business, industry) any sort of bus indexing can be used without worrying about bus id collision; if instead the network is connected to a shared medium where other PJON users could transmit and receive data, it is strongly suggested to request a unique PJON bus id [here](http://www.pjon.org/get-bus-id.php).

Device id can also be set afterwards using `set_id`:
```cpp  
  bus.set_id(44);  
```
Device and bus id can be read using `device_id` or `bus_id`:
```cpp  
  bus.device_id(); // Get device id
  bus.bus_id;      // Get or set bus id
```
