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


#define TS_MAX_BYTE_TIME         100000
/* Maximum timeframe between every receive call in any of the connected devices.
   If this timeframe is in average exceeded by some of the connected devices
   communication reliability could drop or be disrupted. */

#define TS_FREE_TIME_BEFORE_START   500
/* 0.5 milliseconds minimum timeframe of free port before transmitting

   The proposed default timing configuration is ok for a master-slave setup, but
   could lead to collisions if used in a multi-master setup.

   If using ThroughSerial in a multi-master setup with synchronous acknowledgment
   NEVER set TS_FREE_TIME_BEFORE_START < TS_MAX_BYTE_TIME or a device could start
   transmitting while a couple is still exchanging an acknowledge

   i.e.
   #define THROUGH_SERIAL_MAX_BYTE_TIME           100000
   #define THROUGH_SERIAL_FREE_TIME_BEFORE_START  110000

   Above is shown multi-master compatible setup able to receive a synchronous
   acknowledgment with a maximum delay 100 milliseconds. Channel analysis before
   transmission is set to 110 milliseconds to avoid collisions.

   Which is the correct value for your setup depends on the maximum average time
   interval between every receive call in your system. TS_MAX_BYTE_TIME
   should be around the same duration. So in a sketch where there is only a
   delay(10) between every receive call at least 10000 should be the correct
   value for TS_MAX_BYTE_TIME.

   If your tasks timing are long and a satisfactory setup can't be reached
   consider to drop the use of the synchronous acknowledge and start using the
   asynchronous acknowledgment instead. */


/* Maximum 1 second random initial delay */
#ifndef TS_INITIAL_DELAY
  #define TS_INITIAL_DELAY            1000
#endif

/* Mamimum 16 microseconds random delay in case of collision */
#ifndef TS_COLLISION_DELAY
  #define TS_COLLISION_DELAY            16
#endif

/* Maximum transmission attempts */
#ifndef TS_MAX_ATTEMPTS
  #define TS_MAX_ATTEMPTS 20
#endif

/* Back-off exponential degree */
#ifndef TS_BACK_OFF_DEGREE
  #define TS_BACK_OFF_DEGREE 4
#endif
