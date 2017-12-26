
// Uncomment to run SoftwareBitBang in MODE 2
// #define SWBB_MODE 2
// Uncomment to run SoftwareBitBang in MODE 3
// #define SWBB_MODE 3

/*  Acknowledge Latency maximum duration (1000 microseconds default).
    Can be necessary to higher SWBB_RESPONSE_TIMEOUT to leave enough time to
    receiver to compute the CRC and to respond with a synchronous acknowledgement
    SWBB_RESPONSE_TIMEOUT can be reduced to higher communication speed if
    devices are near and able to compute CRC fast enough. */
//#define SWBB_RESPONSE_TIMEOUT 1000

#include <PJON.h>

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(44);

// Custom class
class MyClass {
  public:
    void doit() {
      Serial.println("Custom instance member function called correctly!");
    };
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(packet_info.custom_pointer)
    ((MyClass*)packet_info.custom_pointer)->doit();
};

MyClass myObject; // Custom class instance

void setup() {
  bus.strategy.set_pin(12);
  bus.begin();
  bus.set_receiver(receiver_function);
  bus.set_custom_pointer(&myObject); // Pointer to custom class instance
  Serial.begin(115200);
};

void loop() {
  Serial.println("Starting 1 second custom class instance pointer callback test...");
  long time = millis();
  unsigned int response = 0;
  while(millis() - time < 1000) response = bus.receive();
};
