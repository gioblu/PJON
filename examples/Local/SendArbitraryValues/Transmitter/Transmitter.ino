
#include <PJON.h>
/* VOLTAGE TESTER DEVICE
   This is a basic example to show how PJON can be used practically.
   Lets print in the Serial monitor the voltage detected by the analog
   pin (max 5v) of another Arduino compatible device connected to the bus.

   This sketch contains the transmitter side. In the Serial monitor
   is printed transmitted data, exchange duration and transmission speed
   for easy benchmarking and nominal functionality assessment. */

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(45);
unsigned long time;
int packet;
int voltage;

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  Serial.begin(115200);
  time = millis();
  Serial.print("PJON - Device id ");
  Serial.print(bus.device_id());
  Serial.println(" A0 pin voltage cyclical packet sending...");
}


void loop() {
  if(millis() - time > 2500) {
    time = millis();
    voltage = (float)(5.0 / (1023.0 / analogRead(A0))) * 1000;
    Serial.print("Voltage: ");
    Serial.print(voltage, DEC);
    Serial.print("mV ");
    /* Avoid Serial and PJON concurrency */
    Serial.flush();

    /* Send a "V", break the int in two bytes */
    char content[3] = {'V', voltage >> 8, voltage & 0xFF};

    unsigned long send_time = micros();
    /* Use a blocking version of send. */
    packet = bus.send_packet_blocking(44, content, 3);

    /* Determine communication result and duration */
    send_time = micros() - send_time;
    float duration = send_time / 1000.0;
    Serial.print((packet == PJON_ACK) ? " Transmitted" : " Some error occurred");
    Serial.print(" - Duration: ");
    Serial.print(duration);
    Serial.print(" milliseconds");
    Serial.print(" - Communication speed: ");
    //               1 second / (duration / (overhead + length + PJON_ACK))
    Serial.print((int)(1000.0 / (duration / (bus.packet_overhead() + 3 + 1))));
    Serial.println("B/s");
  }
};
