#include "DaisyChain.h"
#include <Preferences.h>

#define DEBUG_ENABLE_DAISYCHAIN 1
#if ((DEBUG_ENABLE_DAISYCHAIN == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) debugPrint("[INF][Chain]", f, ##__VA_ARGS__)
#define DEBUG_ERROR(f, ...) debugPrint("[ERR][Chain]", f, ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#define DEBUG_ERROR(...)
#endif

DaisyChain::DaisyChain() {
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

void DaisyChain::initialize() {
  DEBUG_INFO("Initialize DaisyChain [...]");
  pinMode(Chain0SelectPin, OUTPUT);
  pinMode(Chain1SelectPin, OUTPUT);
  pinMode(Chain2SelectPin, OUTPUT);
  pinMode(Chain3SelectPin, OUTPUT);
  pinMode(Chain4SelectPin, OUTPUT);
  pinMode(Chain5SelectPin, OUTPUT);

  digitalWrite(Chain0SelectPin, HIGH);
  digitalWrite(Chain1SelectPin, HIGH);
  digitalWrite(Chain2SelectPin, HIGH);
  digitalWrite(Chain3SelectPin, HIGH);
  digitalWrite(Chain4SelectPin, HIGH);
  digitalWrite(Chain5SelectPin, HIGH);

  chain_.init();
  writeData();

  if (loadCalibratedValues() == false) {
    DEBUG_ERROR("Calibrated values not found!");
    loadDefaultValues();
  }
  applyIdleValues();

  flushAll();
  DEBUG_INFO("Initialize DaisyChain [OK]");
}

bool DaisyChain::loadCalibratedValues() {
  /* NVS storage layout:
   * "calibration" namespace:
   * - "format_version" (uint8_t)
   * - "calib_name" (string)
   * - "calib_chain0" (uint8_t[CHAIN_SIZE][LED_COUNT])
   * - "calib_chain1" (uint8_t[CHAIN_SIZE][LED_COUNT])
   * - "calib_chain2" (uint8_t[CHAIN_SIZE][LED_COUNT])
   * - "calib_chain3" (uint8_t[CHAIN_SIZE][LED_COUNT])
   * - "calib_chain4" (uint8_t[CHAIN_SIZE][LED_COUNT])
   * - "calib_chain5" (uint8_t[CHAIN_SIZE][LED_COUNT])
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
    return migrateCalibrationData(format_version);
  }

  // Check if all keys are present
  if (preferences.isKey("calib_name") == false ||    //
      preferences.isKey("calib_chain0") == false ||  //
      preferences.isKey("calib_chain1") == false ||  //
      preferences.isKey("calib_chain2") == false ||  //
      preferences.isKey("calib_chain3") == false ||  //
      preferences.isKey("calib_chain4") == false ||  //
      preferences.isKey("calib_chain5") == false) {
    preferences.end();
    DEBUG_ERROR("Calibration data incomplete!");
    return false;
  }

  preferences.getString("calib_name", calibration_name_, CalibrationNameMaxLength);

  size_t length0 = preferences.getBytesLength("calib_chain0");
  size_t length1 = preferences.getBytesLength("calib_chain1");
  size_t length2 = preferences.getBytesLength("calib_chain2");
  size_t length3 = preferences.getBytesLength("calib_chain3");
  size_t length4 = preferences.getBytesLength("calib_chain4");
  size_t length5 = preferences.getBytesLength("calib_chain5");

  if (length0 != sizeof(idle_brightness0_) ||  //
      length1 != sizeof(idle_brightness1_) ||  //
      length2 != sizeof(idle_brightness2_) ||  //
      length3 != sizeof(idle_brightness3_) ||  //
      length4 != sizeof(idle_brightness4_) ||  //
      length5 != sizeof(idle_brightness5_)) {
    preferences.end();
    return false;
  }

  preferences.getBytes("calib_chain0", idle_brightness0_, sizeof(idle_brightness0_));
  preferences.getBytes("calib_chain1", idle_brightness1_, sizeof(idle_brightness1_));
  preferences.getBytes("calib_chain2", idle_brightness2_, sizeof(idle_brightness2_));
  preferences.getBytes("calib_chain3", idle_brightness3_, sizeof(idle_brightness3_));
  preferences.getBytes("calib_chain4", idle_brightness4_, sizeof(idle_brightness4_));
  preferences.getBytes("calib_chain5", idle_brightness5_, sizeof(idle_brightness5_));

  preferences.end();
  DEBUG_INFO("Calibration (\"%s\") loaded [OK]", calibration_name_);
  return true;
}

bool DaisyChain::migrateCalibrationData(uint8_t from_version) {
  // No migration needed for now
  return false;
}

bool DaisyChain::saveCalibratedValues(const char calibration_name[]) {
  if (calibration_name == nullptr || strnlen(calibration_name, sizeof(calibration_name_)) > CalibrationNameMaxLength) {
    DEBUG_ERROR("Invalid calibration name!");
    return false;
  }

  strncpy(calibration_name_, calibration_name, CalibrationNameMaxLength);
  calibration_name_[CalibrationNameMaxLength - 1] = '\0';  // Ensure

  Preferences preferences;
  preferences.begin("calibration", false);  // open (create if needed) the namespace in RW mode

  preferences.putUChar("format_version", CalibrationFormatVersion);
  preferences.putString("calib_name", calibration_name_);
  preferences.putBytes("calib_chain0", idle_brightness0_, sizeof(idle_brightness0_));
  preferences.putBytes("calib_chain1", idle_brightness1_, sizeof(idle_brightness1_));
  preferences.putBytes("calib_chain2", idle_brightness2_, sizeof(idle_brightness2_));
  preferences.putBytes("calib_chain3", idle_brightness3_, sizeof(idle_brightness3_));
  preferences.putBytes("calib_chain4", idle_brightness4_, sizeof(idle_brightness4_));
  preferences.putBytes("calib_chain5", idle_brightness5_, sizeof(idle_brightness5_));

  preferences.end();
  DEBUG_INFO("Calibrated values saved [OK]");
  return true;
}

bool DaisyChain::deleteCalibrationData() {
  Preferences preferences;
  preferences.begin("calibration", false);  // open (create if needed) the namespace in RW mode
  preferences.clear();                      // clears all keys in "calibration"
  preferences.end();
  return true;
}

const char* DaisyChain::getCalibrationName() const {
  return calibration_name_;
}

void DaisyChain::loadDefaultValues() {
  memset(idle_brightness0_, 0, sizeof(idle_brightness0_));
  memset(idle_brightness1_, 0, sizeof(idle_brightness1_));
  memset(idle_brightness2_, 0, sizeof(idle_brightness2_));
  memset(idle_brightness3_, 0, sizeof(idle_brightness3_));
  memset(idle_brightness4_, 0, sizeof(idle_brightness4_));
  memset(idle_brightness5_, 0, sizeof(idle_brightness5_));
}

void DaisyChain::applyIdleValues() {
  memcpy(active_brightness0_, idle_brightness0_, sizeof(active_brightness0_));
  memcpy(active_brightness1_, idle_brightness1_, sizeof(active_brightness1_));
  memcpy(active_brightness2_, idle_brightness2_, sizeof(active_brightness2_));
  memcpy(active_brightness3_, idle_brightness3_, sizeof(active_brightness3_));
  memcpy(active_brightness4_, idle_brightness4_, sizeof(active_brightness4_));
  memcpy(active_brightness5_, idle_brightness5_, sizeof(active_brightness5_));
  chain0_changed_ = true;
  chain1_changed_ = true;
  chain2_changed_ = true;
  chain3_changed_ = true;
  chain4_changed_ = true;
  chain5_changed_ = true;
}

void DaisyChain::flushAll() {
  flushChain(ChainIdx::CHAIN_0);
  flushChain(ChainIdx::CHAIN_1);
  flushChain(ChainIdx::CHAIN_2);
  flushChain(ChainIdx::CHAIN_3);
  flushChain(ChainIdx::CHAIN_4);
  flushChain(ChainIdx::CHAIN_5);
}

void DaisyChain::flushChain(ChainIdx idx, bool force) {
  BrgNumber(*current_brightness)[CHAIN_SIZE][LED_COUNT] = nullptr;

  switch (idx) {
    case ChainIdx::CHAIN_0:
      if (chain0_changed_ == false && force == false) {
        return;
      }
      chain0_changed_ = false;
      current_brightness = &active_brightness0_;
      break;
    case ChainIdx::CHAIN_1:
      if (chain1_changed_ == false && force == false) {
        return;
      }
      chain1_changed_ = false;
      current_brightness = &active_brightness1_;
      break;
    case ChainIdx::CHAIN_2:
      if (chain2_changed_ == false && force == false) {
        return;
      }
      chain2_changed_ = false;
      current_brightness = &active_brightness2_;
      break;
    case ChainIdx::CHAIN_3:
      if (chain3_changed_ == false && force == false) {
        return;
      }
      chain3_changed_ = false;
      current_brightness = &active_brightness3_;
      break;
    case ChainIdx::CHAIN_4:
      if (chain4_changed_ == false && force == false) {
        return;
      }
      chain4_changed_ = false;
      current_brightness = &active_brightness4_;
      break;
    case ChainIdx::CHAIN_5:
      if (chain5_changed_ == false && force == false) {
        return;
      }
      chain5_changed_ = false;
      current_brightness = &active_brightness5_;
      break;
    default:
      DEBUG_INFO("Invalid chain index!");
      return;
  }

  for (uint8_t tlc_idx = 0; tlc_idx < CHAIN_SIZE; tlc_idx++) {
    // Invert tlc_idx and led_idx to match physical wiring/naming
    uint16_t tlc_idx_inv = CHAIN_SIZE - tlc_idx - 1;

    for (uint8_t led_idx = 0; led_idx < (LED_COUNT / 3); led_idx++) {  // LED index is [0, 1, 2, 3]
      uint16_t ch_r = linearizeBrightness((*current_brightness)[tlc_idx][led_idx * 3]);
      uint16_t ch_g = linearizeBrightness((*current_brightness)[tlc_idx][led_idx * 3 + 1]);
      uint16_t ch_b = linearizeBrightness((*current_brightness)[tlc_idx][led_idx * 3 + 2]);

      uint8_t led_idx_inv = (LED_COUNT / 3 - 1) - led_idx;        // Invert led index to match physical wiring/naming
      chain_.setLed(tlc_idx_inv, led_idx_inv, ch_b, ch_g, ch_r);  // Note the order: B, G, R
    }
  }

  selectChain(idx);
  writeData();
}

void DaisyChain::selectChain(ChainIdx idx) {
  digitalWrite(Chain0SelectPin, LOW);
  digitalWrite(Chain1SelectPin, LOW);
  digitalWrite(Chain2SelectPin, LOW);
  digitalWrite(Chain3SelectPin, LOW);
  digitalWrite(Chain4SelectPin, LOW);
  digitalWrite(Chain5SelectPin, LOW);

  switch (idx) {
    case ChainIdx::CHAIN_0:
      digitalWrite(Chain0SelectPin, HIGH);
      break;
    case ChainIdx::CHAIN_1:
      digitalWrite(Chain1SelectPin, HIGH);
      break;
    case ChainIdx::CHAIN_2:
      digitalWrite(Chain2SelectPin, HIGH);
      break;
    case ChainIdx::CHAIN_3:
      digitalWrite(Chain3SelectPin, HIGH);
      break;
    case ChainIdx::CHAIN_4:
      digitalWrite(Chain4SelectPin, HIGH);
      break;
    case ChainIdx::CHAIN_5:
      digitalWrite(Chain5SelectPin, HIGH);
      break;
    default:
      break;
  }
}

void DaisyChain::writeData() {
  // Send data using DMA
  spi_transaction_t trans = {};
  trans.length = chain_.getChainBufferSize() * 8;
  trans.tx_buffer = chain_.getChainBuffer();
  trans.rx_buffer = nullptr;

  ESP_ERROR_CHECK(spi_device_transmit(spi_, &trans));
}

uint16_t DaisyChain::linearizeBrightness(BrgNumber brightness) {
  return BRIGHTNESS_LINEARIZATION_TABLE[static_cast<int>(brightness)];
}

void DaisyChain::getActiveLeds(LedObj leds[], size_t size) const {
  for (size_t i = 0; i < size; i++) {
    ChainIdx chain_idx = leds[i].chain_idx;
    uint8_t pcb_idx = leds[i].pcb_idx;
    uint8_t led_idx = leds[i].led_idx;

    switch (chain_idx) {
      case ChainIdx::CHAIN_0:
        leds[i].brightness = active_brightness0_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_1:
        leds[i].brightness = active_brightness1_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_2:
        leds[i].brightness = active_brightness2_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_3:
        leds[i].brightness = active_brightness3_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_4:
        leds[i].brightness = active_brightness4_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_5:
        leds[i].brightness = active_brightness5_[pcb_idx][led_idx];
        break;
      default:
        break;
    }
  }
}

void DaisyChain::setActiveLeds(LedObj leds[], size_t size) {
  for (size_t i = 0; i < size; i++) {
    ChainIdx chain_idx = leds[i].chain_idx;
    uint8_t pcb_idx = leds[i].pcb_idx;
    uint8_t led_idx = leds[i].led_idx;
    BrgNumber brightness = leds[i].brightness;

    switch (chain_idx) {
      case ChainIdx::CHAIN_0:
        active_brightness0_[pcb_idx][led_idx] = brightness;
        chain0_changed_ = true;
        break;
      case ChainIdx::CHAIN_1:
        active_brightness1_[pcb_idx][led_idx] = brightness;
        chain1_changed_ = true;
        break;
      case ChainIdx::CHAIN_2:
        active_brightness2_[pcb_idx][led_idx] = brightness;
        chain2_changed_ = true;
        break;
      case ChainIdx::CHAIN_3:
        active_brightness3_[pcb_idx][led_idx] = brightness;
        chain3_changed_ = true;
        break;
      case ChainIdx::CHAIN_4:
        active_brightness4_[pcb_idx][led_idx] = brightness;
        chain4_changed_ = true;
        break;
      case ChainIdx::CHAIN_5:
        active_brightness5_[pcb_idx][led_idx] = brightness;
        chain5_changed_ = true;
        break;
      default:
        break;
    }
  }
}

void DaisyChain::getIdleLeds(LedObj leds[], size_t size) const {
  for (size_t i = 0; i < size; i++) {
    ChainIdx chain_idx = leds[i].chain_idx;
    uint8_t pcb_idx = leds[i].pcb_idx;
    uint8_t led_idx = leds[i].led_idx;

    switch (chain_idx) {
      case ChainIdx::CHAIN_0:
        leds[i].brightness = idle_brightness0_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_1:
        leds[i].brightness = idle_brightness1_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_2:
        leds[i].brightness = idle_brightness2_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_3:
        leds[i].brightness = idle_brightness3_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_4:
        leds[i].brightness = idle_brightness4_[pcb_idx][led_idx];
        break;
      case ChainIdx::CHAIN_5:
        leds[i].brightness = idle_brightness5_[pcb_idx][led_idx];
        break;
      default:
        break;
    }
  }
}

void DaisyChain::setIdleLeds(LedObj leds[], size_t size) {
  for (size_t i = 0; i < size; i++) {
    ChainIdx chain_idx = leds[i].chain_idx;
    uint8_t pcb_idx = leds[i].pcb_idx;
    uint8_t led_idx = leds[i].led_idx;
    BrgNumber brightness = leds[i].brightness;

    switch (chain_idx) {
      case ChainIdx::CHAIN_0:
        idle_brightness0_[pcb_idx][led_idx] = brightness;
        break;
      case ChainIdx::CHAIN_1:
        idle_brightness1_[pcb_idx][led_idx] = brightness;
        break;
      case ChainIdx::CHAIN_2:
        idle_brightness2_[pcb_idx][led_idx] = brightness;
        break;
      case ChainIdx::CHAIN_3:
        idle_brightness3_[pcb_idx][led_idx] = brightness;
        break;
      case ChainIdx::CHAIN_4:
        idle_brightness4_[pcb_idx][led_idx] = brightness;
        break;
      case ChainIdx::CHAIN_5:
        idle_brightness5_[pcb_idx][led_idx] = brightness;
        break;
      default:
        break;
    }
  }
}
