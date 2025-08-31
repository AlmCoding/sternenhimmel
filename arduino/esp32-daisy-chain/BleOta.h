#ifndef BLE_OTA_H
#define BLE_OTA_H

#include <NimBLEDevice.h>
#include "common.h"

class BleOta {
  constexpr static uint32_t RestartDelay = 2000;  // Delay before restart in milliseconds

 public:
  BleOta(const BleOta&) = delete;
  BleOta& operator=(const BleOta&) = delete;

  static BleOta& getInstance() {
    static BleOta instance;
    return instance;
  }

  void initialize(NimBLEServer* server);
  void complete();
  void run();

 private:
  BleOta() = default;

  bool complete_ = false;
  uint32_t complete_time_ = 0;
};

#endif  // BLE_OTA_H
