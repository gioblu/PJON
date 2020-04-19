
#define PJON_INCLUDE_TL

#include <PJON.h>

/* To use this example, please download the LoRa third party Library from
   https://github.com/sandeepmistry/arduino-LoRa/ */

// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(45);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	/* Make use of the payload before sending something, the buffer where payload points to is
	   overwritten when a new message is dispatched */
	if ((char)payload[0] == 'B') {
		bus.reply("B", 1);
		digitalWrite(LED_BUILTIN, HIGH);
		delay(5);
		digitalWrite(LED_BUILTIN, LOW);
	}
}

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
	if (code == PJON_PACKETS_BUFFER_FULL) {
		Serial.print("Packet buffer is full, has now a length of ");
		Serial.println(data, DEC);
		Serial.println("Possible wrong bus configuration!");
		Serial.println("higher PJON_MAX_PACKETS if necessary.");
	}
	if (code == PJON_CONTENT_TOO_LONG) {
		Serial.print("Content is too long, length: ");
		Serial.println(data);
	}
};

void setup() {
	pinMode(LED_BUILTIN, OUTPUT);
	digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

	bus.set_error(error_handler);
	bus.set_receiver(receiver_function);
	// Synchronous acknowledgement is not supported
	bus.set_synchronous_acknowledge(false);
	// Obligatory to initialize Radio with correct frequency
	bus.strategy.setFrequency(868100000UL);
	// Optional
	bus.strategy.setSignalBandwidth(250E3);

	bus.begin();
	bus.send(44, "B", 1);
	Serial.begin(115200);
};

void loop() {
	bus.receive(20000);
	bus.update();
};
