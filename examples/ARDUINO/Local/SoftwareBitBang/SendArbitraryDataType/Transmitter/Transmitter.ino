
#include <PJONSoftwareBitBang.h>
/* ARBITRARY DATA TYPE TRANSMISSION EXAMPLE
   This sketch contains the transmitter side. */

// <Strategy name> bus(selected device id)
PJONSoftwareBitBang bus(45);
int packet;
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
  Serial.print("PJON - Sender's device id: ");
  Serial.print(bus.device_id());
  Serial.println(" A0 pin voltage cyclical record sending...");
  Serial.println("This example demonstrates how to send arbitrary data types");
  // Avoid simultaneous transmission of Serial and SoftwareBitBang data
  Serial.flush();
  r.id = 0;
}


void loop() {
  if(!bus.update()) {
    execute_reading();
    bus.send(44, &r, sizeof(r));
  }
};
