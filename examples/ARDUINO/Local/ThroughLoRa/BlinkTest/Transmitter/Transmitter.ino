


#include <PJONThroughLora.h>
/*
To use this example, please download the LoRa third party Library from https://github.com/sandeepmistry/arduino-LoRa/
*/
// <Strategy name> bus(selected device id)
PJONThroughLora bus(45);

void setup() {
	// Synchronous acknowledgement is not supported
  bus.set_acknowledge(false);
	// Obligatory to initialize Radio with correct frequency
	bus.strategy.setFrequency(868100000UL);
	// Optional
	bus.strategy.setSignalBandwidth(250E3);

	bus.begin();
	bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
};

void loop() {
	bus.update();
};
