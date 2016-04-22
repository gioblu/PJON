#include <PJON.h>

// network(Arduino pin used, selected device id)
PJON network(12);
int packet;

void setup() {
  Serial.begin(115200);
  network.set_error(error_handler);

  unsigned long time = micros();
  network.acquire_id();
  Serial.print(micros() - time);

  Serial.print(" microseconds needed - ID found: ");
  Serial.print(network.device_id());

  char actual_id = network.device_id();
  packet = network.send(44, &actual_id, 1, 1000000);
}

void error_handler(uint8_t code, uint8_t data) {
  if(code == ID_ACQUISITION_FAIL) {
    Serial.print("Can't acquire a free id ");
    Serial.println(data);
  }
}

void loop() {
 network.update();
}
