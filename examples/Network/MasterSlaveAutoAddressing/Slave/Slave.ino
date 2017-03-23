#include <PJONSlave.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED);

int packet;
char content[] = "01234567890123456789";
uint32_t time;

void setup() {
  bus.set_error(error_handler);
  bus.strategy.set_pin(12);
  bus.begin();
  Serial.begin(115200);
  time = millis();
}

void error_handler(uint8_t code, uint8_t data) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with master ");
    Serial.println(bus.packets[data].content[0], DEC);
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    Serial.print("Unable to acquire an id: ");
    Serial.println(data);
  }
}

void loop() {
  if(millis() - time > 3000) {
    Serial.println(bus.device_id());
    time = millis();
  }
  bus.update();
  bus.receive();
};
