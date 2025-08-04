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
  void get_calibrated_leds(LedObj leds[], size_t size) const;
  void apply_calibrated_values();
  void flush_chain(ChainIdx idx, bool force = false);

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

  DaisyChain() {
    spi_bus_config_t buscfg = { .mosi_io_num = SpiDataPin,   //
                                .miso_io_num = -1,           //
                                .sclk_io_num = SpiClockPin,  //
                                .quadwp_io_num = -1,         //
                                .quadhd_io_num = -1 };
    spi_device_interface_config_t devcfg = { .mode = SPI_MODE0,               // SPI mode 0
                                             .clock_speed_hz = SpiClockFreq,  // Set clock speed
                                             .spics_io_num = -1,              // Slave Select pin
                                             .queue_size = 4,                 // SPI transaction queue size
                                             .pre_cb = NULL,
                                             .post_cb = NULL };

    // Initialize SPI bus
    ESP_ERROR_CHECK(spi_bus_initialize(SPI3_HOST, &buscfg, SPI_DMA_CH_AUTO));

    // Add SPI device to bus
    ESP_ERROR_CHECK(spi_bus_add_device(SPI3_HOST, &devcfg, &spi_));
  }

  void select_chain(ChainIdx idx);
  void write_data();
  bool load_calibrated_values();
  void save_calibrated_values();
  bool migrate_calibration_data(uint8_t from_version);
  void load_default_values();
  uint16_t linearize_brightness(BrgNumber brightness);

  BrgNumber active_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness1_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness2_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness3_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness4_[CHAIN_SIZE][LED_COUNT];
  BrgNumber active_brightness5_[CHAIN_SIZE][LED_COUNT];

  char calibration_name_[CalibrationNameMaxLength] = "default";
  BrgNumber calibrated_brightness0_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness1_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness2_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness3_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness4_[CHAIN_SIZE][LED_COUNT];
  BrgNumber calibrated_brightness5_[CHAIN_SIZE][LED_COUNT];

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
