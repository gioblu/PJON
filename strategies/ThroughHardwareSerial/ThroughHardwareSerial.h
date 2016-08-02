
/* ThroughHardwareSerial enables PJON communication through the Serial port.
   Copyright (c) 2016 by Giovanni Blu Mitolo and Fred Larsen All rights reserved.

   With ThroughHardwareSerial PJON can run through the Serial port of your device.
   Both ports should be free from usb computer connection and data pin should
   be wired inverted (TX->RX, RX->TX).

   Why to add PJON over Serial?
   Serial communication is an hardware integrated communication medium that can
   reach very fast communication speed but it is quite basic.

   Serial Downsides:
     - No CRC, checksum or parity bit (ensure correct data transfert)
     - No transmission certainty (lack of acknowledge from receiver)
     - No packet handling (no retrasmission in case of error)
     - No idea of id (no chance to connect more than 2 devices together)

   Adding PJON over Serial you get:
     - Correct transmission certainty with the use of CRC 8-bit
     - Acnowledge byte to inform transmitter of correct data reception
     - Packet handling, i.e. example schedule a repeated packet transmission.
     - Connect 2 slaves with a 1 master having univocal communication with device id

   ThroughHardwareSerial can run a 2 device bus in multi-master or a n devices bus
   in a master-slave setup. (n still unkown, tested with 1 master and 2 slaves) */

#include <Arduino.h>

#define THROUGH_HARDWARE_SERIAL_MAX_TIME 1000000 // Wait up to 1 second for an incoming byte

class ThroughHardwareSerial {
  public:
    HardwareSerial *serial;

    /* Pass the Serial port where you want to operate with */

    void set_serial(HardwareSerial *serial_port) {
      serial = serial_port;
    };


    /* Returns the Serial object value i.e. if(Serial) */

    boolean can_start(uint8_t input_pin, uint8_t output_pin) {
      return serial;
    };


    /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b, uint8_t input_pin, uint8_t output_pin) {
      uint32_t time = micros();
      serial->write(b);
      serial->flush();

      #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD)
        // Wait for the lingering bytes to transmit (REF: http://forum.arduino.cc/index.php?topic=151014.15)
        while (!(UCSR0A & (1 << UDRE0)))  // Wait for empty transmit buffer
           UCSR0A |= 1 << TXC0;  // mark transmission not complete
        while (!(UCSR0A & (1 << TXC0)));   // Wait for the transmission to complete
      #endif
    };


    /* Try to receive a byte with a maximum waiting time */

    uint16_t receive_byte(uint8_t input_pin, uint8_t output_pin) {
      uint32_t time = micros();
      while(micros() - time < THROUGH_HARDWARE_SERIAL_MAX_TIME)
        if(serial->available() > 0)
          return (uint8_t)serial->read();
      return FAIL;
    };


    /* Receive byte response */

    uint16_t receive_response(uint8_t input_pin, uint8_t output_pin) {
      return receive_byte(input_pin, output_pin);
    };


    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response, uint8_t input_pin, uint8_t output_pin) {
      send_byte(response, input_pin, output_pin);
    };
};
