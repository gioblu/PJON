
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
PJON objects can operate in local or shared mode. The PJON protocol v3.0 in [local](/specification/PJON-protocol-specification-v3.0.md#local-mode) mode supports connectivity for up to 254 devices using a 1 byte device identifier, in [shared](/specification/PJON-protocol-specification-v3.0.md#shared-mode) mode supports connectivity for up to 4.294.967.295 buses (groups of devices) and up to 1.090.921.692.930 devices using an additional 4 bytes bus identifier.

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

### Dynamic addressing
`PJONMaster` and `PJONSlave` classes implement the master-slave and multi-master [dynamic addressing](/specification/PJON-dynamic-addressing-specification-v2.0.md) features. Slaves can be connected to a bus and can be hot-swapped, their id is automatically assigned by master and their presence can be optionally monitored. Master keeps an updated list of known dynamically addressed slaves. Use the `PJON` class if those features are not required.   

### PJONMaster
The `PJONMaster` class implement master's dynamic addressing procedure which is totally automatic a requires no configuration, although some optional configuration is available.
```cpp
// Optionally configurable maximum amount of slaves handled by master
#define PJON_MAX_DEVICES 50

#include <PJONMaster.h>          // Include PJONMaster class
uint8_t bus_id[] = {0, 0, 0, 1}; // Bus id definition

// PJON object - Master device id is PJON_MASTER_ID or 254
PJONMaster<SoftwareBitBang> bus(bus_id);
```
If addressing procedure debug is needed set the state of `bus.debug` accordingly:
```cpp
bus.debug = true;
```
Slave presence check can be requested manually calling `check_slaves_presence`. This is a bandwidth consuming procedure because master contacts each known slave to determine if still online. If a slave is found unresponsive it is removed from the list of known slaves. The more devices are connected the more bandwidth is consumed. Call this function seldom (every 5-10 seconds) to avoid bandwidth saturation.
```cpp
// Check if registered slaves are still present on the bus
bus.check_slaves_presence();
```
This is the list of the addressing errors possibly returned by the error call-back:

- `PJON_ID_ACQUISITION_FAIL` (value 105), `data` parameter contains lost packet's id.
- `PJON_DEVICES_BUFFER_FULL` (value 254), `data` parameter contains slaves buffer length.

This is an example of how an error call-back can be defined:
```cpp

void setup() {
  bus.set_error(error_function);
}

void error_function(uint8_t code, uint8_t data, void *custom_pointer) {
  // Standard PJON error
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  // PJONMaster related errors
  if(code == PJON_ID_ACQUISITION_FAIL) {
    Serial.print("Connection lost with device ");
    Serial.println(data, DEC);
  }
  if(code == PJON_DEVICES_BUFFER_FULL) {
    Serial.print("Master devices buffer is full with a length of ");
    Serial.println(data);
  }
};
```

### PJONSlave
Use the `PJONSlave` class for slaves:
```cpp
#include <PJONSlave.h>
// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};
// PJON object
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED);
```
Call `acquire_id_master_slave()` to acquire an id assigned by master:
```cpp
bus.acquire_id_master_slave();
```
Call `acquire_id_multi_master()` to acquire an id autonomously without the need of a master:
```cpp
bus.acquire_id_multi_master();
```
This is the list of the addressing errors possibly returned by the error call-back:
- `PJON_ID_ACQUISITION_FAIL` (value 105), `data` parameter contains the failed request

Requests list:
- `PJON_ID_ACQUIRE` (value 199), multi-master id acquisition
- `PJON_ID_REQUEST` (value 200), master-slave id request
- `PJON_ID_CONFIRM` (value 201), master-slave id confirmation
- `PJON_ID_NEGATE`  (value 203), master-slave id release

This is an example of how an error call-back can be defined:
```cpp
void setup() {
  bus.set_error(error_function);
}

void error_handler(uint8_t code, uint8_t data, void *custom_pointer) {
  // Standard PJON error
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  // PJONSlave related errors
  if(code == PJON_ID_ACQUISITION_FAIL) {
    if(data == PJON_ID_ACQUIRE)
      Serial.println("Multi-master auto addressing procedure failed.");
    if(data == PJON_ID_CONFIRM)
      Serial.println("Master-slave id confirmation procedure failed.");
    if(data == PJON_ID_NEGATE)
      Serial.println("Master-slave id release procedure failed.");
    if(data == PJON_ID_REQUEST)
      Serial.println("Master-slave id request procedure failed.");
  }
};
```

See the [MasterSlaveAutoAddressing](../examples/ARDUINO/Network/SoftwareBitBang/MasterSlaveAutoAddressing) example for a working showcase.
