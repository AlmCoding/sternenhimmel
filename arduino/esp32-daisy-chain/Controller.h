#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <ArduinoJson.h>
#include "common.h"

class Controller {
  constexpr static uint32_t RxTimeout = 1000;        // Timeout for RX in milliseconds
  constexpr static size_t RxBufferSize = 1024 * 10;  // Size of the RX buffer
  constexpr static size_t TxBufferSize = 1024 * 10;  // Size of the TX buffer

  constexpr static size_t MaxLedObjects = 256;    // Maximum number of LED objects in a single command
  constexpr static size_t MaxLedGroups = 16;      // Maximum number of LED groups in a single command
  constexpr static size_t MaxSequenceSteps = 16;  // Maximum number of sequence steps in a single command

  constexpr static char KEY_RID[] = "rid";
  constexpr static char KEY_CMD[] = "cmd";
  constexpr static char KEY_NAME[] = "name";
  constexpr static char KEY_LEDS[] = "leds";
  constexpr static char KEY_FORCE[] = "force";
  constexpr static char KEY_GROUPS[] = "groups";
  constexpr static char KEY_SEQUENCE[] = "sequence";
  constexpr static char KEY_MSG[] = "msg";
  constexpr static char KEY_STATUS[] = "status";
  constexpr static char KEY_VERSION[] = "version";

  constexpr static char CMD_GET_VERSION[] = "get_version";
  constexpr static char CMD_GET_CALIBRATION_NAME[] = "get_calibration_name";
  constexpr static char CMD_DELETE_CALIBRATION[] = "delete_calibration";
  constexpr static char CMD_SAVE_CALIBRATION[] = "save_calibration";
  constexpr static char CMD_SET_BRIGHTNESS[] = "set_brightness";
  constexpr static char CMD_GET_BRIGHTNESS[] = "get_brightness";
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

 private:
  struct GroupInfo {
    LedObj* leds;
    size_t size;
  };
  Controller() = default;

  void processReceivedData();

  void handleGetVersion();
  void handleGetCalibrationName();
  void handleDeleteCalibration();
  void handleSaveCalibration();
  void handleSetBrightness();
  void handleGetBrightness();

  void handlePlayShow();
  void handleStopShow();

  void sendStatusResponse(int status, const char key[], const char value[], ...);
  bool setLedObj(LedObj& obj, uint8_t pcb_idx, uint8_t led_idx, uint8_t brightness);
  bool extractGroups();
  bool extractSequence();

  uint8_t rx_buffer_[RxBufferSize];
  uint8_t tx_buffer_[TxBufferSize];
  size_t rx_index_ = 0;  // Current index in RX buffer

  uint32_t rx_start_time_ = 0;  // Start time for RX buffer
  bool rx_ongoing_ = false;     // Flag to indicate if RX is ongoing
  bool process_rx_data_ = false;

  StaticJsonDocument<2 * RxBufferSize> rx_json_doc_;
  StaticJsonDocument<2 * TxBufferSize> tx_json_doc_;

  LedObj leds_[MaxLedObjects];  // All groups are in this array (the start of each group is pointed to by groups_[])
  GroupInfo groups_[MaxLedGroups];
  size_t group_count_ = 0;

  SequenceStep sequence_[MaxSequenceSteps];
  size_t sequence_length_ = 0;

  int32_t current_rid_ = -1;
};

#endif  // CONTROLLER_H