// This PJON device is a simulated environmental controller, trying to change
// temperature so that measurement is equal to the target that can be set
// from other devices. In this case we will let it send messages to and receive
// messages from another device which is this setup is an MQTT gateway.

// Using the free MQTT Explorer or similar, publish the value "temperature=24"
// to the topic pjon/device42/input. Observe the value of the topic
// pjon/device42/output change gradually from "temperature=20" to "temperature=24".

// NOTE: The gateway used in this setup is using MIRROR_DIRECT mode, just forwarding
// packets both ways without trying to locate or translate variable names,
// therefore packets are sent to pjon/device42/output instead of
// pjon/device42/output/temperature as would have been the case with
// the MIRROR_TRANSLATE mode.

#include <PJONMQTTTranslate.h>

#define PJON_GATEWAY_ID 254


PJON<SoftwareBitBang> bus(42);

float temperature = 20.0;        // "Measurement"
float temperature_target = 20.0; // "Target"
uint32_t start = 0;

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if (length > 0) {
    char buf[30];
    uint16_t len = min(sizeof buf, length + 1);
    strncpy(buf, (const char *)payload, len);
    buf[len -1] = 0;
    Serial.print("Received from broker: "); Serial.println(buf);
    if (strncmp("temperature=", buf, 12)==0) temperature_target = atof(&buf[12]);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("SomeDevice started.");
  bus.strategy.set_pin(7);
  bus.set_receiver(receiver_function);
  bus.begin();
}

void loop() {
  bus.update();
  bus.receive(1000);

  if (millis() - start > 1000) {
    start = millis();
    // Send outputs to broker via gateway
    char buf[30];
    strcpy(buf, "temperature=");
    dtostrf(temperature,5,2,&buf[strlen(buf)]);
    bus.send(PJON_GATEWAY_ID, buf, strlen(buf));

    // Adjust to get closer to target, simulate a regulator
    temperature += (temperature_target - temperature) / 20;
  }
};
