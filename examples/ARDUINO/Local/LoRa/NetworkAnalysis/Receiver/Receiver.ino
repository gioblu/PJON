#define PJON_INCLUDE_TL
// Uncomment to run SoftwareBitBang to mode FAST
// #define SWBB_MODE 2
// Uncomment to run SoftwareBitBang to mode OVERDRIVE
// #define SWBB_MODE 3

/*  Acknowledge Latency maximum duration (1000 microseconds default).
	Can be necessary to higher SWBB_RESPONSE_TIMEOUT to leave enough time to
	receiver to compute the CRC and to respond with a synchronous acknowledgement
	SWBB_RESPONSE_TIMEOUT can be reduced to higher communication speed if
	devices are near and able to compute CRC fast enough. */
	//#define SWBB_RESPONSE_TIMEOUT 1000

#include <PJON.h>
/*
To use this example, please download the LoRa third party Library from https://github.com/sandeepmistry/arduino-LoRa/
*/
// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(44);

void setup() {
	bus.strategy.setFrequency(868100000UL); //Obrigatory to initialize Radio with correct frequency
	bus.strategy.setSignalBandwidth(250E3); //Optional
	bus.begin();
};

void loop() {
	bus.receive();
};