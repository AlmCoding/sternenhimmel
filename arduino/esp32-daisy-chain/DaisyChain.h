#ifndef DAISY_CHAIN_H
#define DAISY_CHAIN_H

#include <SPI.h>
#include "TurboTLC59711.h"
#include "common.h"
#include "driver/spi_master.h"

class DaisyChain {
  constexpr static uint8_t CalibrationFormatVersion = 0;
  constexpr static size_t CalibrationNameMaxLength = 64;
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

  // Gamma brightness lookup table <https://victornpb.github.io/gamma-table-generator>
  // gamma = 2.0 steps = 101 range = 0-65535
  constexpr static uint16_t BRIGHTNESS_LINEARIZATION_TABLE[101] = {
    0,     7,     26,    59,    105,   164,   236,   321,   419,   531,   655,   793,   944,   1108,  1284,
    1475,  1678,  1894,  2123,  2366,  2621,  2890,  3172,  3467,  3775,  4096,  4430,  4778,  5138,  5511,
    5898,  6298,  6711,  7137,  7576,  8028,  8493,  8972,  9463,  9968,  10486, 11016, 11560, 12117, 12688,
    13271, 13867, 14477, 15099, 15735, 16384, 17046, 17721, 18409, 19110, 19824, 20552, 21292, 22046, 22813,
    23593, 24386, 25192, 26011, 26843, 27689, 28547, 29419, 30303, 31201, 32112, 33036, 33973, 34924, 35887,
    36863, 37853, 38856, 39871, 40900, 41942, 42998, 44066, 45147, 46241, 47349, 48470, 49603, 50750, 51910,
    53083, 54270, 55469, 56681, 57907, 59145, 60397, 61662, 62940, 64231, 65535,
  };

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
  void set_idle_leds(LedObj leds[], size_t size);
  void load_default_values();
  void apply_idle_values();
  void flush_all();
  void flush_chain(ChainIdx idx, bool force = false);
  bool save_calibrated_values(const char calibration_name[]);
  bool delete_calibration_data();
  const char* get_calibration_name() const;

 private:
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
