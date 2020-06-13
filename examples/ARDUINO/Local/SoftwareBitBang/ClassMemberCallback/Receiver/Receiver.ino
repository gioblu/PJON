
// Uncomment to use the mode you prefer (default SWBB_MODE 1)
// #define SWBB_MODE 1 // 1.95kB/s - 15625Bd
// #define SWBB_MODE 2 // 2.21kB/s - 17696Bd
// #define SWBB_MODE 3 // 2.94kB/s - 23529Bd
// #define SWBB_MODE 4 // 3.40kB/s - 27210Bd

/*  Response timeout (1500 microseconds default).
    If the acknowledgement fails SWBB_RESPONSE_TIMEOUT may be too short
    specially if long packets are sent or if devices are far from each other */

//#define SWBB_RESPONSE_TIMEOUT 1500

#include <PJONSoftwareBitBang.h>

// <Strategy name> bus(selected device id)
PJONSoftwareBitBang bus(44);

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
