#define OS_GAIN_REFRESH_DELAY 0
/* Gain refresh time of SRX882 module is around 100 milliseconds.
   If only one pair of SRX and STX are used to connect 2 devices in SIMPLEX
   mode, there is no need to refresh receiver's gain, being communication
   mono-directional. */

#include <PJON.h>
/*
To use this example, please download the LoRa third party Library from https://github.com/sandeepmistry/arduino-LoRa/
*/
float test;
float mistakes;
int busy;
int fail;

// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(44);

void setup() {
	bus.set_communication_mode(PJON_SIMPLEX);
	bus.strategy.setFrequency(868100000UL); //Obrigatory to initialize Radio with correct frequency
	bus.strategy.setSignalBandwidth(250E3); //Optional
	bus.begin();

	bus.set_receiver(receiver_function);

	Serial.begin(115200);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	// Do nothing to avoid affecting speed analysis
}

void loop() {
	Serial.println("Starting 1 second communication speed test...");
	long time = millis();
	unsigned int response = 0;
	while (millis() - time < 1000) {
		response = bus.receive();
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
	Serial.print(test * 25);
	Serial.println("B/s");
	Serial.print("Practical bandwidth: ");
	Serial.print(test * 20);
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