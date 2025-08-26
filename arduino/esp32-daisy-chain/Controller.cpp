#include "Controller.h"
#include "BleManager.h"
#include "DaisyChain.h"
#include "Player.h"

#define DEBUG_ENABLE_CONTROLLER 1
#if ((DEBUG_ENABLE_CONTROLLER == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) Serial.printf("[INF][Ctrl]: " f "\n", ##__VA_ARGS__)
#define DEBUG_ERROR(f, ...) Serial.printf("[ERR][Ctrl]: " f "\n", ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#define DEBUG_ERROR(...)
#endif

void Controller::initialize() {
  DEBUG_INFO("Initialize Controller [...]");

  // Initialize RX and TX buffers
  memset(rx_buffer_, 0, RxBufferSize);
  memset(tx_buffer_, 0, TxBufferSize);

  rx_index_ = 0;        // Reset RX index
  rx_start_time_ = 0;   // Reset RX start time
  rx_ongoing_ = false;  // Reset RX ongoing flag

  DEBUG_INFO("Controller initialized [OK]");
}

void Controller::dataReceivedCallback(const uint8_t data[], size_t length) {
  if (data == nullptr || length == 0) {
    DEBUG_ERROR("Received data is NULL or empty!");
    return;
  }

  if (rx_ongoing_ == false) {
    // Start new RX operation
    rx_ongoing_ = true;
    rx_index_ = 0;
  }

  if (rx_index_ + length > RxBufferSize) {
    DEBUG_ERROR("RX buffer overflow!");
    return;
  }

  // Reset timeout timer for every received data chunk
  rx_start_time_ = millis();

  memcpy(rx_buffer_ + rx_index_, data, length);
  rx_index_ += length;

  // Check if string terminator is present
  if (rx_index_ > 0 && rx_buffer_[rx_index_ - 1] == '\0') {
    DEBUG_INFO("Full doc received: %.*s", static_cast<int>(rx_index_), rx_buffer_);
    process_rx_data_ = true;
  }
}

void Controller::run() {
  if (rx_ongoing_ == true) {
    // Check for timeout expiration
    if (millis() - rx_start_time_ >= RxTimeout) {
      DEBUG_ERROR("RX timeout expired, aborting RX operation!");
      rx_index_ = 0;
      rx_ongoing_ = false;
      process_rx_data_ = false;
    }

    // Check if data can be processed
    if (process_rx_data_ == true) {
      processReceivedData();
      rx_index_ = 0;
      rx_ongoing_ = false;
      process_rx_data_ = false;
    }
  }
}

void Controller::processReceivedData() {
  if (rx_index_ == 0) {
    DEBUG_ERROR("No data to process!");
    return;
  }
  current_rid_ = -1;

  // Parse JSON from the RX buffer
  DeserializationError error = deserializeJson(rx_json_doc_, rx_buffer_, rx_index_);
  if (error != DeserializationError::Ok) {
    sendStatusResponse(-1, KEY_MSG, "Deserialize JSON string failed (%s)", error.c_str());
    return;
  }

  // Parse request id
  if (rx_json_doc_.containsKey(KEY_RID) == false) {
    sendStatusResponse(-1, KEY_MSG, STATUS_MSG_MISSING_KEY, KEY_RID);
    return;
  }
  current_rid_ = rx_json_doc_[KEY_RID];

  // Parse the command from the JSON document
  if (rx_json_doc_.containsKey(KEY_CMD) == false) {
    sendStatusResponse(-1, KEY_MSG, STATUS_MSG_MISSING_KEY, KEY_CMD);
    return;
  }
  const char* cmd = rx_json_doc_[KEY_CMD];

  if (strcmp(cmd, CMD_GET_VERSION) == 0) {
    handleGetVersion();
  } else if (strcmp(cmd, CMD_GET_CALIBRATION_NAME) == 0) {
    handleGetCalibrationName();
  } else if (strcmp(cmd, CMD_DELETE_CALIBRATION) == 0) {
    handleDeleteCalibration();
  } else if (strcmp(cmd, CMD_SAVE_CALIBRATION) == 0) {
    handleSaveCalibration();
  } else if (strcmp(cmd, CMD_SET_BRIGHTNESS) == 0) {
    handleSetBrightness();
  } else if (strcmp(cmd, CMD_GET_BRIGHTNESS) == 0) {
    handleGetBrightness();
  } else if (strcmp(cmd, CMD_PLAY) == 0) {
    handlePlayShow();
  } else if (strcmp(cmd, CMD_STOP) == 0) {
    handleStopShow();
  } else {
    sendStatusResponse(-1, KEY_MSG, "Unknown '%s': '%s'", KEY_CMD, cmd);
  }
}

