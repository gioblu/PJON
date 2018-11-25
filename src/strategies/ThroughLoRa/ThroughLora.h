
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

	void idle(uint8_t txPower, uint8_t boostPin) {
		LoRa.idle();
	};

	void sleep(uint8_t txPower, uint8_t boostPin) {
		LoRa.sleep();
	};

	uint8_t getRandom() {
		return LoRa.random();
	};

    /* Begin method, to be called before transmission or reception:
       (returns always true) */

    bool begin(uint8_t additional_randomness = 0) {
      PJON_DELAY_MICROSECONDS(
        PJON_RANDOM(TL_INITIAL_DELAY) + additional_randomness
      );
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

    /* Handle a collision: */

    void handle_collision() {
      PJON_DELAY_MICROSECONDS(PJON_RANDOM(TL_COLLISION_DELAY));
    };

    /* Receive byte response */

    uint16_t receive_response() {
      uint32_t time = PJON_MICROS();
      while((uint32_t)(PJON_MICROS() - time) < TL_RESPONSE_TIME_OUT) {
        uint8_t frame_size = LoRa.parsePacket();
        if(frame_size > 0) return LoRa.read();
      }
      return PJON_FAIL;
    };

    /* Receive a string: */

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {
      uint8_t frameSize = LoRa.parsePacket();
      if(frameSize > 0) {
        while(LoRa.available()) {
          *string = LoRa.read();
          string++;
        }
        return frameSize;
      } else return PJON_FAIL;
    };

      /* Send a byte and wait for its transmission end */

    void send_byte(uint8_t b) {
      LoRa.write(b);
    };

    /* Send byte response to the packet's transmitter */

    void send_response(uint8_t response) {
      start_tx();
      send_byte(response);
      end_tx();
    };

    /* Send a string: */

    void send_string(uint8_t *string, uint8_t length) {
      start_tx();
      for(uint8_t b = 0; b < length; b++)
        send_byte(string[b]);
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
};
