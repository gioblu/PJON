/* ThroughSerial digital communication data link layer
   used as a Strategy by the PJON framework (included in version v4.1)

   Contributors:
    - Fred Larsen, Development, testing and debugging
    - Zbigniew Zasieczny, collision avoidance multi-drop RS485 (latency)
      and SoftwareSerial compatibility
    - Franketto (Arduino forum user) RS485 TX enable pin compatibility
   ____________________________________________________________________________

   Based on ThroughSerial, developed by sticilface
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
#ifndef TS_INITIAL_DELAY
  #define TS_INITIAL_DELAY      1000
#endif

/* Maximum 64 microseconds random delay in case of collision */
#ifndef TS_COLLISION_DELAY
  #define TS_COLLISION_DELAY      64
#endif

/* Set 45 milliseconds as the maximum timeframe between transmission and
   synchronous acknowledgement response. Its  optimal configuration is
   strictly related to the maximum time needed by receiver to receive, compute
   and transmit back a response. Set TS_RESPONSE_TIME_OUT to 0 and do not use
   the acknowledgement feature if the system operates in master-slave mode and
   or applies the request-response scheme. */
#ifndef TS_RESPONSE_TIME_OUT
  #define TS_RESPONSE_TIME_OUT 45000
#endif

/* Minum duration of channel free for use before transmission, used to avoid
   disrupting an ongoing acknowledgement exchange. Set TS_TIME_IN to 0 and do
   not use the acknowledgement feature if the system operates in master-slave
   mode and or applies the request-response scheme. */
#ifndef TS_TIME_IN
  #define TS_TIME_IN  TS_RESPONSE_TIME_OUT + TS_COLLISION_DELAY
#endif

/* Set 100 microseconds as the interval between each byte read.
   Depending on the latency, baud rate and computation time the
   optimal TS_READ_INTERVAL value may variate.
   Always set: TS_READ_INTERVAL > (byte transmission time + latency) */
#ifndef TS_READ_INTERVAL
  #define TS_READ_INTERVAL       100
#endif

/* Byte reception timeout (Default 1 second) */
#ifndef TS_BYTE_TIME_OUT
  #define TS_BYTE_TIME_OUT   1000000
#endif

/* Response length (the response is composed by the last TS_RESPONSE_LENGTH
   bytes of the packet received). Setting TS_RESPONSE_LENGTH < 4 when using
   ThroughSerial in multi-master mode reduces reliability and leads to higher
   chances of detecting a false positive. */
#ifndef TS_RESPONSE_LENGTH
  #define TS_RESPONSE_LENGTH       1
#endif

/* Maximum transmission attempts */
#ifndef TS_MAX_ATTEMPTS
  #define TS_MAX_ATTEMPTS         20
#endif

/* Back-off exponential degree */
#ifndef TS_BACK_OFF_DEGREE
  #define TS_BACK_OFF_DEGREE       4
#endif

/* Delay before enabling and disabling RS485 DE and or RE pin */
#ifndef TS_RS485_DELAY
  #define TS_RS485_DELAY           1
#endif

/* Force blocking sending hack (adds a delay for each character sent). */

#ifndef TS_FLUSH_OFFSET
  #define TS_FLUSH_OFFSET 152
#endif
