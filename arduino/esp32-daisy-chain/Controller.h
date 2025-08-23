#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ArduinoJson.h>
#include "common.h"

class Controller {
  constexpr static uint32_t RxTimeout = 1000;       // Timeout for RX in milliseconds
  constexpr static size_t RxBufferSize = 1024 * 8;  // Size of the RX buffer
  constexpr static size_t TxBufferSize = 1024 * 2;  // Size of the TX buffer

  constexpr static char KEY_RID[] = "rid";
  constexpr static char KEY_CMD[] = "cmd";
  constexpr static char KEY_TXT[] = "txt";

  // For status response
  constexpr static char KEY_MSG[] = "msg";
  constexpr static char KEY_STS[] = "sts";

  constexpr static char CMD_GET_VERSION[] = "get_version";
  constexpr static char CMD_DELETE_CALIBRATION[] = "delete_calibration";
  constexpr static char CMD_SAVE_CALIBRATION[] = "save_calibration";
  constexpr static char CMD_PLAY[] = "play_show";
  constexpr static char CMD_STOP[] = "stop_show";

  constexpr static char STATUS_MSG_MISSING_KEY[] = "JSON key ('%s') not found!";

 public:
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;

  static Controller& getInstance() {
    static Controller instance;
    return instance;
  }

  void initialize();
  void dataReceivedCallback(const uint8_t data[], size_t length);

  void run();
  void processReceivedData();

  void handleGetVersion();
  void handleDeleteCalibration();
  void handleSaveCalibration();
  void handlePlayShow();
  void handleStopShow();

  void sendStatusResponse(int status, const char info[], ...);

 private:
  Controller() = default;

  uint8_t rx_buffer_[RxBufferSize];
  uint8_t tx_buffer_[TxBufferSize];
  size_t rx_index_ = 0;  // Current index in RX buffer

  uint32_t rx_start_time_ = 0;  // Start time for RX buffer
  bool rx_ongoing_ = false;     // Flag to indicate if RX is ongoing
  bool process_rx_data_ = false;

  StaticJsonDocument<2 * RxBufferSize> rx_json_doc_;
  StaticJsonDocument<2 * TxBufferSize> tx_json_doc_;

  int32_t current_rid_ = -1;
};

#endif  // CONTROLLER_H