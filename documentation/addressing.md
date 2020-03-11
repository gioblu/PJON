
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
PJON objects can operate in local or shared mode. The PJON protocol v3.2 in [local](/specification/PJON-protocol-specification-v3.2.md#local-mode) mode supports connectivity for up to 254 devices using a 8bits device identifier, in [shared](/specification/PJON-protocol-specification-v3.2.md#shared-mode) mode supports connectivity for up to 4.294.967.295 buses (groups of devices) and up to 1.090.921.692.930 devices using a 32bits bus identifier and a 8bits device identifier.

### Local mode

The simples way to instantiate PJON in local mode is the following:
```cpp  
  PJON<SoftwareBitBang> bus;
```
When the object is instantiated without passing parameters it operates in local mode and the device identifier is set to 255 or `PJON_NOT_ASSIGNED`. PJON objects can be instantiated passing the device identifier:
```cpp
  PJON<SoftwareBitBang> bus(44);
```
`bus` receives packets for device identifier 44 and ignores all others.

### Set device identifier

Device id can be set or changed after instantiation using `set_id`:
```cpp  
  bus.set_id(44);  
```
0 or `PJON_BROADCAST` is reserved for broadcasting and should not be used as a device identifier.

### Get device identifier

The device identifier of an object can be read after instantiation using `device_id`:
```cpp  
  uint8_t id = bus.device_id(); // Get device id
```
`device_id` returns 255 or `PJON_NOT_ASSIGNED` if the instance is initialised without configuring its device identifier.

### Shared mode

if the medium used is private and not accessible from the outside world (wired network in home, business, industry) any  bus indexing scheme can be used without worrying about bus id collision; if instead the network uses a shared medium, such as commonly used radio frequencies like LoRa, it is strongly suggested to request a unique PJON bus id [here](http://www.pjon.org/get-bus-id.php) to avoid collisions.

Instantiation in shared mode:
```cpp
uint8_t bus_id[4] = {1, 2, 3, 4};
PJON<SoftwareBitBang> bus(bus_id, 44);
// Device id 44, bus id 1.2.3.4 in shared mode
```
### Get or set bus identifier

The bus id can be read and set after initialisation using `bus_id`:
```cpp  
  bus.bus_id; // Get or set bus id
```
