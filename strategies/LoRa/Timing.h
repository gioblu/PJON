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
#ifndef TS_INITIAL_DELAY
  #define TS_INITIAL_DELAY      1000
#endif

/* Mamimum 32 microseconds random delay in case of collision */
#ifndef TS_COLLISION_DELAY
  #define TS_COLLISION_DELAY      32
#endif

/* Set 100 milliseconds as the maximum timeframe between transmission and
   synchronous acknowledgement response. This value is strictly related to the
   maximum time needed by receiver to receive, compute and transmit a response.
   Higher if necessary. */

  #define TS_RESPONSE_TIME_OUT 100000UL


/* Minum timeframe with channel free for use before transmission.
   (Avoid disrupting synchronous acknowledgement exchange) */
#ifndef TS_TIME_IN
  #define TS_TIME_IN  TS_RESPONSE_TIME_OUT + TS_COLLISION_DELAY
#endif

/* Set 5 milliseconds as the maximum timeframe for byte reception.
   This value depends on the latency, baud rate and computation time.
   Always set TS_BYTE_TIME_OUT > (byte transmission time + latency) */
#ifndef TS_BYTE_TIME_OUT
  #define TS_BYTE_TIME_OUT      5000
#endif

/* Maximum transmission attempts */
#ifndef TS_MAX_ATTEMPTS
  #define TS_MAX_ATTEMPTS         10
#endif

/* Back-off exponential degree */
#ifndef TS_BACK_OFF_DEGREE
  #define TS_BACK_OFF_DEGREE       3
#endif
