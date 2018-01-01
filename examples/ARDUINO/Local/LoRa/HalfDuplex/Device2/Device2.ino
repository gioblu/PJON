#include <PJON.h>
/*
To use this example, please download the LoRa third party Library from https://github.com/sandeepmistry/arduino-LoRa/
*/
// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(45);

void setup() {
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW); // Initialize LED 13 to be off
	bus.set_error(error_handler);

	bus.set_receiver(receiver_function);

	bus.strategy.setFrequency(868100000UL); //Obrigatory to initialize Radio with correct frequency
	bus.strategy.setSignalBandwidth(250E3); //Optional

	bus.begin();

	bus.send(44, "B", 1);

	Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	/* Make use of the payload before sending something, the buffer where payload points to is
	   overwritten when a new message is dispatched */
	if ((char)payload[0] == 'B') {
		bus.reply("B", 1);
		digitalWrite(13, HIGH);
		delay(5);
		digitalWrite(13, LOW);
	}
}

void error_handler(uint8_t code, uint8_t data) {
	if (code == PJON_CONNECTION_LOST) {
		Serial.print("Connection with device ID ");
		Serial.print(bus.packets[data].content[0], DEC);
		Serial.println(" is lost.");
	}
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

void loop() {
	bus.receive(20000);
	bus.update();
};