
/* If you need non-blocking asynchronous operation include the
   ThroughSerialAsync strategy using:
   #define PJON_INCLUDE_TSA true */
#include <PJON.h>

PJON<ThroughSerial> bus(45);
/* Use PJON<ThroughSerialAsync> bus(45);
   instead if non-blocking asynchronous operation is required */

void setup() {
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial); // Pass the Serial object you want to use for communication
  bus.begin();
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
};

void loop() {
  bus.update();
};
