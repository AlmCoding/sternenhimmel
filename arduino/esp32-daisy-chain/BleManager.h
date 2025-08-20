#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <NimBLEDevice.h>
#include "common.h"

class BleManager {
  constexpr static char DEVICE_NAME[] = "Sternenhimmel";  // Device name

  constexpr static char SERVICE_UUID[] = "6E400001-B5A3-F393-E0A9-E50E24DCCA9E";            // UART service UUID
  constexpr static char CHARACTERISTIC_UUID_RX[] = "6E400002-B5A3-F393-E0A9-E50E24DCCA9E";  // RX characteristic
  constexpr static char CHARACTERISTIC_UUID_TX[] = "6E400003-B5A3-F393-E0A9-E50E24DCCA9E";  // TX characteristic

  constexpr static size_t MaxTxDataLength = 20;  // Maximum length of TX data

 public:
  BleManager(const BleManager&) = delete;
  BleManager& operator=(const BleManager&) = delete;

  static BleManager& getInstance() {
    static BleManager instance;
    return instance;
  }

  void initialize();
  void startAdvertising();
  void stopAdvertising();
  void onClientConnect();
  void onClientDisconnect();
  void onDataReceived(const uint8_t data[], size_t length);
  bool writeData(const uint8_t data[], size_t length);
  bool writeDataComplete();

 private:
  BleManager() = default;

  NimBLEServer* server_ = nullptr;
  NimBLEService* service_ = nullptr;
  NimBLECharacteristic* txCharacteristic_ = nullptr;
  NimBLECharacteristic* rxCharacteristic_ = nullptr;
  NimBLEAdvertising* advertising_ = nullptr;
  bool connected_ = false;

  bool tx_ongoing_ = false;     // Flag to indicate if TX is ongoing
  uint8_t* tx_data_ = nullptr;  // Pointer to hold TX data
  size_t tx_length_ = 0;        // Length of TX data
  size_t tx_index_ = 0;         // Current index in TX data
};

#endif  // BLE_MANAGER_H
