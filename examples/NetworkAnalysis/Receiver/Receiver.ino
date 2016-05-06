#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(44);

void setup() {
  bus.set_pin(12);
  bus.begin();

  Serial.begin(115200);
};

void loop() {
  bus.receive();
};
