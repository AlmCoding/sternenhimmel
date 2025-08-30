#ifndef BLE_OTA_H
#define BLE_OTA_H

#include <NimBLEDevice.h>
#include "common.h"

class BleOta {
 public:
  BleOta(const BleOta&) = delete;
  BleOta& operator=(const BleOta&) = delete;

  static BleOta& getInstance() {
    static BleOta instance;
    return instance;
  }

  void initialize(NimBLEServer* server);

 private:
  BleOta() = default;
};

#endif  // BLE_OTA_H
