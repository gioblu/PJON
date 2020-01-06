/* ARBITRARY DATA TYPE TRANSMISSION EXAMPLE
   This sketch contains the transmitter side. */

// Include only SoftwareBitBang
#define PJON_INCLUDE_SWBB
// Include PJONLocal
#include <PJONLocal.h>

/* Create PJONLocal object:
   Class name <Strategy name> instance name ( device id ) */
PJONLocal<SoftwareBitBang> bus(45);

// Buffer used by PJONLocal
uint8_t packet[PJON_PACKET_MAX_LENGTH];

unsigned long time;
int voltage;

/* A custom struct is defined just to clarify that any kind of custom
   data type can be easily transmitted using the PJON send functions. */

struct voltage_record {
  uint16_t voltage;
  uint32_t id;
};

voltage_record r;

void execute_reading() {
  r.voltage = (float)(5.0 / (1023.0 / analogRead(A0))) * 1000;
  r.id++;
};


void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  Serial.begin(115200);
  time = millis();
  Serial.print("PJONLocal - Sender's device id: ");
  Serial.print(bus.device_id());
  Serial.println("Cyclical A0 pin voltage reading transmission...");
  Serial.println("This example demonstrates how to send arbitrary data types");
  r.id = 0;
}

void loop() {
  execute_reading();
  if(bus.ready_to_send()) bus.send_packet(44, packet, &r, sizeof(r));
  delay(1000);
};
