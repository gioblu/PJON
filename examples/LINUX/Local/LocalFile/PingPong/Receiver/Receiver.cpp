//#define PJON_MAX_PACKETS 1
#define PJON_INCLUDE_LF
#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<LocalFile> bus(44);

uint32_t cnt = 0, cnt2 = 1;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  if(payload[0] == 'P') {
    cnt++;

    // Verify that we are receiving a continuous sequence of increasing numbers
    // (No packets lost, no reordering)
    uint32_t packetCnt = 0;
    static uint32_t lastReceivedPacketCnt = 0;
    sscanf((const char*)payload, "P%d", &packetCnt);
    if (packetCnt - lastReceivedPacketCnt != 1)
      printf("diff=%d ", packetCnt - lastReceivedPacketCnt);
    lastReceivedPacketCnt = packetCnt;  

    // Reply with a another continuous sequence of increasing numbers
    char buf[20];
    sprintf(buf, "P%d", cnt2++);
    bus.reply(buf, (uint16_t) strlen(buf)+1);
  }
}

void loop() {
  bus.receive();
  while(bus.update());

  if (millis() - start > 1000) {
    start = millis();
    printf("PING/s: %d\n", cnt);
    cnt = 0;
  }
}

int main() {
  bus.set_receiver(receiver_function);
  bus.begin();

  do loop(); while(true);
}
