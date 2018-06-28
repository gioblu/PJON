
#define PJON_INCLUDE_TL

#include <PJON.h>

/* To use this example, please download the LoRa third party Library
   from https://github.com/sandeepmistry/arduino-LoRa/ */

// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(45);

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

	// Obligatory to initialize Radio with correct frequency
	bus.strategy.setFrequency(868100000UL);
	// Optional
	bus.strategy.setSignalBandwidth(250E3);

	bus.set_synchronous_acknowledge(false);
	bus.set_receiver(receiver_function);

	bus.begin();

	bus.send_repeatedly(44, "B", 1, 1000000);

	Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	/* Make use of the payload before sending something, the buffer where payload points to is
	   overwritten when a new message is dispatched */
	if ((char)payload[0] == 'B') {
		digitalWrite(LED_BUILTIN, HIGH);
		delay(5);
		digitalWrite(LED_BUILTIN, LOW);
		delay(5);
	}
};

void loop() {
	bus.receive();
	bus.update();
};
