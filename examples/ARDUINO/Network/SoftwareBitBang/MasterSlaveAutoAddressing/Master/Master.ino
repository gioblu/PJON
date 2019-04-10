
/* Manual synchronous acknowledge requires longer than default timeout
   Set higher duration if required */
#define SWBB_RESPONSE_TIMEOUT 5000

#include <PJONMaster.h>

// Bus id (unique bus identifier)
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object, device id is fixed to PJON_MASTER_ID or 254
PJONMaster<SoftwareBitBang> bus(bus_id);

uint32_t time;

void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("PJON error: connection lost with device id ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  if(code == PJON_DEVICES_BUFFER_FULL) {
    Serial.print("PJONMaster error: master devices' buffer is full with a length of ");
    Serial.println(data);
  }
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */

  // If debug is active prints addressing packet infromation
  if(packet_info.port == PJON_DYNAMIC_ADDRESSING_PORT) {
    Serial.print("Addressing request: ");
    Serial.print(payload[0]);
    Serial.print(" Device address: ");
    Serial.print(payload[1]);
    Serial.print(".");
    Serial.print(payload[2]);
    Serial.print(".");
    Serial.print(payload[3]);
    Serial.print(".");
    Serial.print(payload[4]);
    Serial.print(".");
    Serial.print(payload[5]);
  }

  // General packet data
  Serial.print(" Header: ");
  Serial.print(packet_info.header, BIN);
  Serial.print(" Length: ");
  Serial.print(length);
  Serial.print(" Sender id: ");
  Serial.print(packet_info.sender_id);

  // Packet content
  Serial.print(" Packet: ");
  for(uint8_t i = 0; i < length; i++) {
    Serial.print(payload[i]);
    Serial.print(" ");
  }
  Serial.print("Packets in buffer: ");
  Serial.println(bus.update());
};

void setup() {
  Serial.begin(115200);
  /* Let addressing procedure packets to be received by the receiver function
     to ease debugging or analysis */
  bus.debug = true;

  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.set_error(error_handler);
  bus.begin();
  time = millis();
};

void loop() {
  if(millis() - time > 5000) {
    Serial.println("List of slaves known by master: ");
    for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
      if(bus.ids[i].state) {
        Serial.print(" - Device id: ");
        Serial.print(i + 1); // Shifted by one to avoid PJON_BROADCAST
        Serial.print(" Device address: ");
        Serial.print(bus.ids[i].device_address[0]);
        Serial.print(".");
        Serial.print(bus.ids[i].device_address[1]);
        Serial.print(".");
        Serial.print(bus.ids[i].device_address[2]);
        Serial.print(".");
        Serial.print(bus.ids[i].device_address[3]);
        Serial.print(".");
        Serial.print(bus.ids[i].device_address[4]);
        Serial.println();
      }
    }
    Serial.println();
    Serial.flush();
    time = millis();
  }
  bus.receive(5000);
  bus.update();
};
