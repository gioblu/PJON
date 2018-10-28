### ESPNOW

**Medium:** 802.11 Peer-to-peer

With the `ESPNOW` PJON strategy, up to 10 ESP32 devices can use PJON to communicate with each other over
the [Espressif ESPNOW protocol](https://www.espressif.com/en/products/software/esp-now/overview) (peer-to-peer 802.11).

#### What about PJON over WiFi using GlobalUDP or LocalUDP?

PJON over ESPNOW has the benefit (over Wifi) of :-

* Lower latency (no router is required - the devices communicate directly with each other)
* Auto configuration (devices register on the group when starting up and so are accessable by device_id immediately)
* Further distance (ESPNOW claims upto 1km line of sight)

PJON over Wifi has the benefit (over ESPNOW) of :-

* Connected directly over IP network (allows easier OTA update)

#### How to use ESPNOW

Compilation tested as an esp-idf component with [arduino-esp32](https://github.com/espressif/arduino-esp32)

Pass the `ESPNOW` type as PJON template parameter to instantiate a
PJON object ready to communicate through this Strategy.

```cpp  
  // Use ESPNOW strategy with PJON device id 44
  PJON<ESPNOW> bus(44);
```

You can customise the channel (default is channel 1) and PMK (encryption) as follows (all devices must be the same):
```cpp
void setup() {

    /* note the PMK is 16 bytes, but storing it in a char requires char[17] - an extra byte for the nul terminator */
    char pmk[17] = "\x2b\xb2\x1e\x7a\x83\x13\x76\x9f\xf8\xa9\x3b\x1b\x5b\x52\xd0\x70";
    PJON<ESPNOW> bus(44);

    bus.strategy.set_channel(6);
    bus.strategy.set_pmk(pmk);
}
```

The ESPNOW strategy will send a broadcast message if the device_id is not already registered.

Once a response is received (assuming autoregistration is enabled) then the device will automatically
add the node id / mac.

Sender autoregistration is now enabled by default and can be disabled with:

```cpp
  bus.set_autoregistration(false);
```

With sender autoregistration the sender of each incoming packet is automatically registered in the
node table, meaning that you don't have to pre-register each devices' mac address.

If you disable autoregistration you can add a node as follows:

```cpp
uint8_t dev_mac[6] = {0x02, 0x23, 0x34, 0x22, 0x33, 0x44 };
bus.strategy.add_node(device_id, dev_mac);
```

#### Safety warning

In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger.
When using ESPNOW keep in mind that all connected devices must be considered potentially compromised if the PMK is known
or can be guessed, and therefore potentially manipulated or remotely actuated against your will. It should be considered
a good practice to have your own message authentication on top of the transport medium / PJON.
