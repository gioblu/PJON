#include <PJON.h>

// Bus id definition
uint8_t bus_id[] = {0, 0, 0, 1};

// PJON object
PJON<SoftwareBitBang> bus(bus_id, 44);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  Serial.print("Receiver bus id: ");
  Serial.print(packet_info.rx.bus_id[0]);
  Serial.print(packet_info.rx.bus_id[1]);
  Serial.print(packet_info.rx.bus_id[2]);
  Serial.print(packet_info.rx.bus_id[3]);
  Serial.print(" - device id: ");
  Serial.println(packet_info.rx.id);

  Serial.print("Sender bus id: ");
  Serial.print(packet_info.tx.bus_id[0]);
  Serial.print(packet_info.tx.bus_id[1]);
  Serial.print(packet_info.tx.bus_id[2]);
  Serial.print(packet_info.tx.bus_id[3]);
  Serial.print(" - device id: ");
  Serial.println(packet_info.tx.id);

  if(payload[0] == 'B') {
    Serial.println(" BLINK!");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(30);
    digitalWrite(LED_BUILTIN, LOW);
  }
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  bus.strategy.set_pin(12);
  bus.begin();
  bus.set_receiver(receiver_function);

  Serial.begin(9600);
};

void loop() {
  bus.receive(1000);
};
