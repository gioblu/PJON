#include <PJONMaster.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};
uint32_t time;

// PJON object - The Master device id is fixed to PJON_MASTER_ID or 254
PJONMaster<SoftwareBitBang> bus(bus_id);

void setup() {
  Serial.begin(115200);
  /* Let addressing procedure packets to be received by the receiver function
     to ease debugging or analysis */
  bus.debug = true;

  bus.strategy.set_pin(12);
  bus.set_receiver(receiver_function);
  bus.set_error(error_function);
  bus.begin();
  /* Send a continuous greetings packet every secon
     to showcase the receiver function functionality if debug is active*/
  if(bus.debug)
    bus.send_repeatedly(PJON_BROADCAST, "Master says hi!", 15, 1000000);
  time = millis();
};

void error_function(uint8_t code, uint8_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection lost with device ");
    Serial.println((uint8_t)bus.packets[data].content[0], DEC);
  }
  if(code == PJON_DEVICES_BUFFER_FULL) {
    Serial.print("Master devices buffer is full with a length of ");
    Serial.println(data);
  }
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */

  // If debug is active prints addressing packet infromation
  if(packet_info.port == PJON_DYNAMIC_ADDRESSING_PORT) {
    uint32_t rid =
      (uint32_t)(payload[1]) << 24 |
      (uint32_t)(payload[2]) << 16 |
      (uint32_t)(payload[3]) <<  8 |
      (uint32_t)(payload[4]);
    Serial.print("Addressing request: ");
    Serial.print(payload[0]);
    Serial.print(" RID: ");
    Serial.print(rid);
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

void loop() {
  if(millis() - time > 5000) {
    Serial.println("List of slaves known by master: ");

    // Check if registered slaves are still present on the bus
    bus.check_slaves_presence();

    for(uint8_t i = 0; i < PJON_MAX_DEVICES; i++) {
      if(bus.ids[i].state) {
        Serial.print(" - Device id: ");
        Serial.print(i + 1); // Shifted by one to avoid PJON_BROADCAST
        Serial.print(" Device rid: ");
        Serial.print(bus.ids[i].rid);
        Serial.println();
      }
    }
    Serial.println();
    time = millis();
  }
  bus.receive();
  bus.update();
};
