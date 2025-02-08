#ifndef DAISY_CHAIN_H
#define DAISY_CHAIN_H

#include <SPI.h>
#include "Adafruit_TLC59711.h"
#include "common.h"

class DaisyChain {
 public:
  DaisyChain(const DaisyChain&) = delete;
  DaisyChain& operator=(const DaisyChain&) = delete;

  static DaisyChain& getInstance() {
    static DaisyChain instance;
    return instance;
  }

  void initialize();
  void get_active_leds(LedObj leds[], size_t size) const;
  void set_active_leds(LedObj leds[], size_t size);
  void get_calibrated_leds(LedObj leds[], size_t size) const;
  void apply_calibrated_values();
  void flush_chain(ChainIdx idx, bool force = false);

 private:
  constexpr static uint8_t CalibrationFormatVersion = 0;
  constexpr static size_t CalibrationNameMaxLength = 32;
  // HSPI pins
  constexpr static int SpiClockPin = 14;
  constexpr static int SpiDataPin = 13;
  constexpr static int Chain0SelectPin = 15;
  constexpr static int Chain1SelectPin = 2;
  constexpr static int Chain2SelectPin = 4;
  // VSPI pins
  // constexpr static int Clock1Gpio = 18;
  // constexpr static int Data1Gpio = 23;

  DaisyChain() : chain_(CHAIN_SIZE, SpiClockPin, SpiDataPin) {}

  void select_chain(ChainIdx idx);
  bool load_calibrated_values();
  void save_calibrated_values();
  bool migrate_calibration_data(uint8_t from_version);
  void load_default_values();
  uint16_t linearize_brightness(BrgNumber brightness);

  BrgNumber active_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness1_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness2_[CHAIN_SIZE][LED_COUNT];

  char calibration_name_[CalibrationNameMaxLength] = "default";
  BrgNumber calibrated_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness1_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness2_[CHAIN_SIZE][LED_COUNT];

  Adafruit_TLC59711 chain_;
  bool chain0_changed_ = false;
  bool chain1_changed_ = false;
  bool chain2_changed_ = false;
};

#endif  // DAISY_CHAIN_H
