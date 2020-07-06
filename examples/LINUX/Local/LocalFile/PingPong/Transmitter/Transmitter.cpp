
#include <PJONLocalFile.h>


PJONLocalFile bus(45);

uint32_t cnt = 0;
uint32_t start = millis();

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if (payload[0] == 'P') {
    cnt++;

    // Verify that we are receiving a continuous sequence of increasing numbers
    // (No packets lost, no reordering)
    uint32_t packetCnt = 0;
    static uint32_t lastReceivedPacketCnt = 0;
    sscanf((const char*)payload, "P%d", &packetCnt);
    if (packetCnt - lastReceivedPacketCnt != 1)
      printf("diff=%d ", packetCnt - lastReceivedPacketCnt);
    lastReceivedPacketCnt = packetCnt;  
  }
}

void loop() {
  bus.update();
  bus.receive();

  if (millis() - start > 1000) {
    start = millis();
    printf("PONG/s: "); printf("%d\n", cnt);
    cnt = 0;
  }

   static uint32_t lastSend = micros();
   static uint32_t packetCnt = 1;
  if (micros() - lastSend >= 1000) {
    lastSend = micros();
    // Send a continuous sequence of increasing numbers
    char buf[20];
    sprintf(buf, "P%d", packetCnt++);
    bus.send(44, buf, (uint16_t) strlen(buf)+1);
  }
};

int main() {
  bus.set_receiver(receiver_function);
  bus.begin();

  do loop(); while(true);
}
