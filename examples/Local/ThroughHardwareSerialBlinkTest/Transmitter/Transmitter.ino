#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<ThroughHardwareSerial> bus(45);

void setup() {
  bus.begin();
  Serial.begin(9600);
  bus.strategy.set_serial(&Serial); // Pass the Serial object you want to use for communication
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
}

void loop() {
  bus.update();
};
