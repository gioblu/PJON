
// For printf used below
#include <stdio.h>
// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
// RPI serial interface
#include <wiringPi.h>
#include <wiringSerial.h>

#ifndef RPI
  #define RPI true
#endif

#define PJON_INCLUDE_TS true // Include only ThroughSerial
#include "PJON/PJON.h"

int main() {
  printf("PJON instantiation... \n");
  PJON<ThroughSerial> bus(45);

  printf("Opening serial... \n");
  int s = serialOpen("/dev/ttyAMA0", 115200);
  if(s < 0) printf("Serial open fail!");
  if(wiringPiSetup() == -1) printf("WiringPi setup fail");
  printf("Setting serial... \n");
  bus.strategy.set_serial(s);

  printf("Opening bus... \n");
  bus.begin();
  printf("Attempting to send a packet... \n");
  bus.send(44, "B", 1);
  printf("Attempting to roll bus... \n");
  bus.update();
  printf("Attempting to receive from bus... \n");
  bus.receive();
  printf("Success! \n");
  return 0;
};
