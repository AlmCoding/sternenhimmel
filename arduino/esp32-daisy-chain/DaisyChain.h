#ifndef DAISY_CHAIN_H
#define DAISY_CHAIN_H

#include <SPI.h>
#include "Adafruit_TLC59711.h"
#include "common.h"

// HSPI pins (chain 0)
#define CLOCK_0_GPIO 14
#define DATA_0_GPIO 13
// VSPI pins (chain 1)
#define CLOCK_1_GPIO 18
#define DATA_1_GPIO 23

class DaisyChain {
 public:
  DaisyChain(const DaisyChain&) = delete;
  DaisyChain& operator=(const DaisyChain&) = delete;

  static DaisyChain& getInstance() {
    static DaisyChain instance;
    return instance;
  }

  void initialize();
  void runTestShow();

 private:
  constexpr static uint8_t CalibrationFormatVersion = 0;
  constexpr static size_t CalibrationNameMaxLength = 32;

  DaisyChain()
      : chain0_(CHAIN_SIZE, CLOCK_0_GPIO, DATA_0_GPIO),  //
        chain1_(CHAIN_SIZE, CLOCK_1_GPIO, DATA_1_GPIO) {}
  bool load_calibrated_values();
  void save_calibrated_values();
  bool migrate_calibration_data(uint8_t from_version);
  void load_default_values();
  void flush_chain(ChainIdx idx);
  uint16_t linearize_brightness(BrgNumber brightness);

  BrgNumber current_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber current_brightness1_[CHAIN_SIZE][LED_COUNT];

  char calibration_name_[CalibrationNameMaxLength] = "default";
  BrgNumber calibrated_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness1_[CHAIN_SIZE][LED_COUNT];

  Adafruit_TLC59711 chain0_;
  Adafruit_TLC59711 chain1_;
};

#endif  // DAISY_CHAIN_H
