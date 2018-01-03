#define PJON_INCLUDE_TL
#include <SPI.h>
#include <LoRa.h>
#include <PJON.h>
#include <ArduinoJson.h>
/*
To use this example, please download the LoRa third party Library from https://github.com/sandeepmistry/arduino-LoRa/
and JSON library from https://github.com/bblanchon/ArduinoJson
*/
#define EVENT_TIME	1000 //1 message every 1 second

PJON<ThroughLora> LoraPJON(45);

void setup() {
	Serial.begin(115200);
	Serial.println("LoRa Transmitter");

	LoraPJON.strategy.setFrequency(868100000UL); //Obrigatory to initialize Radio with correct frequency
	LoraPJON.strategy.setSignalBandwidth(250E3); //Optional

	LoraPJON.begin();
	//LoraPJON.set_communication_mode(PJON_SIMPLEX); //Optional disable ACK
}

void loop() {
	LoraPJON.update();
	LoraPJON.receive();
	eventManager(); //Send a message every 1 second without using PJON auto repeat
}

inline void eventManager() {
	static uint32_t lastTime = millis();
	if (millis() - lastTime > EVENT_TIME) {
		generateData();
		lastTime = millis();
	}
}

inline void generateData() {
	String message;
	DynamicJsonBuffer  jsonBuffer(200);
	JsonObject& root = jsonBuffer.createObject();
	JsonArray& data = root.createNestedArray("ADC");
	data.add(analogRead(A0));
	data.add(analogRead(A1));
	data.add(analogRead(A2));
	data.add(analogRead(A3));
	data.add(analogRead(A4));
	data.add(analogRead(A5));
	root.printTo(message);

	LoraPJON.send(44, message.c_str(), message.length());
}