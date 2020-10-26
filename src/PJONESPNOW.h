
#pragma once

#ifndef PJON_INCLUDE_MAC
  #error ESPNOW strategy requires the MAC feature, define PJON_INCLUDE_MAC.
#endif

#include "PJON.h"

#include "strategies/ESPNOW/ESPNOW.h"

#define PJONESPNOW PJON<ESPNOW>
