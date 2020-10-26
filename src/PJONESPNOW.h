
#pragma once

/* ESPNOW strategy actively uses the PJON's MAC address feature for this reason
   it must be included. */

#ifndef PJON_INCLUDE_MAC
  #define PJON_INCLUDE_MAC
#endif

#include "PJON.h"

#include "strategies/ESPNOW/ESPNOW.h"

#define PJONESPNOW PJON<ESPNOW>
