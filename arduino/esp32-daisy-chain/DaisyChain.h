#ifndef DAISY_CHAIN_H
#define DAISY_CHAIN_H

#include <SPI.h>
#include "TurboTLC59711.h"
#include "common.h"
#include "driver/spi_master.h"

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
  void get_idle_leds(LedObj leds[], size_t size) const;
  void load_default_values();
  void apply_idle_values();
  void flush_all();
  void flush_chain(ChainIdx idx, bool force = false);
  bool save_calibrated_values(const char calibration_name[]);
  bool delete_calibration_data();

 private:
  constexpr static uint8_t CalibrationFormatVersion = 0;
  constexpr static size_t CalibrationNameMaxLength = 32;
  // HSPI pins
  constexpr static int SpiClockFreq = 8000000;  // 8 MHz
  constexpr static int SpiClockPin = 12;
  constexpr static int SpiDataPin = 11;
  constexpr static int Chain0SelectPin = 48;
  constexpr static int Chain1SelectPin = 47;
  constexpr static int Chain2SelectPin = 21;
  constexpr static int Chain3SelectPin = 10;
  constexpr static int Chain4SelectPin = 9;
  constexpr static int Chain5SelectPin = 3;

  DaisyChain();
  void select_chain(ChainIdx idx);
  void write_data();
  bool load_calibrated_values();
  bool migrate_calibration_data(uint8_t from_version);
  uint16_t linearize_brightness(BrgNumber brightness);

  BrgNumber active_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness1_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness2_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness3_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness4_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness5_[CHAIN_SIZE][LED_COUNT];

  char calibration_name_[CalibrationNameMaxLength + 1] = "NULL";

  BrgNumber idle_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber idle_brightness1_[CHAIN_SIZE][LED_COUNT];
  BrgNumber idle_brightness2_[CHAIN_SIZE][LED_COUNT];
  BrgNumber idle_brightness3_[CHAIN_SIZE][LED_COUNT];
  BrgNumber idle_brightness4_[CHAIN_SIZE][LED_COUNT];
  BrgNumber idle_brightness5_[CHAIN_SIZE][LED_COUNT];

  spi_device_handle_t spi_ = nullptr;
  TurboTLC59711<CHAIN_SIZE> chain_ = {};
  bool chain0_changed_ = false;
  bool chain1_changed_ = false;
  bool chain2_changed_ = false;
  bool chain3_changed_ = false;
  bool chain4_changed_ = false;
  bool chain5_changed_ = false;
};

#endif  // DAISY_CHAIN_H
