
/* This example implemments a continuous PING-PONG between 2 devices at the
   maximum speed. In average 170-180 PONG/s should be exchanged */

// ESP32 WiFi country configuration is done using constants:

// Set country, for example Italy
// #define PJON_ESPNOW_COUNTRY "IT"

// Set schan, for example 1
// PJON_ESPNOW_SCHAN 1

// Set nchan, for example 14
// PJON_ESPNOW_NCHAN 14

// Set country policy
// #define PJON_ESPNOW_POLICY WIFI_COUNTRY_POLICY_MANUAL // for manual
// #define PJON_ESPNOW_POLICY WIFI_COUNTRY_POLICY_AUTO   // for auto

#define PJON_INCLUDE_EN
#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ESPNOW> bus(44);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'P') {
    cnt++;
    bus.reply("P", 1);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver started.");

  bus.set_receiver(receiver_function);
  bus.begin();
};

void loop() {
  bus.update();
  bus.receive();

  if (millis() - start > 1000) {
    Serial.print("PING/s: "); Serial.println(1000.0f*float(cnt)/float((uint32_t)(millis()-start)));
    start = millis();
    cnt = 0;
  }
};
