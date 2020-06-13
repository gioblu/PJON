#include <PJONSoftwareBitBang.h>
/* VOLTAGE TESTER DEVICE
   This is a basic example to show how PJON can be used practically.
   Lets print in the Serial monitor the voltage detected by the analog
   pin of another Arduino compatible device connected to the bus.

   This sketch contains the receiver side. In the Serial monitor
   is printed the voltage detected and transmitted by the other device. */

// PJON object
PJONSoftwareBitBang bus(44);

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  Serial.print("RX:");
  Serial.print(" Device id: ");
  Serial.print(packet_info.rx.id);
  Serial.print(" | TX: ");
  Serial.print(" Device id: ");
  Serial.print(packet_info.tx.id);

  if((char)payload[0] == 'V') {
    Serial.print(" | Voltage: ");
    Serial.print((uint16_t)(payload[1] << 8) | (uint16_t)(payload[2] & 0xFF));
    Serial.print("mV");
    // Get back the original integer from the 2 separate bytes
  }
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
  bus.receive(1000);
};
