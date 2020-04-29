## ESPNOW

**Medium:** ESPNOW protocol |
**Pins used:** NA |
**Inclusion constant:** `PJON_INCLUDE_EN`

With the `ESPNOW` PJON strategy, up to 10 ESP32 devices can use PJON to communicate with each other over
the [Espressif ESPNOW protocol](https://www.espressif.com/en/products/software/esp-now/overview) (peer-to-peer 802.11).

PJON over ESPNOW has the following benefits:
* Lower latency (no router is required, devices communicate directly with each other)
* Auto configuration (devices register on the group when starting up and so are accessible immediately)
* Further distance (ESPNOW claims up to 1km line of sight)

PJON over WiFi has the following benefit:
* Connected directly over IP network (easier accessibility)

### How to use ESPNOW
ESPNOW strategy can be used within the Arduino IDE or as an esp-idf component with [arduino-esp32](https://github.com/espressif/arduino-esp32).

Pass the `ESPNOW` type as PJON template parameter to instantiate a
PJON object ready to communicate through this Strategy.

```cpp  
  // Use ESPNOW strategy with PJON device id 44
  PJON<ESPNOW> bus(44);
```

You can customise the channel (default is channel 1) and PMK (encryption) as follows (all devices must be the same):
```cpp
void setup() {
  // Note PMK is 16 bytes, to store it in a char use char[17] or add an extra byte for the null terminator
  char pmk[17] = "\x2b\xb2\x1e\x7a\x83\x13\x76\x9f\xf8\xa9\x3b\x1b\x5b\x52\xd0\x70";
  PJON<ESPNOW> bus(44);

  bus.strategy.set_channel(6);
  bus.strategy.set_pmk(pmk);
}
```

The ESPNOW strategy will send a broadcast message if the device_id is not already registered. Once a response is received (assuming auto-registration is enabled) the device automatically adds the node id and mac in its table. Sender auto-registration is enabled by default and can be disabled using the following setter:

```cpp
  bus.set_autoregistration(false);
```

With auto-registration the sender of each incoming packet is automatically registered in the node table, meaning that you don't have to pre-register each devices' mac address. If you disable auto-registration you can add a node as follows:

```cpp
uint8_t dev_mac[6] = {0x02, 0x23, 0x34, 0x22, 0x33, 0x44 };
bus.strategy.add_node(device_id, dev_mac);
```

### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger.
When using ESPNOW keep in mind that all connected devices must be considered potentially compromised if the PMK is known
or can be guessed, and therefore potentially manipulated or remotely actuated against your will. It should be considered
a good practice to implement a message authentication procedure.
