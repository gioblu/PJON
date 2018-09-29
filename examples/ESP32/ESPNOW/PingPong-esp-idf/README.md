# ESP32 ESPNOW PingPong example

This setup consists of a Transmitter and a Receiver. The Transmitter will regularly send a small packet to the Receiver, and the Receiver will reply back every time.

## How to build

Follow the [arduino-esp32](https://github.com/espressif/arduino-esp32/blob/master/docs/esp-idf_component.md)
instructions for esp-idf components

Then add PJON under your components path. See component.mk in Receiver/Transmitter dirs for how the PJON source is
included.
