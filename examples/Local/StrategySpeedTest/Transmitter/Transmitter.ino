// This pair of examples can be used to measure the speed of a strategy directly between two devices.
//
// Measured on two Unos for PJON v8.0:
// Strategy name     Packet size   Bandwidth    Msg/s

// LocalUP           5 B msg       5635 B/s     1127
// EthernetTCP       5 B msg       1569 B/s     262 keep_connection(false) (default)
// EthernetTCP       5 B msg       2652 B/s     441 keep_connection(true)

// LocalUP           100 B msg     45600 B/s    456
// EthernetTCP       100 B msg     17069 B/s    169 keep_connection(false) (default)
// EthernetTCP       100 B msg     22523 B/s    223 keep_connection(true)

#include <PJON.h>

#define SELECT_SWBB // SELECT_ETCP, SELECT_LUDP, or SELECT_SWBB

#if defined(SELECT_ETCP) || defined(SELECT_LUDP)
// Ethernet configuration for this device
byte gateway[] = { 192, 1, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };
byte mac[] = {0xDA, 0xCA, 0x7E, 0xEF, 0xFE, 0x5D};
uint8_t local_ip[] = { 192, 1, 1, 150 };
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
  Serial.println("Transmitter started.");
  
#if defined(SELECT_ETCP) || defined(SELECT_LUDP) 
  Ethernet.begin(mac, local_ip, gateway, gateway, subnet);
#endif
#if defined(SELECT_ETCP)
  uint8_t remote_ip[] = { 192, 1, 1, 151 };
  strategy.link.add_node(44, remote_ip);
  strategy.link.keep_connection(false);
#elif defined(SELECT_SWBB)
  strategy.set_pin(7);
#endif
  strategy.begin();
}

uint32_t cnt = 0;
uint32_t start = millis();

void loop() {
  if (!strategy.can_start()) return;
  const char *data5 = ",012";  // Comma is ascii 44 receiver id, needed by EthernetTCP strategy which is connection-oriented.
  const char *data100 = ",012 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 0123456789 012345";
  const char *data = data5; // Select packet length here by pointing to data5 or data100
  strategy.send_string(data, strlen(data) + 1); // Include null separator
  cnt++;
  if (millis() - start > 1000) {
    start = millis();
    Serial.print("Tried to send PING/s: "); Serial.print(cnt);
    cnt = 0;
  }
};
