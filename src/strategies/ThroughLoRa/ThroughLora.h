
/* ThroughLora data link
   Proposed and developed by Matheus Eduardo Garbelini 2017-2018
  ____________________________________________________________________________

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

	 To use this strategy, please download the LoRa third party Library from
   https://github.com/sandeepmistry/arduino-LoRa/ */

#pragma once

#include <LoRa.h>
#include "Timing.h"

// Recommended receive time for this strategy, in microseconds
#ifndef TL_RECEIVE_TIME
  #define TL_RECEIVE_TIME 0
#endif

class ThroughLora {
  public:
    /* Returns the suggested delay related to the attempts
       passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      uint32_t result = attempts;
      for(uint8_t d = 0; d < TL_BACK_OFF_DEGREE; d++)
        result *= (uint32_t)(attempts);
      return result;
    };

    int packetRssi() {
      return LoRa.packetRssi();
    };

    float packetSnr() {
      return LoRa.packetSnr();
    };

    bool setFrequency(uint32_t frequency) {
      return LoRa.begin(frequency);
    };

    void setSignalBandwidth(uint32_t bandwidth) {
      LoRa.setSignalBandwidth(bandwidth);
    };

    void setPins(uint8_t cs_pin, uint8_t reset_pin, uint8_t dio0_pin) {
      LoRa.setPins(cs_pin, reset_pin, dio0_pin);
    };

    void setSpreadingFactor(uint8_t spreadingFactor) {
      LoRa.setSpreadingFactor(spreadingFactor);
    };

    void setCodingRate4(uint8_t codingRate) {
      LoRa.setCodingRate4(codingRate);
    };

    void setPreambleLength(uint16_t preambleLength) {
      LoRa.setPreambleLength(preambleLength);
    };

    void setSyncWord(uint8_t syncWord) {
      LoRa.setSyncWord(syncWord);
    };

    void setCRC(bool enableCRC) {
      if (enableCRC) LoRa.enableCrc();
      else LoRa.disableCrc();
    };

    void setTxPower(uint8_t txPower) {
      LoRa.setTxPower(txPower);
    };

    void setTxPower(uint8_t txPower, uint8_t boostPin) {
      LoRa.setTxPower(txPower, boostPin);
    };

    void idle() {
      LoRa.idle();
    };

    void sleep() {
      LoRa.sleep();
    };

    uint8_t getRandom() {
      return LoRa.random();
    };

    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t did = 0) {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TL_INITIAL_DELAY) + did);
      return true;
    };

    /* Check if the channel is free for transmission: */

    bool can_start() {
      if(LoRa.parsePacket() > 0) return false;
      return true;
    };

    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() {
      return TL_MAX_ATTEMPTS;
    };

    /* Returns the recommended receive time for this strategy: */

    static uint16_t get_receive_time() {
      return TL_RECEIVE_TIME;
    };

    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TL_COLLISION_DELAY));
    };

    /* The last 5 bytes of the frame are used as a unique identifier within
       the response. PJON has CRC8 or CRC32 at the end of the packet, encoding
       a CRC (that is a good hashing algorithm) and using 40 bits looks enough
       to provide a relatively safe response that should be nearly flawless
       (yield few false positives per millennia). */

    void prepare_response(const uint8_t *buffer, uint16_t position) {
      for(int8_t i = 0; i < TL_RESPONSE_LENGTH; i++)
        _response[i] =
          buffer[(position - ((TL_RESPONSE_LENGTH - 1) - i)) - 1];
    };

    /* Receive byte response (not supported) */

    uint16_t receive_response() {
      uint32_t time = PJON_MICROS();
      while((uint32_t)(PJON_MICROS() - time) < TL_RESPONSE_TIME_OUT) {
        uint8_t frame_size = LoRa.parsePacket();
        if(frame_size == TL_RESPONSE_LENGTH) {
          for(uint8_t i = 0; i < TL_RESPONSE_LENGTH; i++)
            if(LoRa.read() != _response[i]) return PJON_FAIL;
          return PJON_ACK;
        }
      }
      return PJON_FAIL;
    };

    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      uint8_t frameSize = LoRa.parsePacket();
      /* Filter frames if too short to contain a PJON packet
         or if too long to be received */
      if((frameSize > 5) && (frameSize <= max_length)) {
        while(LoRa.available()) {
          *data = LoRa.read();
          data++;
        }
        prepare_response(data - frameSize, frameSize);
        return frameSize;
      } else return PJON_FAIL;
    };

      /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      LoRa.write(b);
    };

    /* Send byte response to the packet's transmitter (not supported) */

    void send_response(uint8_t response) {
      if(response == PJON_ACK) {
        start_tx();
        #if TL_RESPONSE_DELAY != 0
        PJON_DELAY(TL_RESPONSE_DELAY);    
        #endif
        for(uint8_t i = 0; i < TL_RESPONSE_LENGTH; i++)
          send_byte(_response[i]);
        end_tx();
      }
    };

    /* Send a frame: */

    void send_frame(uint8_t *data, uint8_t length) {
      start_tx();
      for(uint8_t b = 0; b < length; b++)
        send_byte(data[b]);
      prepare_response(data, length);
      end_tx();
    };

    void start_tx() {
      LoRa.beginPacket();
    };

    void end_tx() {
      LoRa.endPacket();
    };

  private:
    uint32_t _last_reception_time;
    uint8_t  _response[TL_RESPONSE_LENGTH];
};
