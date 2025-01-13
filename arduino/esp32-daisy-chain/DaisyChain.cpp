#include "DaisyChain.h"

void DaisyChain::initialize() {
  chain0_.begin();
  chain0_.write();
  chain1_.begin();
  chain1_.write();

  // Set default brightness
  for (uint8_t tlc_idx = 0; tlc_idx < CHAIN_SIZE; tlc_idx++) {
    for (uint8_t led_idx = 0; led_idx < LED_COUNT; led_idx++) {
      current_brightness0_[tlc_idx][led_idx] = static_cast<BrgNumber>(DEFAULT_BRIGHTNESS_CHAIN0[tlc_idx][led_idx]);
      current_brightness1_[tlc_idx][led_idx] = static_cast<BrgNumber>(DEFAULT_BRIGHTNESS_CHAIN1[tlc_idx][led_idx]);
    }
  }

  flush_chain(ChainIdx::CHAIN_0);
  flush_chain(ChainIdx::CHAIN_1);
}

void DaisyChain::flush_chain(ChainIdx idx) {
  Adafruit_TLC59711* chain = nullptr;
  BrgNumber(*current_brightness)[CHAIN_SIZE][LED_COUNT] = nullptr;

  if (idx == ChainIdx::CHAIN_0) {
    chain = &chain0_;
    current_brightness = &current_brightness0_;
  } else {
    chain = &chain1_;
    current_brightness = &current_brightness1_;
  }

  for (uint8_t tlc_idx = 0; tlc_idx < CHAIN_SIZE; tlc_idx++) {
    for (uint8_t led_idx = 0; led_idx < (LED_COUNT / 3); led_idx++) {  // LED index is [0, 1, 2, 3]
      uint16_t led_number = tlc_idx * (LED_COUNT / 3) + led_idx;

      uint16_t ch_r = linearize_brightness((*current_brightness)[tlc_idx][led_idx * 3]);
      uint16_t ch_g = linearize_brightness((*current_brightness)[tlc_idx][led_idx * 3 + 1]);
      uint16_t ch_b = linearize_brightness((*current_brightness)[tlc_idx][led_idx * 3 + 2]);

      chain->setLED(led_number, ch_r, ch_g, ch_b);
    }
  }

  chain->write();
}

uint16_t DaisyChain::linearize_brightness(BrgNumber brightness) {
  return BRIGHTNESS_LINEARIZATION_TABLE[static_cast<int>(brightness)];
}

void DaisyChain::runTestShow() {
  int delay_ms = 100;

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 0;
    current_brightness0_[1][i] = 100;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 10;
    current_brightness0_[1][i] = 90;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 20;
    current_brightness0_[1][i] = 80;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 30;
    current_brightness0_[1][i] = 70;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 40;
    current_brightness0_[1][i] = 60;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 50;
    current_brightness0_[1][i] = 50;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 60;
    current_brightness0_[1][i] = 40;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 70;
    current_brightness0_[1][i] = 30;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 80;
    current_brightness0_[1][i] = 20;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 90;
    current_brightness0_[1][i] = 10;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    current_brightness0_[0][i] = 100;
    current_brightness0_[1][i] = 0;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
}