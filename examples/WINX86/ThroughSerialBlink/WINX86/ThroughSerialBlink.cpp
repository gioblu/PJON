
#include "stdafx.h"
#include <stdio.h>
#include <iostream>

// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>

#include "PJON/interfaces/WINX86/Serial/Serial.h"

#ifndef WINX86
  #define WINX86 true
#endif

#define PJON_INCLUDE_TS true // Include only ThroughSerial
#include "PJON/PJON.h"
#include "PJON/PJONDefines.h"

static void receiver_function(
  uint8_t *payload,
  uint16_t length,
  const PJON_Packet_Info &packet_info
) {
  for (uint32_t i = 0; i != length; i++)
    std::cout <<  payload[i];
  std::cout << std::endl;
};

int main() {
  printf("PJON instantiation... \n");
  PJON<ThroughSerial> bus(45);

  bool resetComOnStratup = false;
  bool testComOnStartup = false;
  tstring commPortName(TEXT("COM3"));
  int bitRate = 115200;

  try {
    printf("Opening serial... \n");
    Serial serial_handle(
      commPortName,
      bitRate,
      testComOnStartup,
      resetComOnStratup
    );

    if(resetComOnStratup)
      delayMicroseconds(2000000);

    printf("Setting serial... \n");
    bus.strategy.set_serial(&serial_handle);

    printf("Opening bus... \n");
    bus.begin();
    bus.set_receiver(receiver_function);

    for(int i = 0; i < 500; i++) {
      bus.send(44, "BCD123456789BCD123456789", 24);

      auto begin_ts = std::chrono::high_resolution_clock::now();
      int bus_update_ms = 15;
      while(true) {
        auto elapsed_usec =
          std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::high_resolution_clock::now() - begin_ts
          ).count();

        if(elapsed_usec >= bus_update_ms * 1000) break;
        bus.update();
        bus.receive(100000);
      }
    }

    printf("Attempting to receive from bus... \n");
    bus.receive(100);
    printf("Success! \n");
    return 0;
  } catch (const char* msg) {
    printf("exc: ");
    printf(msg);
    printf("\n");
    return 1;
  }

};
