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

  // void runTestShow();  // TODO: Remove this function

 private:
  constexpr static uint8_t CalibrationFormatVersion = 0;
  constexpr static size_t CalibrationNameMaxLength = 32;
  // HSPI pins (chain 0)
  constexpr static int Clock0Gpio = 14;
  constexpr static int Data0Gpio = 13;
  // VSPI pins (chain 1)
  constexpr static int Clock1Gpio = 18;
  constexpr static int Data1Gpio = 23;

  DaisyChain()
      : chain0_(CHAIN_SIZE, Clock0Gpio, Data0Gpio),  //
        chain1_(CHAIN_SIZE, Clock1Gpio, Data1Gpio) {}

  bool load_calibrated_values();
  void save_calibrated_values();
  bool migrate_calibration_data(uint8_t from_version);
  void load_default_values();
  uint16_t linearize_brightness(BrgNumber brightness);

  BrgNumber active_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness1_[CHAIN_SIZE][LED_COUNT];

  char calibration_name_[CalibrationNameMaxLength] = "default";
  BrgNumber calibrated_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness1_[CHAIN_SIZE][LED_COUNT];

  Adafruit_TLC59711 chain0_;
  Adafruit_TLC59711 chain1_;
  bool chain0_changed_ = false;
  bool chain1_changed_ = false;
};

#endif  // DAISY_CHAIN_H
