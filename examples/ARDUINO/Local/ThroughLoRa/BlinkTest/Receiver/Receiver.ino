
#define PJON_INCLUDE_TL

#include <PJON.h>

/* To use this example, please download the LoRa third party Library from
https://github.com/sandeepmistry/arduino-LoRa/ */

// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(44);

void setup() {
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW); // Initialize LED 13 to be off

	// Obligatory to initialize Radio with correct frequency
	bus.strategy.setFrequency(868100000UL);
	// Optional
	bus.strategy.setSignalBandwidth(250E3);
	bus.begin();

	bus.set_receiver(receiver_function);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	/* Make use of the payload before sending something, the buffer where payload points to is
	   overwritten when a new message is dispatched */
	if (payload[0] == 'B') {
		digitalWrite(13, HIGH);
		delay(30);
		digitalWrite(13, LOW);
	}
};

void loop() {
	bus.receive(1000);
};
