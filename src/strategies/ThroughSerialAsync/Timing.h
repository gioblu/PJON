/* ThroughSerial digital communication data link layer
   used as a Strategy by the PJON framework (included in version v4.1)

   Contributors:
    - Fred Larsen, Development, testing and debugging
    - Zbigniew Zasieczny, collision avoidance multi-drop RS485 (latency)
      and SoftwareSerial compatibility
    - Franketto (Arduino forum user) RS485 TX enable pin compatibility
   ____________________________________________________________________________

   ThroughSerialAsync timing, based on ThroughSerial, developed by sticilface
   copyright 2018 by Giovanni Blu Mitolo All rights reserved

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

/* Maximum 1 second random initial delay */
#ifndef TSA_INITIAL_DELAY
  #define TSA_INITIAL_DELAY      1000
#endif

/* Maximum 32 microseconds random delay in case of collision */
#ifndef TSA_COLLISION_DELAY
  #define TSA_COLLISION_DELAY      32
#endif

/* Set 10 milliseconds as the maximum timeframe between transmission and
   synchronous acknowledgement response. Its  optimal configuration is
   strictly related to the maximum time needed by receiver to receive, compute
   and transmit back a response. */
#ifndef TSA_RESPONSE_TIME_OUT
  #define TSA_RESPONSE_TIME_OUT 10000
#endif

/* Minum duration of channel free for use before transmission.
   (Avoid disrupting synchronous acknowledgement exchange) */
#ifndef TSA_TIME_IN
  #define TSA_TIME_IN  TSA_RESPONSE_TIME_OUT + TSA_COLLISION_DELAY
#endif

/* Set 100 microseconds as the interval between each byte read.
   Depending on the latency, baud rate and computation time the
   optimal TSA_READ_INTERVAL value may variate.
   Always set: TSA_READ_INTERVAL > (byte transmission time + latency) */
#ifndef TSA_READ_INTERVAL
  #define TSA_READ_INTERVAL       100
#endif

/* Byte reception timeout */
#ifndef TSA_BYTE_TIME_OUT
  #define TSA_BYTE_TIME_OUT   1000000
#endif

/* Response length (the response is composed by the last TSA_RESPONSE_LENGTH
   bytes of the packet received). By default should be relatively safe.
   (Few false positives per millennia)
   If you are ready to trade safety for bandwidth reduce it, consider that
   setting TSA_RESPONSE_LENGTH < 4 reduces reliability and leads to higher
   chances of detecting a false positive. */
#ifndef TSA_RESPONSE_LENGTH
  #define TSA_RESPONSE_LENGTH       5
#endif

/* Maximum transmission attempts */
#ifndef TSA_MAX_ATTEMPTS
  #define TSA_MAX_ATTEMPTS         20
#endif

/* Back-off exponential degree */
#ifndef TSA_BACK_OFF_DEGREE
  #define TSA_BACK_OFF_DEGREE       4
#endif

/* Delay before enabling and disabling RS485 DE and or RE pin */
#ifndef TSA_RS485_DELAY
  #define TSA_RS485_DELAY           1
#endif

/* Force blocking sending hack (adds a delay for each character sent). */

#ifndef TSA_FLUSH_OFFSET
  #define TSA_FLUSH_OFFSET 152
#endif
