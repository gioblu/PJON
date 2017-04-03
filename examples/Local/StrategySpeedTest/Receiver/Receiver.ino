#include <PJON.h>

#define SELECT_SWBB // SELECT_ETCP, SELECT_LUDP, or SELECT_SWBB

#if defined(SELECT_ETCP) || defined(SELECT_LUDP)
// Ethernet configuration for this device
byte gateway[] = { 192, 1, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte mac[] = {0xDA, 0x5A, 0x4E, 0xEF, 0xAE, 0xED};
uint8_t local_ip[] = { 192, 1, 1, 151 };
#endif

#if defined(SELECT_ETCP)
EthernetTCP strategy;
#elif defined(SELECT_LUDP)
LocalUDP strategy;
#elif defined(SELECT_SWBB)
SoftwareBitBang strategy;
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Receiver started.");

#if defined(SELECT_ETCP) || defined(SELECT_LUDP)   
  Ethernet.begin(mac, local_ip, gateway, gateway, subnet);
#endif
#if defined(SELECT_ETCP)
  strategy.link.set_id(44);
  strategy.link.keep_connection(false);
  strategy.link.start_listening();
#elif defined(SELECT_SWBB)
  strategy.set_pin(7);
#endif
  strategy.begin();
};

uint32_t cnt = 0, byte_cnt = 0;
uint32_t start = millis();
char buf[200];

void loop() {
  if (!strategy.can_start()) return;
  uint16_t result = strategy.receive_string(buf, sizeof buf);
  if (result != PJON_FAIL) {
    cnt++;
    byte_cnt += result;
  }
  if (millis() - start > 1000) {
    start = millis();
    Serial.print("Received PING/s: "); Serial.print(cnt); Serial.print(", bytes/s: "); Serial.print(byte_cnt);
    Serial.print(", last msg: "); Serial.println(buf);
    cnt = byte_cnt = 0;
  }  
};
