
### Documentation index
- **[Addressing](/documentation/addressing.md)**
  - [`device_id`](/documentation/addressing.md#local-mode)
  - [`get_bus_id`](/documentation/addressing.md#shared-mode)
  - [`get_mac`](/documentation/addressing.md#get-or-set-hardware-identifier)
  - [`set_bus_id`](/documentation/addressing.md#shared-mode)
  - [`set_id`](/documentation/addressing.md#local-mode)
  - [`set_mac`](/documentation/addressing.md#get-or-set-hardware-identifier)
- [Configuration](/documentation/configuration.md)
  - [`include_mac`](/documentation/configuration.md#hardware-identification)
  - [`include_port`](/documentation/configuration.md#network-service-identification)
  - [`include_sender_info`](/documentation/configuration.md#sender-information)
  - [`set_acknowledge`](/documentation/configuration.md#acknowledgement)
  - [`set_communication_mode`](/documentation/configuration.md#communication-mode)
  - [`set_crc_32`](/documentation/configuration.md#crc-configuration)
  - [`set_packet_auto_deletion`](/documentation/configuration.md#packet-handling)
  - [`set_packet_id`](/documentation/configuration.md#packet-identification)
  - [`set_router`](/documentation/configuration.md#router-mode)
  - [`set_shared_network`](/documentation/configuration.md#network-mode)
- [Data reception](/documentation/data-reception.md)
  - [`receive`](/documentation/data-reception.md#data-reception)
  - [`set_receiver`](/documentation/data-reception.md#data-reception)
- [Data structures](/documentation/data-structures.md)
  - [`PJON_Endpoint`](/documentation/data-structures.md#pjon_endpoint)
  - [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info)
- [Data transmission](/documentation/data-transmission.md)
  - [`begin`](/documentation/data-transmission.md#begin)
  - [`reply`](/documentation/data-transmission.md#reply)
  - [`reply_blocking`](/documentation/data-transmission.md#reply_blocking)
  - [`send`](/documentation/data-transmission.md#send)
  - [`send_packet`](/documentation/data-transmission.md#send_packet)
  - [`send_packet_blocking`](/documentation/data-transmission.md#send_packet_blocking)
  - [`send_repeatedly`](/documentation/data-transmission.md#send_repeatedly)
- [Error handling](/documentation/error-handling.md)
  - [`set_error`](/documentation/error-handling.md#error-handling)
- [Routing](/documentation/routing.md)
  - [`PJONSimpleSwitch`](/documentation/routing.md#simpleswitch)
  - [`PJONSwitch`](/documentation/routing.md#switch)
  - [`PJONRouter`](/documentation/routing.md#router)
  - [`PJONDynamicRouter`](/documentation/routing.md#dynamicrouter)
  - [`PJONInteractiveRouter`](/documentation/routing.md#interactiverouter)
- [IO setup](/documentation/io-setup.md)
   - [`strategy`](/documentation/io-setup.md#io-setup)

---

## Addressing
PJON objects can operate in local or shared mode. The PJON protocol v4.0 in [local](/specification/PJON-protocol-specification-v4.0.md#local-mode) mode supports connectivity for up to 254 devices using a 8bits device identifier, in [shared](/specification/PJON-protocol-specification-v4.0.md#shared-mode) mode supports connectivity for up to 4.294.967.295 buses (groups of devices) and up to 1.090.921.692.930 devices using a 32bits bus identifier and a 8bits device identifier.

### Local mode

The simples way to instantiate PJON in local mode is the following:
```cpp  
  PJONSoftwareBitBang bus;
```
When the object is instantiated without passing parameters it operates in local mode and the device identifier is set to 255 or `PJON_NOT_ASSIGNED`. PJON objects can be instantiated passing the device identifier:
```cpp
  PJONSoftwareBitBang bus(44);
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
`device_id` returns `PJON_NOT_ASSIGNED` or 255 if the instance is initialised without configuring its device identifier.

### Shared mode

if the medium used is private and not accessible from the outside world (wired network in home, business, industry) bus ids can be used arbitrarily without any risk of collision; if instead the network uses a shared medium, for example on unlicensed radio frequencies with [ThroughLoRa](/src/strategies/ThroughLoRa), it is strongly suggested to request a unique PJON bus id [here](http://www.pjon.org/get-bus-id.php) to avoid collisions.

Instantiation in shared mode:
```cpp
uint8_t bus_id[4] = {1, 2, 3, 4};
PJONSoftwareBitBang bus(bus_id, 44);
// Device id 44, bus id 1.2.3.4 in shared mode
```
### Get or set bus identifier

The bus id can be read and set after initialisation using `bus_id`:
```cpp  
  bus.tx.bus_id; // Get or set bus id
```

### Hardware identifier

PJON can optionally operate using the MAC address of the device:
```cpp
// Include MAC address feature
#define PJON_INCLUDE_MAC

// MAC address of the device
uint8_t mac[6] = {1, 2, 3, 4, 5, 6};

PJONSoftwareBitBang bus(mac);
// Local mode, device id PJON_NOT_ASSIGNED
```
This instantiation sets the MAC address, the device id set to `PJON_NOT_ASSIGNED` or 255 but can be changed afterwards as required. Packets containing a recipient's MAC address that is not equal to the one configured are discarded. PJON can operate in both local and shared mode while including MAC addresses. The feature can be disabled using `includ_mac`:

```cpp
bus.include_mac(false);
```
### Get or set hardware identifier

The MAC address can be read and set after initialisation using `tx.mac`:
```cpp  
  bus.tx.mac; // Get or set MAC
```
