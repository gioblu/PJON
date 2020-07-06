


#include <PJONThroughLora.h>

/* To use this example, please download the LoRa third party Library
   from https://github.com/sandeepmistry/arduino-LoRa/ */


PJONThroughLora bus(44);

void setup() {
	// Synchronous acknowledgement is not supported
	bus.set_acknowledge(false);
	// Obligatory to initialize Radio with correct frequency
	bus.strategy.setFrequency(868100000UL);
	// Optional
	bus.strategy.setSignalBandwidth(250E3);
	bus.begin();
};

void loop() {
	bus.receive();
};
