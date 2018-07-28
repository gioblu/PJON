/* ThroughSerial digital communication data link layer
   used as a Strategy by the PJON framework (included in version v4.1)

   Contributors:
    - Fred Larsen, Development, testing and debugging
    - Zbigniew Zasieczny, collision avoidance multi-drop RS485 (latency)
      and SoftwareSerial compatibility
    - Franketto (Arduino forum user) RS485 TX enable pin compatibility
   ____________________________________________________________________________

   ThroughSerial, Copyright (c) 2016 by Giovanni Blu Mitolo All rights reserved.

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
#ifndef TAS_INITIAL_DELAY
  #define TAS_INITIAL_DELAY      1000
#endif

/* Mamimum 32 microseconds random delay in case of collision */
#ifndef TAS_COLLISION_DELAY
  #define TAS_COLLISION_DELAY      32
#endif

/* Set 10 milliseconds as the maximum timeframe between transmission and
   synchronous acknowledgement response. This value is strictly related to the
   maximum time needed by receiver to receive, compute and transmit a response.
   Higher if necessary. */
#ifndef TAS_RESPONSE_TIME_OUT
  #define TAS_RESPONSE_TIME_OUT 10000
#endif

/* Minum timeframe with channel free for use before transmission.
   (Avoid disrupting synchronous acknowledgement exchange) */
#ifndef TAS_TIME_IN
  #define TAS_TIME_IN  TAS_RESPONSE_TIME_OUT + TAS_COLLISION_DELAY
#endif

/* Set 5 milliseconds as the maximum timeframe for byte reception.
   This value depends on the latency, baud rate and computation time.
   Always set TS_BYTE_TIME_OUT > (byte transmission time + latency) */
#ifndef TAS_BYTE_TIME_OUT
  #define TAS_BYTE_TIME_OUT      5000
#endif

/* Maximum transmission attempts */
#ifndef TAS_MAX_ATTEMPTS
  #define TAS_MAX_ATTEMPTS         20
#endif

/* Back-off exponential degree */
#ifndef TAS_BACK_OFF_DEGREE
  #define TAS_BACK_OFF_DEGREE      4
#endif
