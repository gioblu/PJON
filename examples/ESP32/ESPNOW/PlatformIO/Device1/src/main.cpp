#include <Arduino.h>
#include <ArduinoOTA.h>

/* This example implemments a continuous PING-PONG between 2 devices at
   10Hz, so around 10 PING/s should be received. */

#include <WiFi.h>

// Define WiFi country configuration
static wifi_country_t wifi_country = {
  cc : "AU",
  schan : 1,
  nchan : 14,
  max_tx_power : 1,
  policy : WIFI_COUNTRY_POLICY_MANUAL
};

// Use the PJON_INCLUDE_EN to include the ESPNOW strategy
#define PJON_INCLUDE_EN

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ESPNOW> bus(44);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info)
{
  if (payload[0] == 'P')
  {
    cnt++;
    bus.reply("P", 1);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Receiver started.");

  bus.set_receiver(receiver_function);
  bus.begin();
};

void loop()
{
  bus.update();
  bus.receive();

  if (millis() - start > 1000)
  {
    Serial.print("PING/s: ");
    Serial.println(1000.0f * float(cnt) / float((uint32_t)(millis() - start)));
    start = millis();
    cnt = 0;
  }
};
