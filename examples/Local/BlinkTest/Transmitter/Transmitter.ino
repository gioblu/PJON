#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);

void setup() {
  bus.set_pin(12);
  bus.begin();
  bus.send_repeatedly(44, "B", 1, 1000000); // Send B to device 44 every second
}

void loop() {
  bus.update();
};
