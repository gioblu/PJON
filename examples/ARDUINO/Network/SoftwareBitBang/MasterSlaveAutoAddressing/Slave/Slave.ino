#include <PJONSlave.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJONSlave<SoftwareBitBang> bus(bus_id, PJON_NOT_ASSIGNED);

int packet;
char content[] = "01234567890123456789";
uint32_t time;
bool acquired = false;

void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  Serial.print("Received: ");
  for(uint16_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial.print(" ");
  }
  Serial.println();
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    if(data == PJON_ID_ACQUIRE)
      Serial.println("Multi-master auto addressing procedure failed.");
    if(data == PJON_ID_CONFIRM)
      Serial.println("Master-slave id confirmation procedure failed.");
    if(data == PJON_ID_NEGATE)
      Serial.println("Master-slave id release procedure failed.");
    if(data == PJON_ID_REQUEST)
      Serial.println("Master-slave id request procedure failed.");
  }
};

void setup() {
  Serial.begin(115200);
  bus.set_error(error_handler);
  bus.set_receiver(receiver_handler);
  bus.strategy.set_pin(12);
  bus.begin();
  bus.acquire_id_master_slave();
  time = millis();
}

void loop() {
  if((bus.device_id() != PJON_NOT_ASSIGNED) && !acquired) {
    Serial.print("Acquired device id: ");
    Serial.println(bus.device_id());
    acquired = true;
  }
  bus.update();
  bus.receive(1000);
};
