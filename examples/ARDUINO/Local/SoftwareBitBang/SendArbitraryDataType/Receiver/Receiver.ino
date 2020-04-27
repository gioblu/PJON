#include <PJON.h>
/* ARBITRARY DATA TYPE RECEPTION EXAMPLE
   This sketch contains the receiver side. In the Serial monitor
   is printed the record transmitted by the other device. */

// PJON object
PJON<SoftwareBitBang> bus(44);

/* A custom struct is defined just to clarify that any kind of custom
   data type can be easily transmitted using the PJON send functions. */

struct voltage_record {
  uint16_t voltage;
  uint32_t id;
};

voltage_record r;

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  Serial.print("Receiver device id: ");
  Serial.print(packet_info.rx.id);
  Serial.print(" | Transmitter device id: ");
  Serial.println(packet_info.tx.id);

  /* Copy received data in buffer */
  memcpy(&r, payload, sizeof(r));

  Serial.print("Record voltage: ");
  Serial.print(r.voltage);
  Serial.print("mV | Record id: ");
  Serial.println(r.id);
  Serial.println();
  // Avoid simultaneous transmission of Serial and SoftwareBitBang data
  Serial.flush();
};

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  bus.set_receiver(receiver_function);
  Serial.begin(115200);
};

void loop() {
  bus.receive();
};
