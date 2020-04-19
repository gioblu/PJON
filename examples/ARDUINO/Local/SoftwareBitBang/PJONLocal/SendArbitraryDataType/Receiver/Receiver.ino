/* ARBITRARY DATA TYPE RECEPTION EXAMPLE
   This sketch contains the receiver side. In the Serial monitor
   is printed the record transmitted by the other device. */

// Include only SoftwareBitBang
#define PJON_INCLUDE_SWBB
// Include PJONLocal
#include <PJONLocal.h>
// <Strategy name> bus(selected device id)
PJONLocal<SoftwareBitBang> bus(44);

uint8_t data[PJON_PACKET_MAX_LENGTH];
PJON_Packet_Info info;

/* A custom struct is defined just to clarify that any kind of custom
   data type can be easily transmitted using the PJON send functions. */

struct voltage_record {
  uint16_t voltage;
  uint32_t id;
};

// Voltage record instance
voltage_record r;

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off
  Serial.begin(115200);
}

void loop() {
  uint16_t length = bus.receive(data, info);
  if(length) {
    uint8_t *payload = bus.get_payload(data);
    Serial.print("Receiver device id: ");
    Serial.print(info.receiver_id);
    Serial.print(" | Transmitter device id: ");
    Serial.println(info.sender_id);

    /* Copy received data in buffer */
    memcpy(&r, payload, sizeof(r));

    Serial.print("Record voltage: ");
    Serial.print(r.voltage);
    Serial.print("mV | Record id: ");
    Serial.println(r.id);
    Serial.println();
  }
};
