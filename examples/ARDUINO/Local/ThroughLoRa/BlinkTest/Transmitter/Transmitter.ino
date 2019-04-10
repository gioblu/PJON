
#define PJON_INCLUDE_TL

#include <PJON.h>
/*
To use this example, please download the LoRa third party Library from https://github.com/sandeepmistry/arduino-LoRa/
*/
// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(45);

void setup() {

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
