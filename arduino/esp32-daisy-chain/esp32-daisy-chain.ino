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
    { ChainIdx::CHAIN_0, 0, 0, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 0, 2, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 0, 4, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 0, 6, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 0, 8, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 0, 10, static_cast<BrgNumber>(BrgName::BRG_MAX) },

    /*
    { ChainIdx::CHAIN_0, 1, 1, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 1, 3, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 1, 5, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 1, 7, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 1, 9, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 1, 11, static_cast<BrgNumber>(BrgName::BRG_MAX) },

    { ChainIdx::CHAIN_0, 2, 0, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 2, 2, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 2, 4, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 2, 6, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 2, 8, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 2, 10, static_cast<BrgNumber>(BrgName::BRG_MAX) },

    { ChainIdx::CHAIN_0, 3, 1, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 3, 3, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 3, 5, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 3, 7, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 3, 9, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    { ChainIdx::CHAIN_0, 3, 11, static_cast<BrgNumber>(BrgName::BRG_MAX) },
    */
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
    .pause_duration_ms = 500,
    .ramp_up_duration_ms = 500,
    .pulse_duration_ms = 500,
    .repetitions = 1000,
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
}
