#include "DaisyChain.h"
#include <Preferences.h>

#define DEBUG_ENABLE_DAISYCHAIN 1
#if ((DEBUG_ENABLE_DAISYCHAIN == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) Serial.printf("[INF][DaisyChain]: " f "\n", ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#endif

void DaisyChain::initialize() {
  DEBUG_INFO("Initialize DaisyChain [...]");
  chain0_.begin();
  chain0_.write();
  chain1_.begin();
  chain1_.write();

  if (load_calibrated_values() == false) {
    DEBUG_INFO("Calibrated values not found, loading default values!");
    load_default_values();
    save_calibrated_values();
  }
  apply_calibrated_values();

  flush_chain(ChainIdx::CHAIN_0);
  flush_chain(ChainIdx::CHAIN_1);
  DEBUG_INFO("Initialize DaisyChain [OK]");
}

bool DaisyChain::load_calibrated_values() {
  /* NVS storage layout:
   * "calibration" namespace:
   * - "format_version" (uint8_t)
   * - "calib_name" (string)
   * - "calib_chain0" (uint8_t[CHAIN_SIZE][LED_COUNT])
   * - "calib_chain1" (uint8_t[CHAIN_SIZE][LED_COUNT])
   */
  Preferences preferences;
  preferences.begin("calibration", false);  // open (create if needed) the namespace in RW mode

  if (preferences.isKey("format_version") == false) {
    preferences.end();
    return false;
  }
  uint8_t format_version = preferences.getUChar("format_version");
  if (format_version != CalibrationFormatVersion) {
    preferences.end();
    DEBUG_INFO("Calibration format version mismatch, migrating data ...");
    return migrate_calibration_data(format_version);
  }

  // Check if all keys are present
  if (preferences.isKey("calib_name") == false ||    //
      preferences.isKey("calib_chain0") == false ||  //
      preferences.isKey("calib_chain1") == false) {
    preferences.end();
    DEBUG_INFO("Calibration data incomplete!");
    return false;
  }

  preferences.getString("calib_name", calibration_name_, CalibrationNameMaxLength);

  size_t length0 = preferences.getBytesLength("calib_chain0");
  size_t length1 = preferences.getBytesLength("calib_chain1");

  if (length0 != sizeof(calibrated_brightness0_) ||  //
      length1 != sizeof(calibrated_brightness1_)) {
    preferences.end();
    return false;
  }

  preferences.getBytes("calib_chain0", calibrated_brightness0_, sizeof(calibrated_brightness0_));
  preferences.getBytes("calib_chain1", calibrated_brightness1_, sizeof(calibrated_brightness1_));

  preferences.end();
  DEBUG_INFO("Calibrated values loaded [OK]");
  return true;
}

void DaisyChain::save_calibrated_values() {
  Preferences preferences;
  preferences.begin("calibration", false);  // open (create if needed) the namespace in RW mode

  preferences.putUChar("format_version", CalibrationFormatVersion);
  preferences.putString("calib_name", calibration_name_);
  preferences.putBytes("calib_chain0", calibrated_brightness0_, sizeof(calibrated_brightness0_));
  preferences.putBytes("calib_chain1", calibrated_brightness1_, sizeof(calibrated_brightness1_));

  preferences.end();
  DEBUG_INFO("Calibrated values saved [OK]");
}

bool DaisyChain::migrate_calibration_data(uint8_t from_version) {
  // No migration needed for now
  return false;
}

void DaisyChain::load_default_values() {
  for (uint8_t tlc_idx = 0; tlc_idx < CHAIN_SIZE; tlc_idx++) {
    for (uint8_t led_idx = 0; led_idx < LED_COUNT; led_idx++) {
      calibrated_brightness0_[tlc_idx][led_idx] = static_cast<BrgNumber>(DEFAULT_BRIGHTNESS_CHAIN0[tlc_idx][led_idx]);
      calibrated_brightness1_[tlc_idx][led_idx] = static_cast<BrgNumber>(DEFAULT_BRIGHTNESS_CHAIN1[tlc_idx][led_idx]);
    }
  }
}

