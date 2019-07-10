
#define PJON_INCLUDE_TL

#include <PJON.h>

/* To use this example, please download the LoRa third party Library
   from https://github.com/sandeepmistry/arduino-LoRa/ */

float test;
float mistakes;
int busy;
int fail;

// <Strategy name> bus(selected device id)
PJON<ThroughLora> bus(45);

int packet;
uint8_t content[] = "01234567890123456789";

void setup() {
	bus.set_communication_mode(PJON_SIMPLEX);
	bus.strategy.setFrequency(868100000UL); //Obrigatory to initialize Radio with correct frequency
	bus.strategy.setSignalBandwidth(250E3); //Optional

	bus.begin();
	packet = bus.send(44, content, 20);

	Serial.begin(115200);
	Serial.println("PJON - Network analysis");
	Serial.println("Starting a 1 second communication test..");
	Serial.println();
}

void loop() {
	if(!bus.update())
		packet = bus.send(44, content, 20);
};
