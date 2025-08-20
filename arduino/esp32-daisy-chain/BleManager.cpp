#include "BleManager.h"

#define DEBUG_ENABLE_BLEMANAGER 1
#if ((DEBUG_ENABLE_BLEMANAGER == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) Serial.printf("[INF][BleMgr]: " f "\n", ##__VA_ARGS__)
#define DEBUG_ERROR(f, ...) Serial.printf("[ERR][BleMgr]: " f "\n", ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#define DEBUG_ERROR(...)
#endif

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
    BleManager::getInstance().onClientConnect();
  }
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
    BleManager::getInstance().onClientDisconnect();
  }
  void onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo) override {
    DEBUG_INFO("MTU updated: %u for connection ID: %u\n", MTU, connInfo.getConnHandle());
  }
};

class RxCallbacks : public NimBLECharacteristicCallbacks {
  // Client writes data to RxCharacteristic [App -> ESP32]
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    auto value = pCharacteristic->getValue();
    const uint8_t* data = reinterpret_cast<const uint8_t*>(value.c_str());
    BleManager::getInstance().onDataReceived(data, value.length());
  }
};

class TxCallbacks : public NimBLECharacteristicCallbacks {
  // Client reads data from TxCharacteristic after it has been notified [ESP32 -> App]
  void onRead(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override {
    BleManager::getInstance().writeDataComplete();
  }
};

void BleManager::initialize() {
  DEBUG_INFO("Initialize BLE Manager [...]");

  NimBLEDevice::init(DEVICE_NAME);  // Device name
  server_ = NimBLEDevice::createServer();
  server_->setCallbacks(new ServerCallbacks());

  service_ = server_->createService(SERVICE_UUID);

  // TX characteristic (ESP32 -> Client)
  txCharacteristic_ = service_->createCharacteristic(CHARACTERISTIC_UUID_TX, NIMBLE_PROPERTY::NOTIFY);
  txCharacteristic_->setCallbacks(new TxCallbacks());

  // RX characteristic (Client -> ESP32)
  rxCharacteristic_ = service_->createCharacteristic(CHARACTERISTIC_UUID_RX, NIMBLE_PROPERTY::WRITE);
  rxCharacteristic_->setCallbacks(new RxCallbacks());

  service_->start();
  advertising_ = NimBLEDevice::getAdvertising();
  advertising_->setName(DEVICE_NAME);
  advertising_->addServiceUUID(service_->getUUID());
  advertising_->enableScanResponse(true);
  startAdvertising();

  DEBUG_INFO("Initialize BLE Manager [OK]");
}

void BleManager::startAdvertising() {
  DEBUG_INFO("Start advertising BLE service.");
  advertising_->start();
}

void BleManager::stopAdvertising() {
  DEBUG_INFO("Stop advertising BLE service.");
  advertising_->stop();
}

void BleManager::onClientConnect() {
  DEBUG_INFO("Client connected!");
  connected_ = true;
}

void BleManager::onClientDisconnect() {
  DEBUG_INFO("Client disconnected!");
  connected_ = false;
  startAdvertising();
}

void BleManager::onDataReceived(const uint8_t data[], size_t length) {
  DEBUG_INFO("Data received (%zu): %.*s", length, static_cast<int>(length), data);
  // Process the received data as needed
  // For example, you can parse commands or update the state of the application
}

bool BleManager::writeData(const uint8_t data[], size_t length) {
  if (txCharacteristic_ == nullptr || connected_ == false ||  //
      tx_ongoing_ == true || data == nullptr || length == 0) {
    return false;
  }

  tx_data_ = const_cast<uint8_t*>(data);  // Store the data to be sent
  tx_length_ = length;                    // Store the length of the data
  tx_index_ = 0;                          // Reset the index for TX data

  // Start write loop
  tx_ongoing_ = true;
  return writeDataComplete();
}

bool BleManager::writeDataComplete() {
  if (txCharacteristic_ == nullptr || connected_ == false ||  //
      tx_ongoing_ == false || tx_data_ == nullptr || tx_length_ == 0) {
    return false;
  }
  tx_ongoing_ = false;  // Reset flag after write operation

  if (tx_index_ < tx_length_) {
    // More data to send, continue with next write cycle
    size_t length = min(MaxTxDataLength, tx_length_ - tx_index_);
    txCharacteristic_->setValue(tx_data_ + tx_index_, length);
    txCharacteristic_->notify();
    tx_index_ += length;  // Update index for next TX operation
    tx_ongoing_ = true;
    return true;
  }

  tx_data_ = nullptr;
  tx_length_ = 0;
  tx_index_ = 0;
  return true;  // All data sent successfully
}