void DaisyChain::apply_calibrated_values() {
  memcpy(active_brightness0_, calibrated_brightness0_, sizeof(active_brightness0_));
  memcpy(active_brightness1_, calibrated_brightness1_, sizeof(active_brightness1_));
  chain0_changed_ = true;
  chain1_changed_ = true;
}

void DaisyChain::flush_chain(ChainIdx idx, bool force) {
  Adafruit_TLC59711* chain = nullptr;
  BrgNumber(*current_brightness)[CHAIN_SIZE][LED_COUNT] = nullptr;

  if (idx == ChainIdx::CHAIN_0) {
    if (chain0_changed_ == false && force == false) {
      return;
    }
    chain0_changed_ = false;
    chain = &chain0_;
    current_brightness = &active_brightness0_;

  } else {
    if (chain1_changed_ == false && force == false) {
      return;
    }
    chain1_changed_ = false;
    chain = &chain1_;
    current_brightness = &active_brightness1_;
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

void DaisyChain::get_active_leds(LedObj leds[], size_t size) const {
  for (size_t i = 0; i < size; i++) {
    ChainIdx chain_idx = leds[i].chain_idx;
    uint8_t pcb_idx = leds[i].pcb_idx;
    uint8_t led_idx = leds[i].led_idx;

    if (chain_idx == ChainIdx::CHAIN_0) {
      leds[i].brightness = active_brightness0_[pcb_idx][led_idx];
    } else {
      leds[i].brightness = active_brightness1_[pcb_idx][led_idx];
    }
  }
}

void DaisyChain::set_active_leds(LedObj leds[], size_t size) {
  for (size_t i = 0; i < size; i++) {
    ChainIdx chain_idx = leds[i].chain_idx;
    uint8_t pcb_idx = leds[i].pcb_idx;
    uint8_t led_idx = leds[i].led_idx;
    BrgNumber brightness = leds[i].brightness;

    if (chain_idx == ChainIdx::CHAIN_0) {
      active_brightness0_[pcb_idx][led_idx] = brightness;
      chain0_changed_ = true;
    } else {
      active_brightness1_[pcb_idx][led_idx] = brightness;
      chain1_changed_ = true;
    }
  }
}

void DaisyChain::get_calibrated_leds(LedObj leds[], size_t size) const {
  for (size_t i = 0; i < size; i++) {
    ChainIdx chain_idx = leds[i].chain_idx;
    uint8_t pcb_idx = leds[i].pcb_idx;
    uint8_t led_idx = leds[i].led_idx;

    if (chain_idx == ChainIdx::CHAIN_0) {
      leds[i].brightness = calibrated_brightness0_[pcb_idx][led_idx];
    } else {
      leds[i].brightness = calibrated_brightness1_[pcb_idx][led_idx];
    }
  }
}

/*
void DaisyChain::runTestShow() {
  int delay_ms = 100;

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 0;
    active_brightness0_[1][i] = 100;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 10;
    active_brightness0_[1][i] = 90;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 20;
    active_brightness0_[1][i] = 80;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);

  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 30;
    active_brightness0_[1][i] = 70;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 40;
    active_brightness0_[1][i] = 60;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 50;
    active_brightness0_[1][i] = 50;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 60;
    active_brightness0_[1][i] = 40;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 70;
    active_brightness0_[1][i] = 30;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 80;
    active_brightness0_[1][i] = 20;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 90;
    active_brightness0_[1][i] = 10;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
  for (uint16_t i = 0; i < LED_COUNT; i++) {
    active_brightness0_[0][i] = 100;
    active_brightness0_[1][i] = 0;
  }
  flush_chain(ChainIdx::CHAIN_0);
  delay(delay_ms);
}
*/
