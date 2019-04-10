
/* Manual synchronous acknowledge requires longer than default timeout
   Set higher duration if required */
#define SWBB_RESPONSE_TIMEOUT 5000

#include <PJONSlave.h>

/* Bus id - Initially set as localhost to demonstrate the ability of slaves to
   find a master thanks to its advertisement no matter which bus id is used */
uint8_t bus_id[] = {0, 0, 0, 0};

// Device address (unique address of a physical device)
uint8_t device_address[] = {1, 2, 3, 4, 5};

// PJON object
PJONSlave<SoftwareBitBang> bus(device_address);

void receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  Serial.print("Received: ");
  for(uint16_t i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    Serial.print(" ");
  }
  Serial.println();
  Serial.flush();
};

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  if(code == PJON_ID_ACQUISITION_FAIL) {
    if(data == PJON_DEVICES_BUFFER_FULL)
      Serial.println("PJONSlave error: master is full, no id available.");
    if(data == PJON_ID_CONFIRM)
      Serial.println("PJONSlave error: id confirmation failed.");
    if(data == PJON_ID_NEGATE)
      Serial.println("PJONSlave error: id release failed.");
    if(data == PJON_ID_REQUEST)
      Serial.println("PJONSlave error: id request failed.");
  }
  Serial.flush();
};

void setup() {
  Serial.begin(115200);
  bus.set_error(error_handler);
  bus.set_receiver(receiver_handler);
  bus.strategy.set_pin(12);
  bus.begin();
}

void loop() {
  if(bus.connected) {
    Serial.print("Acquired device id: ");
    Serial.println(bus.device_id());
    Serial.flush();
  }
  bus.update();
  bus.receive(5000);
};
