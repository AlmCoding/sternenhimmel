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
    //{ ChainIdx::CHAIN_0, 0, 0, 0 },
    { ChainIdx::CHAIN_0, 0, 1, 0 },
    { ChainIdx::CHAIN_0, 0, 2, 0 },
    { ChainIdx::CHAIN_0, 0, 3, 0 },
    { ChainIdx::CHAIN_0, 0, 4, 0 },
    { ChainIdx::CHAIN_0, 0, 5, 0 },
    { ChainIdx::CHAIN_0, 0, 6, 0 },
    { ChainIdx::CHAIN_0, 0, 7, 0 },
    { ChainIdx::CHAIN_0, 0, 8, 0 },
    { ChainIdx::CHAIN_0, 0, 9, 0 },
    { ChainIdx::CHAIN_0, 0, 10, 0 },
    { ChainIdx::CHAIN_0, 0, 11, 0 },

    { ChainIdx::CHAIN_1, 0, 0, 0 },
    //{ ChainIdx::CHAIN_1, 0, 1, 0 },
    { ChainIdx::CHAIN_1, 0, 2, 0 },
    { ChainIdx::CHAIN_1, 0, 3, 0 },
    { ChainIdx::CHAIN_1, 0, 4, 0 },
    { ChainIdx::CHAIN_1, 0, 5, 0 },
    { ChainIdx::CHAIN_1, 0, 6, 0 },
    { ChainIdx::CHAIN_1, 0, 7, 0 },
    { ChainIdx::CHAIN_1, 0, 8, 0 },
    { ChainIdx::CHAIN_1, 0, 9, 0 },
    { ChainIdx::CHAIN_1, 0, 10, 0 },
    { ChainIdx::CHAIN_1, 0, 11, 0 },

    { ChainIdx::CHAIN_2, 0, 0, 0 },
    { ChainIdx::CHAIN_2, 0, 1, 0 },
    //{ ChainIdx::CHAIN_2, 0, 2, 0 },
    { ChainIdx::CHAIN_2, 0, 3, 0 },
    { ChainIdx::CHAIN_2, 0, 4, 0 },
    { ChainIdx::CHAIN_2, 0, 5, 0 },
    { ChainIdx::CHAIN_2, 0, 6, 0 },
    { ChainIdx::CHAIN_2, 0, 7, 0 },
    { ChainIdx::CHAIN_2, 0, 8, 0 },
    { ChainIdx::CHAIN_2, 0, 9, 0 },
    { ChainIdx::CHAIN_2, 0, 10, 0 },
    { ChainIdx::CHAIN_2, 0, 11, 0 },

    { ChainIdx::CHAIN_3, 0, 0, 0 },
    { ChainIdx::CHAIN_3, 0, 1, 0 },
    { ChainIdx::CHAIN_3, 0, 2, 0 },
    //{ ChainIdx::CHAIN_3, 0, 3, 0 },
    { ChainIdx::CHAIN_3, 0, 4, 0 },
    { ChainIdx::CHAIN_3, 0, 5, 0 },
    { ChainIdx::CHAIN_3, 0, 6, 0 },
    { ChainIdx::CHAIN_3, 0, 7, 0 },
    { ChainIdx::CHAIN_3, 0, 8, 0 },
    { ChainIdx::CHAIN_3, 0, 9, 0 },
    { ChainIdx::CHAIN_3, 0, 10, 0 },
    { ChainIdx::CHAIN_3, 0, 11, 0 },

    { ChainIdx::CHAIN_4, 0, 0, 0 },
    { ChainIdx::CHAIN_4, 0, 1, 0 },
    { ChainIdx::CHAIN_4, 0, 2, 0 },
    { ChainIdx::CHAIN_4, 0, 3, 0 },
    //{ ChainIdx::CHAIN_4, 0, 4, 0 },
    { ChainIdx::CHAIN_4, 0, 5, 0 },
    { ChainIdx::CHAIN_4, 0, 6, 0 },
    { ChainIdx::CHAIN_4, 0, 7, 0 },
    { ChainIdx::CHAIN_4, 0, 8, 0 },
    { ChainIdx::CHAIN_4, 0, 9, 0 },
    { ChainIdx::CHAIN_4, 0, 10, 0 },
    { ChainIdx::CHAIN_4, 0, 11, 0 },

    { ChainIdx::CHAIN_5, 0, 0, 0 },
    { ChainIdx::CHAIN_5, 0, 1, 0 },
    { ChainIdx::CHAIN_5, 0, 2, 0 },
    { ChainIdx::CHAIN_5, 0, 3, 0 },
    { ChainIdx::CHAIN_5, 0, 4, 0 },
    //{ ChainIdx::CHAIN_5, 0, 5, 0 },
    { ChainIdx::CHAIN_5, 0, 6, 0 },
    { ChainIdx::CHAIN_5, 0, 7, 0 },
    { ChainIdx::CHAIN_5, 0, 8, 0 },
    { ChainIdx::CHAIN_5, 0, 9, 0 },
    { ChainIdx::CHAIN_5, 0, 10, 0 },
    { ChainIdx::CHAIN_5, 0, 11, 0 },
    // spacer
    /*
    { ChainIdx::CHAIN_0, 1, 0, 0 },
    //{ ChainIdx::CHAIN_0, 1, 1, 0 },
    { ChainIdx::CHAIN_0, 1, 2, 0 },
    { ChainIdx::CHAIN_0, 1, 3, 0 },
    { ChainIdx::CHAIN_0, 1, 4, 0 },
    { ChainIdx::CHAIN_0, 1, 5, 0 },
    { ChainIdx::CHAIN_0, 1, 6, 0 },
    { ChainIdx::CHAIN_0, 1, 7, 0 },
    { ChainIdx::CHAIN_0, 1, 8, 0 },
    { ChainIdx::CHAIN_0, 1, 9, 0 },
    { ChainIdx::CHAIN_0, 1, 10, 0 },
    { ChainIdx::CHAIN_0, 1, 11, 0 },

    { ChainIdx::CHAIN_0, 2, 0, 0 },
    { ChainIdx::CHAIN_0, 2, 1, 0 },
    //{ ChainIdx::CHAIN_0, 2, 2, 0 },
    { ChainIdx::CHAIN_0, 2, 3, 0 },
    { ChainIdx::CHAIN_0, 2, 4, 0 },
    { ChainIdx::CHAIN_0, 2, 5, 0 },
    { ChainIdx::CHAIN_0, 2, 6, 0 },
    { ChainIdx::CHAIN_0, 2, 7, 0 },
    { ChainIdx::CHAIN_0, 2, 8, 0 },
    { ChainIdx::CHAIN_0, 2, 9, 0 },
    { ChainIdx::CHAIN_0, 2, 10, 0 },
    { ChainIdx::CHAIN_0, 2, 11, 0 },

    { ChainIdx::CHAIN_0, 3, 0, 0 },
    { ChainIdx::CHAIN_0, 3, 1, 0 },
    { ChainIdx::CHAIN_0, 3, 2, 0 },
    //{ ChainIdx::CHAIN_0, 3, 3, 0 },
    { ChainIdx::CHAIN_0, 3, 4, 0 },
    { ChainIdx::CHAIN_0, 3, 5, 0 },
    { ChainIdx::CHAIN_0, 3, 6, 0 },
    { ChainIdx::CHAIN_0, 3, 7, 0 },
    { ChainIdx::CHAIN_0, 3, 8, 0 },
    { ChainIdx::CHAIN_0, 3, 9, 0 },
    { ChainIdx::CHAIN_0, 3, 10, 0 },
    { ChainIdx::CHAIN_0, 3, 11, 0 },

    { ChainIdx::CHAIN_0, 4, 0, 0 },
    { ChainIdx::CHAIN_0, 4, 1, 0 },
    { ChainIdx::CHAIN_0, 4, 2, 0 },
    { ChainIdx::CHAIN_0, 4, 3, 0 },
    //{ ChainIdx::CHAIN_0, 4, 4, 0 },
    { ChainIdx::CHAIN_0, 4, 5, 0 },
    { ChainIdx::CHAIN_0, 4, 6, 0 },
    { ChainIdx::CHAIN_0, 4, 7, 0 },
    { ChainIdx::CHAIN_0, 4, 8, 0 },
    { ChainIdx::CHAIN_0, 4, 9, 0 },
    { ChainIdx::CHAIN_0, 4, 10, 0 },
    { ChainIdx::CHAIN_0, 4, 11, 0 },

    { ChainIdx::CHAIN_0, 5, 0, 0 },
    { ChainIdx::CHAIN_0, 5, 1, 0 },
    { ChainIdx::CHAIN_0, 5, 2, 0 },
    { ChainIdx::CHAIN_0, 5, 3, 0 },
    { ChainIdx::CHAIN_0, 5, 4, 0 },
    //{ ChainIdx::CHAIN_0, 5, 5, 0 },
    { ChainIdx::CHAIN_0, 5, 6, 0 },
    { ChainIdx::CHAIN_0, 5, 7, 0 },
    { ChainIdx::CHAIN_0, 5, 8, 0 },
    { ChainIdx::CHAIN_0, 5, 9, 0 },
    { ChainIdx::CHAIN_0, 5, 10, 0 },
    { ChainIdx::CHAIN_0, 5, 11, 0 },

    { ChainIdx::CHAIN_0, 6, 0, 0 },
    { ChainIdx::CHAIN_0, 6, 1, 0 },
    { ChainIdx::CHAIN_0, 6, 2, 0 },
    { ChainIdx::CHAIN_0, 6, 3, 0 },
    { ChainIdx::CHAIN_0, 6, 4, 0 },
    { ChainIdx::CHAIN_0, 6, 5, 0 },
    //{ ChainIdx::CHAIN_0, 6, 6, 0 },
    { ChainIdx::CHAIN_0, 6, 7, 0 },
    { ChainIdx::CHAIN_0, 6, 8, 0 },
    { ChainIdx::CHAIN_0, 6, 9, 0 },
    { ChainIdx::CHAIN_0, 6, 10, 0 },
    { ChainIdx::CHAIN_0, 6, 11, 0 },

    { ChainIdx::CHAIN_0, 7, 0, 0 },
    { ChainIdx::CHAIN_0, 7, 1, 0 },
    { ChainIdx::CHAIN_0, 7, 2, 0 },
    { ChainIdx::CHAIN_0, 7, 3, 0 },
    { ChainIdx::CHAIN_0, 7, 4, 0 },
    { ChainIdx::CHAIN_0, 7, 5, 0 },
    { ChainIdx::CHAIN_0, 7, 6, 0 },
    //{ ChainIdx::CHAIN_0, 7, 7, 0 },
    { ChainIdx::CHAIN_0, 7, 8, 0 },
    { ChainIdx::CHAIN_0, 7, 9, 0 },
    { ChainIdx::CHAIN_0, 7, 10, 0 },
    { ChainIdx::CHAIN_0, 7, 11, 0 },

    { ChainIdx::CHAIN_0, 8, 0, 0 },
    { ChainIdx::CHAIN_0, 8, 1, 0 },
    { ChainIdx::CHAIN_0, 8, 2, 0 },
    { ChainIdx::CHAIN_0, 8, 3, 0 },
    { ChainIdx::CHAIN_0, 8, 4, 0 },
    { ChainIdx::CHAIN_0, 8, 5, 0 },
    { ChainIdx::CHAIN_0, 8, 6, 0 },
    { ChainIdx::CHAIN_0, 8, 7, 0 },
    //{ ChainIdx::CHAIN_0, 8, 8, 0 },
    { ChainIdx::CHAIN_0, 8, 9, 0 },
    { ChainIdx::CHAIN_0, 8, 10, 0 },
    { ChainIdx::CHAIN_0, 8, 11, 0 },
    */
    // spacer
    { ChainIdx::CHAIN_0, 9, 0, 0 },
    { ChainIdx::CHAIN_0, 9, 1, 0 },
    { ChainIdx::CHAIN_0, 9, 2, 0 },
    { ChainIdx::CHAIN_0, 9, 3, 0 },
    { ChainIdx::CHAIN_0, 9, 4, 0 },
    { ChainIdx::CHAIN_0, 9, 5, 0 },
    { ChainIdx::CHAIN_0, 9, 6, 0 },
    { ChainIdx::CHAIN_0, 9, 7, 0 },
    { ChainIdx::CHAIN_0, 9, 8, 0 },
    //{ ChainIdx::CHAIN_0, 9, 9, 0 },
    { ChainIdx::CHAIN_0, 9, 10, 0 },
    { ChainIdx::CHAIN_0, 9, 11, 0 },

    { ChainIdx::CHAIN_1, 9, 0, 0 },
    { ChainIdx::CHAIN_1, 9, 1, 0 },
    { ChainIdx::CHAIN_1, 9, 2, 0 },
    { ChainIdx::CHAIN_1, 9, 3, 0 },
    { ChainIdx::CHAIN_1, 9, 4, 0 },
    { ChainIdx::CHAIN_1, 9, 5, 0 },
    { ChainIdx::CHAIN_1, 9, 6, 0 },
    { ChainIdx::CHAIN_1, 9, 7, 0 },
    { ChainIdx::CHAIN_1, 9, 8, 0 },
    //{ ChainIdx::CHAIN_1, 9, 9, 0 },
    { ChainIdx::CHAIN_1, 9, 10, 0 },
    { ChainIdx::CHAIN_1, 9, 11, 0 },

    { ChainIdx::CHAIN_2, 9, 0, 0 },
    { ChainIdx::CHAIN_2, 9, 1, 0 },
    { ChainIdx::CHAIN_2, 9, 2, 0 },
    { ChainIdx::CHAIN_2, 9, 3, 0 },
    { ChainIdx::CHAIN_2, 9, 4, 0 },
    { ChainIdx::CHAIN_2, 9, 5, 0 },
    { ChainIdx::CHAIN_2, 9, 6, 0 },
    { ChainIdx::CHAIN_2, 9, 7, 0 },
    { ChainIdx::CHAIN_2, 9, 8, 0 },
    //{ ChainIdx::CHAIN_2, 9, 9, 0 },
    { ChainIdx::CHAIN_2, 9, 10, 0 },
    { ChainIdx::CHAIN_2, 9, 11, 0 },

    { ChainIdx::CHAIN_3, 9, 0, 0 },
    { ChainIdx::CHAIN_3, 9, 1, 0 },
    { ChainIdx::CHAIN_3, 9, 2, 0 },
    { ChainIdx::CHAIN_3, 9, 3, 0 },
    { ChainIdx::CHAIN_3, 9, 4, 0 },
    { ChainIdx::CHAIN_3, 9, 5, 0 },
    { ChainIdx::CHAIN_3, 9, 6, 0 },
    { ChainIdx::CHAIN_3, 9, 7, 0 },
    { ChainIdx::CHAIN_3, 9, 8, 0 },
    //{ ChainIdx::CHAIN_3, 9, 9, 0 },
    { ChainIdx::CHAIN_3, 9, 10, 0 },
    { ChainIdx::CHAIN_3, 9, 11, 0 },

    { ChainIdx::CHAIN_4, 9, 0, 0 },
    { ChainIdx::CHAIN_4, 9, 1, 0 },
    { ChainIdx::CHAIN_4, 9, 2, 0 },
    { ChainIdx::CHAIN_4, 9, 3, 0 },
    { ChainIdx::CHAIN_4, 9, 4, 0 },
    { ChainIdx::CHAIN_4, 9, 5, 0 },
    { ChainIdx::CHAIN_4, 9, 6, 0 },
    { ChainIdx::CHAIN_4, 9, 7, 0 },
    { ChainIdx::CHAIN_4, 9, 8, 0 },
    //{ ChainIdx::CHAIN_4, 9, 9, 0 },
    { ChainIdx::CHAIN_4, 9, 10, 0 },
    { ChainIdx::CHAIN_4, 9, 11, 0 },

    { ChainIdx::CHAIN_5, 9, 0, 0 },
    { ChainIdx::CHAIN_5, 9, 1, 0 },
    { ChainIdx::CHAIN_5, 9, 2, 0 },
    { ChainIdx::CHAIN_5, 9, 3, 0 },
    { ChainIdx::CHAIN_5, 9, 4, 0 },
    { ChainIdx::CHAIN_5, 9, 5, 0 },
    { ChainIdx::CHAIN_5, 9, 6, 0 },
    { ChainIdx::CHAIN_5, 9, 7, 0 },
    { ChainIdx::CHAIN_5, 9, 8, 0 },
    //{ ChainIdx::CHAIN_5, 9, 9, 0 },
    { ChainIdx::CHAIN_5, 9, 10, 0 },
    { ChainIdx::CHAIN_5, 9, 11, 0 },
  };

  if (Player::getInstance().is_idle() == false) {
    DEBUG_INFO("Player is not idle, cannot play sequence step!");
    return;
  }

  size_t size = sizeof(leds) / sizeof(LedObj);
  DEBUG_INFO("Play test sequence step with %d LEDs", size);

  static SequenceStep steps[2];
  SequenceStep step = {
    .leds = leds,
    .size = size,
    .ramp_down_duration_ms = 500,
    .pause_duration_ms = 500,
    .ramp_up_duration_ms = 500,
    .pulse_duration_ms = 500,
    .repetitions = 5,
    //.pause_brightness = static_cast<BrgNumber>(BrgName::BRG_OFF),
    //.pulse_brightness = static_cast<BrgNumber>(BrgName::BRG_MAX),
    .idle_return = false,
  };
  steps[0] = step;
  steps[1] = step;
  steps[1].pause_duration_ms = 0;
  steps[1].pulse_duration_ms = 0;
  steps[1].repetitions = 10;
  steps[1].idle_return = true;

  Player::getInstance().play_sequence(steps, 2);
}

void setup() {
  setCpuFrequencyMhz(240);

#if (ENABLE_DEBUG_OUTPUT == 1)
  Serial.begin(115200);
#endif
  DEBUG_INFO("%s", DIVIDER);
  DEBUG_INFO("Setup ESP32-daisy-chain [...]");
  DEBUG_INFO("CPU frequency: %d MHz", getCpuFrequencyMhz());
  sleep(0.5);

  DaisyChain::getInstance().initialize();
  Player::getInstance().initialize();

  DEBUG_INFO("Setup ESP32-daisy-chain [OK]");
  DEBUG_INFO("%s", DIVIDER);

  test_play();
}

void loop() {
  Player::getInstance().run();
  DaisyChain::getInstance().flush_all();
}
