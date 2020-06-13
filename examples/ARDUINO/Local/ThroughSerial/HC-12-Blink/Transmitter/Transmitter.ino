/* Giovanni Blu Mitolo 2017 - gioscarab@gmail.com
   With this example it is possible to test HC-12 433MHz wireless serial
   module (HCMODU0054) along with PJON and ThroughSerial strategy. A software
   emulated Serial port is used on pin 2 and 3 to communicate with HC12. The
   receiver device should blink every second.  */

#include <PJONThroughSerial.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(2, 3);

// <Strategy name> bus(selected device id)
PJONThroughSerial bus(45);

void setup() {
  // Set HC12 baudrate (you must use the one configured in HC12, default 9600)
  HC12.begin(9600);

  // Pass the HC12 Serial instance you want to use for PJON communication
  bus.strategy.set_serial(&HC12);

  // Avoid synchronous acknowledgement
  bus.set_acknowledge(false);

  bus.begin();

  // Send B to device 44 every second
  bus.send_repeatedly(44, "B", 1, 1000000);
};

void loop() {
  bus.update();
};
