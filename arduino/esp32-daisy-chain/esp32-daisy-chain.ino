/***************************************************
  This is an example for our Adafruit 12-channel PWM/LED driver

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/

  These drivers uses SPI to communicate, 2 pins are required to
  interface: Data and Clock. The boards are chainable

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <SPI.h>
#include "Adafruit_TLC59711.h"
#include "DaisyChain.h"
#include "Player.h"
#include "common.h"

#define DEBUG_ENABLE_MAIN 1
#if ((DEBUG_ENABLE_MAIN == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) Serial.printf("[INF][Main]: " f "\n", ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#endif

void test_play() {
  static LedObj leds[] = {
    { ChainIdx::CHAIN_0, 0, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 0, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 1, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 1, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 2, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 2, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 3, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 3, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 4, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 4, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 5, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 5, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 6, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 6, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 7, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 7, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 8, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 8, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 9, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 9, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 10, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 10, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 11, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 11, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 12, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 12, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 13, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 13, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 14, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 14, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 15, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 15, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 16, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 16, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 17, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 17, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 18, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 18, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_0, 19, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_0, 19, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
  };

  if (Player::getInstance().is_idle() == false) {
    DEBUG_INFO("Player is not idle, cannot play sequence step!");
    return;
  }

  size_t size = sizeof(leds) / sizeof(LedObj);
  DEBUG_INFO("Play test sequence step with %d LEDs", size);

  SequenceStep step = {
    .leds = leds,
    .size = size,
    .ramp_down_duration_ms = 500,
    .pause_duration_ms = 5000,
    .ramp_up_duration_ms = 500,
    .pulse_duration_ms = 5000,
    .repetitions = 1000,
    //.pause_brightness = static_cast<BrgNumber>(BrgName::BRG_OFF),
    //.pulse_brightness = static_cast<BrgNumber>(BrgName::BRG_MAX),
  };

  Player::getInstance().play(step);
}

void setup() {
  setCpuFrequencyMhz(240);

#if (ENABLE_DEBUG_OUTPUT == 1)
  Serial.begin(115200);
#endif
  DEBUG_INFO("%s", DIVIDER);
  DEBUG_INFO("Setup ESP32-daisy-chain [...]");
  DEBUG_INFO("CPU frequency: %d MHz", getCpuFrequencyMhz());

  DaisyChain::getInstance().initialize();
  Player::getInstance().initialize();

  test_play();

  DEBUG_INFO("Setup ESP32-daisy-chain [OK]");
  DEBUG_INFO("%s", DIVIDER);
}

void loop() {
  Player::getInstance().run();

  DaisyChain::getInstance().flush_chain(ChainIdx::CHAIN_0);
  DaisyChain::getInstance().flush_chain(ChainIdx::CHAIN_1);
  DaisyChain::getInstance().flush_chain(ChainIdx::CHAIN_2);
}
