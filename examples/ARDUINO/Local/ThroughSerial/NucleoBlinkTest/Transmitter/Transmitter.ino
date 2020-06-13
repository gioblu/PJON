
/* Include only ThroughSerial
   (not all data-link are still compatible with STM32 microcontrollers) */


#include <PJONThroughSerial.h>

// <Strategy name> bus(selected device id)
PJONThroughSerial bus(45);

void setup() {
  // Serial1 or pin 2 and 8 on F401RE and L053R8
  Serial1.begin(9600);
  bus.strategy.set_serial(&Serial1);
  bus.begin();
  // Send B to device 44 10 times per second
  bus.send_repeatedly(44, "B", 1, 100000);
};

void loop() {
  bus.update();
};
