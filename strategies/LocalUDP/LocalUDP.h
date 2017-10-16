
/* LocalUDP is a Strategy for the PJON framework (included in version v5.2)
   It supports delivering PJON packets over Ethernet UDP on local network (LAN).
   Compliant with the PJON protocol layer specification v0.3
   _____________________________________________________________________________

    LocalUDP strategy proposed and developed by Fred Larsen 02/10/2016

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License. */

#pragma once

#ifdef HAS_ETHERNETUDP
  #include <interfaces/ARDUINO/UDPHelper_ARDUINO.h>
#else
  #include <interfaces/LINUX/UDPHelper_POSIX.h>
#endif

#include <PJONDefines.h>

#define LUDP_DEFAULT_PORT                 7100
#define LUDP_RESPONSE_TIMEOUT  (uint32_t) 100000
#define LUDP_MAGIC_HEADER      (uint32_t) 0x0DFAC3D0

class LocalUDP {
    bool _udp_initialized = false;
    uint16_t _port = LUDP_DEFAULT_PORT;
    const uint32_t _magic_header = LUDP_MAGIC_HEADER;
    UDPHelper udp;

    bool check_udp() {
      if(!_udp_initialized) {
        udp.set_magic_header(_magic_header);
        if (udp.begin(_port)) _udp_initialized = true;
      }
      return _udp_initialized;
    };

public:
    /* Returns the suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      #ifdef PJON_ESP
        return 10000ul*attempts + random(10000);
      #else
        return 1;
      #endif
    };


    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) { return check_udp(); };


    /* Check if the channel is free for transmission */

    bool can_start() { return check_udp(); };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() { return 10; };


    /* Handle a collision (empty because handled on Ethernet level): */

    void handle_collision() { };


    /* Receive a string: */

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      return udp.receive_string(string, max_length);
    }


    /* Receive byte response */

    int16_t receive_response() {
      /* TODO: Improve robustness by ignoring packets not from the previous
         receiver (Perhaps not that important as long as ACK/NAK responses are
         directed, not broadcast) */
      uint32_t start = PJON_MICROS();
      uint8_t result[6];
      uint16_t reply_length = 0;
      do {
        reply_length = receive_string(result, sizeof result);
        // We expect 1, if packet is larger it is not our ACK.
        // When an ACK is received we know it is for us because an ACK
        // will never be broadcast but directed.
        if(reply_length == 1)
          if(result[0] == PJON_ACK)
            return result[0];
     } while ((uint32_t)(PJON_MICROS() - start) < LUDP_RESPONSE_TIMEOUT);
      return PJON_FAIL;
    };


    /* Send byte response to package transmitter.
       We have the IP so we can skip broadcasting and reply directly. */

    void send_response(uint8_t response) { // Empty, PJON_ACK is always sent
      udp.send_response(response);
    };


    /* Send a string: */

    void send_string(uint8_t *string, uint16_t length) {
      udp.send_string(string, length);
    };


    /* Set the UDP port: */

    void set_port(uint16_t port = LUDP_DEFAULT_PORT) {
      _port = port;
    };
};
