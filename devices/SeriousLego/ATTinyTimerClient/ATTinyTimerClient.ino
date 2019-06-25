#include <PJON.h>
#define myId 40
/*
 * ATTiny85 button client.
 * ========================
  * Configure: Pin2 for PJON
 * Pin 0 for button read
 * External clock on pin 3 and 4
 */

PJON<SoftwareBitBang> bus(myId);

byte receiveState = 0;
byte currentCommand = 0;
boolean doReport = false;
uint16_t currentSendId=PJON_FAIL;
boolean startSend=false;
boolean stopSend=false;
boolean updateInterval=false;
uint32_t sendInterval=1000000;

void setup() {
  // put your setup code here, to run once:
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  pinMode(2, INPUT);
  bus.strategy.set_pin(2);
  bus.set_receiver(receiver_function);
  bus.begin();
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  payload[length]=0;
  if (receiveState == 0) {
    currentCommand = (int)atol(payload);
    switch (currentCommand) {
      case 1: 
        startSend = true;
        break;
      case 2:
        stopSend = true;
        break;
      case 3:
        receiveState++;
    }
 
    return;
  }
  receiveState=0;
  long param = atol(payload);
  sendInterval = param;
  updateInterval = true;
}

void loop() {
  bus.receive();
  bus.update();
  if (startSend) {
    currentSendId = bus.send_repeatedly(1, "A", 1, sendInterval);
    startSend = false;
  }
  if (stopSend) {
    bus.remove(currentSendId);
    stopSend=false;
  }
  if (updateInterval) {
    bus.remove(currentSendId);
    currentSendId = bus.send_repeatedly(1, "A ", 1, sendInterval);
    updateInterval=false;
  }
}