void Controller::handleGetVersion() {
  DEBUG_INFO("CMD: '%s' [...]", CMD_GET_VERSION);

  sendStatusResponse(0, KEY_VERSION, "v%d.%d.%d", FIRMWARE_VERSION_MAJOR, FIRMWARE_VERSION_MINOR,
                     FIRMWARE_VERSION_PATCH);
  DEBUG_INFO("CMD: '%s' [OK]", CMD_GET_VERSION);
}

void Controller::handleGetCalibrationName() {
  DEBUG_INFO("CMD: '%s' [...]", CMD_GET_VERSION);

  const char* name = DaisyChain::getInstance().get_calibration_name();
  ASSERT(name != nullptr);

  sendStatusResponse(0, KEY_NAME, "%s", name);
  DEBUG_INFO("CMD: '%s' [OK]", CMD_GET_VERSION);
}

void Controller::handleDeleteCalibration() {
  DEBUG_INFO("CMD: '%s' [...]", CMD_DELETE_CALIBRATION);

  DaisyChain::getInstance().delete_calibration_data();
  DaisyChain::getInstance().load_default_values();
  DaisyChain::getInstance().apply_idle_values();

  sendStatusResponse(0, "", "");
  DEBUG_INFO("CMD: '%s' [OK]", CMD_DELETE_CALIBRATION);
}

void Controller::handleSaveCalibration() {
  DEBUG_INFO("CMD: '%s' [...]", CMD_SAVE_CALIBRATION);

  if (rx_json_doc_.containsKey(KEY_NAME) == false) {
    sendStatusResponse(-1, KEY_MSG, STATUS_MSG_MISSING_KEY, KEY_NAME);
    return;
  }

  const char* name = rx_json_doc_[KEY_NAME];
  DaisyChain::getInstance().save_calibrated_values(name);

  sendStatusResponse(0, "", "");
  DEBUG_INFO("CMD: '%s' [OK]", CMD_SAVE_CALIBRATION);
}

void Controller::handleSetBrightness() {
  DEBUG_INFO("CMD: '%s' [...]", CMD_SET_BRIGHTNESS);

  if (rx_json_doc_.containsKey(KEY_LEDS) == false) {
    sendStatusResponse(-1, KEY_MSG, STATUS_MSG_MISSING_KEY, KEY_LEDS);
    return;
  }

  LedObj obj;
  JsonArray led;
  size_t led_count = rx_json_doc_[KEY_LEDS].size();
  for (size_t i = 0; i < led_count; i++) {
    led = rx_json_doc_[KEY_LEDS][i];
    uint8_t pcb_idx = led[0];
    uint8_t led_idx = led[1];
    uint8_t brightness = led[2];
    DEBUG_INFO("  LED(%d, %d): brightness=%d", pcb_idx, led_idx, brightness);

    if (setLedObj(obj, pcb_idx, led_idx, brightness) == false) {
      sendStatusResponse(-1, KEY_MSG, "Invalid LED object: [%d, %d, %d]", pcb_idx, led_idx, brightness);
      return;
    }
    DaisyChain::getInstance().set_idle_leds(&obj, 1);
  }
  DaisyChain::getInstance().apply_idle_values();

  sendStatusResponse(0, "", "");
  DEBUG_INFO("CMD: '%s' [OK]", CMD_SET_BRIGHTNESS);
}

