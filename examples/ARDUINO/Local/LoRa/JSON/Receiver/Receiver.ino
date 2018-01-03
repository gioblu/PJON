#define PJON_INCLUDE_TL
#include <SPI.h>
#include <PJON.h>
#include <ArduinoJson.h>
/*
To use this example, please download the LoRa third party Library from https://github.com/sandeepmistry/arduino-LoRa/
and JSON library from https://github.com/bblanchon/ArduinoJson
*/
PJON<ThroughLora> LoraPJON(44);

void setup() {
	Serial.begin(115200);
	Serial.println("LoRa Receiver");

	LoraPJON.strategy.setFrequency(868100000UL); //Obrigatory to initialize Radio with correct frequency
	LoraPJON.strategy.setSignalBandwidth(250E3); //Optional

	LoraPJON.begin();
	//LoraPJON.set_communication_mode(PJON_SIMPLEX); //Optional disable ACK
	LoraPJON.set_receiver(receiver_function);
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
	//Serial.println((float)pow(10.0, (-45.0 - (float)LoRa.packetRssi()) / 20.0)); Aproximate distance
	DynamicJsonBuffer  jsonBuffer(200);
	JsonObject& root = jsonBuffer.parseObject((char *)payload);
	root["RSSI"] = LoraPJON.strategy.packetRssi();
	root["SNR"] = LoraPJON.strategy.packetSnr();
	root.printTo(Serial);
	Serial.println();
};

void loop() {
	LoraPJON.update();
	LoraPJON.receive();
}