// This PJON device is a simulated environmental controller, trying to change
// temperature and pressure so that measurements are equal to targets that 
// can be set from external systems through an MQTT broker.

// Using the free MQTT Explorer or similar, publish the value "24" to the
// topic pjon/device45/input/temperature. Observe the value of the topic
// pjon/device45/output/temperature change gradually from 20 to 24.

#define PJON_INCLUDE_MQTT
#define PJON_INCLUDE_ETCP
#define MQTTT_MODE MQTTT_MODE_MIRROR_TRANSLATE
#include <PJON.h>


// <Strategy name> bus(selected device id)
PJON<MQTTTranslate> bus(45);

// WiFi network to connect to
const char* ssid     = "MyNetworkSSID";
const char* password = "MyNetworkPassword";

// IP address of MQTT broker
const uint8_t broker_ip[] = { 192, 168, 1, 71 };


float pressure = 1.0, temperature = 20.0;               // "Measurements"
float pressure_target = 1.0, temperature_target = 20.0; // "Targets"
uint32_t start = 0;

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if (length > 0) {
    const char *msg = (const char *)payload;
    Serial.print("Received from broker: "); Serial.println(msg);
    if (strncmp("P=", msg, 2)==0) pressure_target = atof(&msg[2]);
    else if (strncmp("T=", msg, 2)==0) temperature_target = atof(&msg[2]);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("PJON MQTT device started.");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("Now listening at IP %s\n", WiFi.localIP().toString().c_str());

  bus.set_receiver(receiver_function);
  bus.strategy.set_address(broker_ip, 1883, "transmitter");
  bus.strategy.add_translation("P","pressure");
  bus.strategy.add_translation("T","temperature");
  bus.begin();
}

void loop() {
  bus.update();
  bus.receive();

  if (millis() - start > 1000) {
    start = millis();
    // Send outputs to broker
    char buf[30];
    sprintf(buf, "P=%f,T=%g", pressure, temperature);
    bus.send(44, buf, strlen(buf));

    // Adjust to get closer to target, simulate a regulator
    pressure += (pressure_target - pressure) / 10;
    temperature += (temperature_target - temperature) / 20;
  }
};