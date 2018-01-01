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
float test;
float mistakes;
int busy;
int fail;

// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(45);

int packet;
char content[] = "01234567890123456789";

void setup() {
	bus.strategy.setFrequency(868100000UL); //Obrigatory to initialize Radio with correct frequency
	bus.strategy.setSignalBandwidth(250E3); //Optional
	bus.set_error(error_handler);
	bus.begin();

	Serial.begin(115200);
	Serial.println("PJON - Network analysis");
	Serial.println("Starting a 5 second communication test...");
	Serial.println();
}

void error_handler(uint8_t code, uint8_t data) {
	if (code == PJON_CONNECTION_LOST) {
		Serial.print("Connection with device ID ");
		Serial.print(data);
		Serial.println(" is lost.");
	}
	if (code == PJON_PACKETS_BUFFER_FULL) {
		Serial.print("Packet buffer is full, has now a length of ");
		Serial.println(bus.packets[data].content[0], DEC);
		Serial.println("Possible wrong bus configuration!");
		Serial.println("higher PJON_MAX_PACKETS if necessary.");
	}
	if (code == PJON_CONTENT_TOO_LONG) {
		Serial.print("Content is too long, length: ");
		Serial.println(data);
	}
};

void loop() {
	long time = millis();
	while (millis() - time < 5000) {
		/* Here send_packet low level function is used to
		be able to catch every single sending result. */

		unsigned int response = bus.send_packet(44, content, 20);
		if (response == PJON_ACK)
			test++;
		if (response == PJON_NAK)
			mistakes++;
		if (response == PJON_BUSY)
			busy++;
		if (response == PJON_FAIL)
			fail++;
	}

	Serial.print("Absolute com speed: ");
	Serial.print((test * 26.0) / 5.0);
	Serial.println("B/s");
	Serial.print("Practical bandwidth: ");
	Serial.print((test * 20.0) / 5.0);
	Serial.println("B/s");
	Serial.print("Packets sent: ");
	Serial.println(test);
	Serial.print("Mistakes (error found with CRC) ");
	Serial.println(mistakes);
	Serial.print("Fail (no answer from receiver) ");
	Serial.println(fail);
	Serial.print("Busy (Channel is busy or affected by interference) ");
	Serial.println(busy);
	Serial.print("Accuracy: ");
	Serial.print(100 - (100 / (test / mistakes)));
	Serial.println(" %");
	Serial.println(" --------------------- ");

	test = 0;
	mistakes = 0;
	busy = 0;
	fail = 0;
};