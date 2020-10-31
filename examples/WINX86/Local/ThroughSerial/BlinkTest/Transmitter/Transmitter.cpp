
// This example shows how to send and receive packets over the serial port

/* WINX86 constant may not be defined
   used by PJON to detect windows */
#ifndef WINX86
  #define WINX86 true
#endif

#include <stdio.h>
#include <iostream>

// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>

// Set the preferred baudrate
#define BAUDRATE  9600

// Set the USB COM port used
#define COM_PORT "COM9"

// Include PJON library with the ThroughSerial strategy
#include <PJONThroughSerial.h>
// Instantiate PJON over ThroughSerial
PJONThroughSerial bus(45);

// Error handler called when something goes wrong

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    printf("Packet transmission for device id ");
    printf("%d", bus.packets[data].content[0]);
    printf(" failed, some error occurred. \n");
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    printf("Packet buffer is full with a length of ");
    std::cout << data;
    printf("\ndefine PJON_MAX_PACKETS setting a higher value. \n");
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    printf("Packet is too long, length: ");
    printf(" %d \n", data);
  }
  fflush(stdout);
};

int main() {
  printf("PJON instantiation... \n");
  bus.set_error(error_handler);

  // Set here the COM port assigned to the device you want to communicate with
  tstring com_port(TEXT(COM_PORT));

  try {
    printf("Opening serial... \n");
    Serial serial_handle(com_port, BAUDRATE);

    printf("Setting serial... \n");
    bus.strategy.set_serial(&serial_handle);

    printf("Opening bus... \n");
    bus.begin();

    printf("Success, initiating BlinkTest 1s repeated transmission... \n");
    bus.send_repeatedly(44, "B", 1, 1000000);

    while(true) {
      bus.update();
      bus.receive();
      // This line helps to reduce the CPU's usage
      PJON_DELAY_MICROSECONDS(10);
    }
  } catch (const char* msg) {
    printf("exc: ");
    printf(msg);
    printf("\n");
    return 1;
  }
};
