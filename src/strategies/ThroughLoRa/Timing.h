/* ThroughLora data link layer
   Proposed and developed by Matheus Eduardo Garbelini
   ____________________________________________________________________________

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
#ifndef TL_INITIAL_DELAY
  #define TL_INITIAL_DELAY      1000
#endif

/* Maximum 32 microseconds random delay in case of collision */
#ifndef TL_COLLISION_DELAY
  #define TL_COLLISION_DELAY      64
#endif

/* Set 100 milliseconds as the maximum timeframe between transmission and
   synchronous acknowledgement response. This value is strictly related to the
   maximum time needed by receiver to receive, compute and transmit a response.
   Higher if necessary. */

#ifndef TL_RESPONSE_TIME_OUT
  #define TL_RESPONSE_TIME_OUT 100000
#endif

/* Set an optional delay (in ms) before responding if the receiver is particularly
   slow at swapping between transmit and receive modes. */
#ifndef TL_RESPONSE_DELAY
  #define TL_RESPONSE_DELAY 0
#endif

/* Maximum transmission attempts (re-transmission not supported) */
#ifndef TL_MAX_ATTEMPTS
  #define TL_MAX_ATTEMPTS           5
#endif

/* Back-off exponential degree (re-transmission not supported) */
#ifndef TL_BACK_OFF_DEGREE
  #define TL_BACK_OFF_DEGREE        5
#endif

/* Response length (the response is composed by the last TL_RESPONSE_LENGTH
   bytes of the packet received). By default should be relatively safe.
   (Few false positives per millennia)
   If you are ready to trade safety for bandwidth reduce it, consider that
   setting TL_RESPONSE_LENGTH < 4 reduces reliability and leads to higher
   chances of detecting a false positive. */
#ifndef TL_RESPONSE_LENGTH
  #define TL_RESPONSE_LENGTH       5
#endif
