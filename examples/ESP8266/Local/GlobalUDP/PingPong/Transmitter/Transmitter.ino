#include <PJONGlobalUDP.h>

// Address of remote device
uint8_t remote_ip[] = { 192, 1, 1, 163 };

// <Strategy name> bus(selected device id)
PJONGlobalUDP bus(45);

const char* ssid     = "MyNetworkSSID";
const char* password = "MyNetworkPassword";

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if (payload[0] == 'P') cnt++;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Transmitter started.");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("Now listening at IP %s\n", WiFi.localIP().toString().c_str());

  bus.strategy.add_node(44, remote_ip);
  bus.set_receiver(receiver_function);
  bus.begin();
  bus.send_repeatedly(44, "P", 1, 20000); // Send P to device 44 repeatedly
}

void loop() {
  bus.update();
  bus.receive();

  if (millis() - start > 1000) {
    Serial.print("PONG/s: "); Serial.println(1000.0f*float(cnt)/float((uint32_t)(millis()-start)));
    start = millis();
    cnt = 0;
  }
};
