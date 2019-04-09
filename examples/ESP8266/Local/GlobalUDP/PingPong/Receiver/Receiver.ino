#include <PJON.h>

// Ethernet configuration for this device
IPAddress gateway = { 192, 1, 1, 1 };
IPAddress subnet = { 255, 255, 255, 0 };
IPAddress local_ip = { 192, 1, 1, 163 };

// <Strategy name> bus(selected device id)
PJON<GlobalUDP> bus(44);

const char* ssid     = "MyNetworkSSID";
const char* password = "MyNetworkPassword";

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(payload[0] == 'P') {
    cnt++;
    bus.reply((uint8_t *)"P", 1);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver started.");
  WiFi.config(local_ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("Now listening at IP %s\n", WiFi.localIP().toString().c_str());

  bus.set_receiver(receiver_function);
  bus.begin();
};

void loop() {
  bus.update();
  bus.receive();

  if (millis() - start > 1000) {
    Serial.print("PING/s: "); Serial.println(1000.0f*float(cnt)/float((uint32_t)(millis()-start)));
    start = millis();
    cnt = 0;
  }
};