void Controller::handleGetBrightness() {
  DEBUG_INFO("CMD: '%s' [...]", CMD_GET_BRIGHTNESS);

  if (rx_json_doc_.containsKey(KEY_LEDS) == false) {
    sendStatusResponse(-1, KEY_MSG, STATUS_MSG_MISSING_KEY, KEY_LEDS);
    return;
  }

  JsonArray response_leds = tx_json_doc_.createNestedArray(KEY_LEDS);
  LedObj obj;
  JsonArray led;
  size_t led_count = rx_json_doc_[KEY_LEDS].size();
  for (size_t i = 0; i < led_count; i++) {
    led = rx_json_doc_[KEY_LEDS][i];
    uint8_t pcb_idx = led[0];
    uint8_t led_idx = led[1];
    DEBUG_INFO("  LED(%d, %d)", pcb_idx, led_idx);

    if (setLedObj(obj, pcb_idx, led_idx, 0) == false) {
      sendStatusResponse(-1, KEY_MSG, "Invalid LED object: [%d, %d]", pcb_idx, led_idx);
      return;
    }
    DaisyChain::getInstance().get_idle_leds(&obj, 1);
    JsonArray resp_led = response_leds.createNestedArray();
    resp_led.add(pcb_idx);
    resp_led.add(led_idx);
    resp_led.add(obj.brightness);
  }

  tx_json_doc_[KEY_RID] = current_rid_;
  tx_json_doc_[KEY_MSG] = "OK";
  tx_json_doc_[KEY_STATUS] = 0;

  size_t len = serializeJson(tx_json_doc_, tx_buffer_, TxBufferSize);
  if (len == 0 || len >= TxBufferSize) {
    DEBUG_ERROR("Failed to serialize JSON response!");
    return;
  }
  tx_buffer_[len] = '\0';  // Ensure null-terminated string
  len++;                   // Include null terminator in length

  DEBUG_INFO("Sending response (%d): %s", len, tx_buffer_);
  if (BleManager::getInstance().writeData(tx_buffer_, len) == false) {
    DEBUG_ERROR("Failed to send response via BLE!");
  }

  DEBUG_INFO("CMD: '%s' [OK]", CMD_GET_BRIGHTNESS);
}

void Controller::handlePlayShow() {
  DEBUG_INFO("CMD: '%s' [...]", CMD_PLAY);

  sendStatusResponse(0, "", "");
  DEBUG_INFO("CMD: '%s' [OK]", CMD_PLAY);
}

void Controller::handleStopShow() {
  DEBUG_INFO("CMD: '%s' [...]", CMD_STOP);

  Player::getInstance().abort();

  sendStatusResponse(0, "", "");
  DEBUG_INFO("CMD: '%s' [OK]", CMD_STOP);
}

void Controller::sendStatusResponse(int status, const char key[], const char value[], ...) {
  char buffer[128];
  ASSERT(key != nullptr);
  ASSERT(value != nullptr);

  va_list args;
  va_start(args, value);
  vsnprintf(buffer, sizeof(buffer), value, args);
  va_end(args);

  tx_json_doc_.clear();
  tx_json_doc_[KEY_RID] = current_rid_;
  tx_json_doc_[KEY_STATUS] = status;
  if (strlen(key) > 0) {
    tx_json_doc_[key] = buffer;
  }

  size_t len = serializeJson(tx_json_doc_, tx_buffer_, TxBufferSize);
  if (len == 0 || len >= TxBufferSize) {
    DEBUG_ERROR("Failed to serialize JSON response!");
    return;
  }
  tx_buffer_[len] = '\0';  // Ensure null-terminated string
  len++;                   // Include null terminator in length

  DEBUG_INFO("Sending response (%d): %s", len, tx_buffer_);
  if (BleManager::getInstance().writeData(tx_buffer_, len) == false) {
    DEBUG_ERROR("Failed to send response via BLE!");
  }
}

bool Controller::setLedObj(LedObj& obj, uint8_t pcb_idx, uint8_t led_idx, uint8_t brightness) {
  pcb_idx -= 1;  // Convert to zero-based index
  led_idx -= 1;  // Convert to zero-based index

  if (pcb_idx >= (CHAIN_SIZE * CHAIN_COUNT) || led_idx >= LED_COUNT ||  //
      brightness > static_cast<uint8_t>(BrgName::MAX)) {
    return false;
  }

  obj.chain_idx = static_cast<ChainIdx>(pcb_idx / CHAIN_SIZE);
  obj.pcb_idx = pcb_idx % CHAIN_SIZE;
  obj.led_idx = led_idx;
  obj.brightness = static_cast<BrgNumber>(brightness);
  return true;
}