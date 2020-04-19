#ifndef _OLED_
#define _OLED_

#include <Arduino.h>

  #define SCREEN_WIDTH 128 // OLED display width, in pixels
  #define SCREEN_HEIGHT 64 // OLED display height, in pixels
  // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
  #define OLED_RESET    16 // Reset pin # (or -1 if sharing Arduino reset pin)

#ifdef USE_OLED
  #include <SSD1306.h> 
  SSD1306 display(0x3c, 21, 22);

#endif

void oled_setup(){
  #ifdef USE_OLED
  pinMode(OLED_RESET,OUTPUT);
  digitalWrite(OLED_RESET,LOW);
  delay(50);
  digitalWrite(OLED_RESET,HIGH);
  display.init();
  display.flipScreenVertically();  
  display.setFont(ArialMT_Plain_16);
  #endif
}

void oled(String text, int8_t line, bool clear){
  #ifdef USE_OLED
  if(clear){
    display.clear();
  }
  //display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(0 , (line*20) , 128, " ");
  display.drawStringMaxWidth(0 , (line*20) , 128, text);
  // for(int8_t x=0;x<5;x++){
  //   //display.drawString(0 , (x*15) , lines[x]);
  // }
  
  display.display();
  #endif
}

#endif