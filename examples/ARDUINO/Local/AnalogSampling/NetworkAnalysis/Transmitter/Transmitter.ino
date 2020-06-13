
/* Use a couple of visible light / IR / UV LEDs as wireless bidirectional transceivers
   Connect LED + to A0
   Connect LED - to GND
   Place a 100KΩ-5MΩ pull down resistor between A0 and GND
   Try different resistor values to find the optimal to maximize range
   Higher resistance can higher the range but can also higher background noise.  */

#include <PJONAnalogSampling.h>

float test;
float mistakes;
int busy;
int fail;


PJONAnalogSampling bus(45);

int packet;
uint8_t content[] = "01234567890123456789";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize LED 13 to be off

  /* Use internal 1.1v analog reference (not available on Arduino Mega)
  DEFAULT: Default analog reference 5v (5v Arduino boards) or 3.3v (3.3V Arduino boards)
  INTERNAL: Built-in 1.1v on ATmega168 or ATmega328 and 2.56v on ATmega8 (not available on Arduino Mega)
  INTERNAL1V1: Built-in 1.1v reference (Arduino Mega only)
  INTERNAL2V56: a built-in 2.56v reference (Arduino Mega only)
  EXTERNAL: Voltage applied to the AREF pin (0 to 5v) is used as the reference. */
  analogReference(INTERNAL);

  bus.strategy.set_pin(A0);
  bus.begin();

  Serial.begin(115200);
  Serial.println("PJON - AnalogSampling Network analysis");
  Serial.println("Starting a 1 second communication test...");
  Serial.println();
}

void loop() {
  long time = millis();
  while(millis() - time < 1000) {

    /* Here send_packet low level function is used to
    be able to catch every single sending result. */

    unsigned int response = bus.send_packet(44, content, 20);
    if(response == PJON_ACK)
      test++;
    if(response == PJON_NAK)
      mistakes++;
    if(response == PJON_BUSY)
      busy++;
    if(response == PJON_FAIL)
      fail++;
  }
  Serial.print("Packet Overhead: ");
  Serial.print(bus.packet_overhead() + 1);
  Serial.print("B - Total: ");
  Serial.print((unsigned int)((bus.packet_overhead() + 1) * test));
  Serial.println("B");
  Serial.print("Bandwidth: ");
  // length + packet overhead + PJON_ACK
  Serial.print((unsigned int)(test * (20 + bus.packet_overhead() + 1)));
  Serial.println("B/s");
  Serial.print("Data throughput: ");
  Serial.print((unsigned int)(test * 20));
  Serial.println("B/s");
  Serial.print("Packets sent: ");
  Serial.println((unsigned int)test);
  Serial.print("Mistakes (error found with CRC): ");
  Serial.println((unsigned int)mistakes);
  Serial.print("Fail (no acknowledge from receiver): ");
  Serial.println(fail);
  Serial.print("Busy (Channel is busy or affected by interference): ");
  Serial.println(busy);
  Serial.print("Accuracy: ");
  Serial.print(100 - (100 / (test / mistakes)));
  Serial.println(" %");
  Serial.println("---------------------");
  // Avoid Serial interference during test flushing
  Serial.flush();

  test = 0;
  mistakes = 0;
  busy = 0;
  fail = 0;
};
