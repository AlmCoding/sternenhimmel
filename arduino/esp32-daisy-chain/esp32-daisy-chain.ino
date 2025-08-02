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
    //{ ChainIdx::CHAIN_0, 0, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
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

    { ChainIdx::CHAIN_1, 0, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    //{ ChainIdx::CHAIN_1, 0, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_1, 0, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_2, 0, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    //{ ChainIdx::CHAIN_2, 0, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_2, 0, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_3, 0, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    //{ ChainIdx::CHAIN_3, 0, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_3, 0, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_4, 0, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    //{ ChainIdx::CHAIN_4, 0, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_4, 0, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

    { ChainIdx::CHAIN_5, 0, 0, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 1, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 2, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 3, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 4, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    //{ ChainIdx::CHAIN_5, 0, 5, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 6, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 7, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 8, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 9, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 10, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },
    { ChainIdx::CHAIN_5, 0, 11, static_cast<BrgNumber>(BrgName::BRG_MEDIUM) },

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
    .pause_duration_ms = 0,
    .ramp_up_duration_ms = 500,
    .pulse_duration_ms = 0,
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
  DaisyChain::getInstance().flush_chain(ChainIdx::CHAIN_3);
  DaisyChain::getInstance().flush_chain(ChainIdx::CHAIN_4);
  DaisyChain::getInstance().flush_chain(ChainIdx::CHAIN_5);
}
