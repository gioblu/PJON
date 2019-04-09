#include <stdio.h>
#include <iostream>
// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>

#ifndef WINX86
  #define WINX86 true
#endif

#define PJON_INCLUDE_TSA true

#define TSA_RESPONSE_TIME_OUT 35000
/* Maximum accepted timeframe between transmission and synchronous
   acknowledgement. This timeframe is affected by latency and CRC computation.
   Could be necessary to higher this value if devices are separated by long
   physical distance and or if transmitting long packets. */

#include <PJON.h>

static void receiver_function(
  uint8_t *payload,
  uint16_t length,
  const PJON_Packet_Info &packet_info
) {
  for (uint32_t i = 0; i != length; i++)
    std::cout << payload[i];
  std::cout << std::endl;
  fflush(stdout);
};

int main() {
  printf("PJON instantiation... \n");
  PJON<ThroughSerialAsync> bus(45);
  bus.set_receiver(receiver_function);

  // Set here the COM port assigned to the device you want to communicate with
  tstring commPortName(TEXT("COM5"));
  uint32_t bitRate = 9600;

  try {
    printf("Opening serial... \n");
    Serial serial_handle(
      commPortName,
      bitRate,
      false,
      false
    );

    printf("Setting serial... \n");
    bus.strategy.set_serial(&serial_handle);
    printf("Opening bus... \n");
    bus.begin();
    bus.send_repeatedly(44, (uint8_t *)"B", 1, 1000000);
    printf("Success, initiating BlinkTest repeated transmission... \n");

    while(true) {
      bus.update();
      bus.receive();
    }
  }

  catch (const char* msg) {
    printf("exc: ");
    printf(msg);
    printf("\n");
    return 1;
  }

};
