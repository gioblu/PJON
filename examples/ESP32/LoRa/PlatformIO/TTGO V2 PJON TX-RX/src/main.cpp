/**
 * @file main.cpp
 * @author Michael Branson
 * @brief TTGO Lora32 Example of PJON.
 *  Upload the same code to two devices and they will PJON_BROADCAST to each other, displaying round trip time on the OLED.
 * @version 0.1
 * @date 2020-04-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <Arduino.h>
#include <SPI.h>

#define USE_OLED // load screen code
#include "oled.h"

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

#define BAND    915000000UL // Match your boards Lora frequencies!

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define PJON_INCLUDE_TL
// #define PJON_PACKET_MAX_LENGTH 1024
#define PJON_INCLUDE_PACKET_ID true

#include <PJON.h>
const uint8_t bus_id[] = {0, 0, 0, 0};
const uint8_t dev_id = (int)random(1, 255);
PJON<ThroughLora> bus(bus_id,dev_id);

int32_t timer2 = millis();
int32_t dev_heartbeat_frequency = 5000;
uint32_t packet_id{};
uint32_t timer{};

void PrintHex8(uint8_t *data, uint16_t length) // prints 8-bit data in hex with leading zeroes
{
       for (int i=0; i<length; i++) {
         if (data[i]<0x10) {Serial.print("0");}
         Serial.printf("0x%02x ",data[i]);
       }
       Serial.println("");
       for (int i=0; i<length; i++) {
         
         Serial.print((char)data[i]);
         Serial.print("  ");
       }
       Serial.println();
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  
  if(payload[0]=='B'){
    uint32_t t_time = millis() - timer;
    String msg = "Pong: "+String(t_time)+"ms rtt"; // round trip time
    oled(msg, 1, false);
  }
  if(payload[0]=='P'){
    bus.send(0, "B",1,bus.config,packet_info.id);
  }

  Serial.print("Got packet from ");
  Serial.print(packet_info.sender_id);
  Serial.print(":");
  Serial.println(packet_info.port);
  PrintHex8(payload,length);
  

}



void setup() {
  Serial.begin(115200);
  oled_setup();
  bus.set_receiver(receiver_function);
  bus.set_packet_id(true);
  bus.include_sender_info(true);
  bus.include_port(true);
  //bus.set_crc_32(true);
  SPI.begin(SCK,MISO,MOSI,SS); // Important for TTGO boards
  bus.strategy.setPins(SS,RST,DI0);
  bus.strategy.setFrequency(BAND); 
  bus.strategy.setSignalBandwidth(250E3); //Optional
  bus.begin();
  Serial.println("\nPJON Ready.");
}

void loop() {
   if(millis()-timer2 > dev_heartbeat_frequency){
      bus.send(0, "P",1,bus.config,++packet_id);
      timer = millis();
      timer2 = timer;
      oled("Ping "+String(packet_id), 0, true);
      
   }
  bus.update();
  bus.receive();
}