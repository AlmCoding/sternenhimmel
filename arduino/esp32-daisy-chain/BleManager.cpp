#include "BleManager.h"
#include "BleOta.h"
#include "Controller.h"

#define DEBUG_ENABLE_BLEMANAGER 1
#if ((DEBUG_ENABLE_BLEMANAGER == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) debugPrint("[INF][BleMgr]", f, ##__VA_ARGS__)
#define DEBUG_ERROR(f, ...) debugPrint("[ERR][BleMgr]", f, ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#define DEBUG_ERROR(...)
#endif

class ServerCallbacks : public NimBLEServerCallbacks {
  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
    BleManager::getInstance().onClientConnect(true);
  }
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
    BleManager::getInstance().onClientConnect(false);
  }
  void onMTUChange(uint16_t MTU, NimBLEConnInfo& connInfo) override {
    BleManager::getInstance().onMtuChange(MTU);
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
  void onSubscribe(NimBLECharacteristic* c, NimBLEConnInfo& connInfo, uint16_t subValue) override {
    if (subValue == 1 || subValue == 3) {
      DEBUG_ERROR("Client subscription (value: %u) is invalid!", subValue);
    }
    BleManager::getInstance().onSubscribe(subValue == 2);
  }
  void onStatus(NimBLECharacteristic* pCharacteristic, int code) override {
    if (code != BLE_HS_EDONE) {
      DEBUG_ERROR("TX indication (code: %d) failed!", code);
    }
    BleManager::getInstance().onWriteConfirm(code == BLE_HS_EDONE);
  }
};

void BleManager::initialize() {
  DEBUG_INFO("Initialize BLE Manager [...]");

  NimBLEDevice::init(DEVICE_NAME);  // Device name
  server_ = NimBLEDevice::createServer();
  server_->setCallbacks(new ServerCallbacks());

  service_ = server_->createService(SERVICE_UUID);
  // RX characteristic (Client -> ESP32)
  rxCharacteristic_ = service_->createCharacteristic(CHARACTERISTIC_UUID_RX, NIMBLE_PROPERTY::WRITE);
  rxCharacteristic_->setCallbacks(new RxCallbacks());

  // TX characteristic (ESP32 -> Client)
  txCharacteristic_ = service_->createCharacteristic(CHARACTERISTIC_UUID_TX, NIMBLE_PROPERTY::INDICATE);
  txCharacteristic_->setCallbacks(new TxCallbacks());
  service_->start();

  advertising_ = NimBLEDevice::getAdvertising();
  advertising_->setName(DEVICE_NAME);
  advertising_->addServiceUUID(service_->getUUID());

  BleOta::getInstance().initialize(server_);

  advertising_->enableScanResponse(true);
  startAdvertising();

  DEBUG_INFO("BLE device: '%s', %s", DEVICE_NAME, NimBLEDevice::getAddress().toString().c_str());
  DEBUG_INFO("Initialize BLE Manager [OK]");
}

void BleManager::startAdvertising() {
  DEBUG_INFO("Start advertising BLE services");
  advertising_->start();
}

void BleManager::stopAdvertising() {
  DEBUG_INFO("Stop advertising BLE service");
  advertising_->stop();
}

void BleManager::onClientConnect(bool connected) {
  DEBUG_INFO("Client connected: %s", connected ? "true" : "false");
  connected_ = connected;

  if (connected_ == false) {
    subscribed_ = false;
    startAdvertising();
  }
}

void BleManager::onMtuChange(uint16_t mtu) {
  DEBUG_INFO("Net MTU changed: %d", mtu - AttPacketOverhead);
  net_mtu_ = mtu - AttPacketOverhead;
}

void BleManager::onSubscribe(bool subscribed) {
  DEBUG_INFO("Client subscribed: %s", subscribed ? "true" : "false");
  subscribed_ = subscribed;
}

void BleManager::onWriteConfirm(bool confirmed) {
  DEBUG_INFO("Write confirmed: %s", confirmed ? "true" : "false");
  tx_confirmed_ = confirmed;
}

void BleManager::onDataReceived(const uint8_t data[], size_t length) {
  if (data == nullptr || length == 0) {
    DEBUG_ERROR("Received data is NULL or empty!");
    return;
  }

  DEBUG_INFO("Data received (%zu): '%.*s'", length, static_cast<int>(length), data);
  Controller::getInstance().dataReceivedCallback(data, length);
}

bool BleManager::writeData(const uint8_t data[], size_t length) {
  if (txCharacteristic_ == nullptr || connected_ == false ||  //
      tx_ongoing_ == true || data == nullptr || length == 0) {
    return false;
  }
  if (subscribed_ == false) {
    DEBUG_ERROR("Client is not subscribed, cannot send data!");
    return false;
  }

  tx_data_ = const_cast<uint8_t*>(data);  // Store the data to be sent
  tx_length_ = length;                    // Store the length of the data
  tx_index_ = 0;                          // Reset the index for TX data

  DEBUG_INFO("Initiate TX of %zu bytes [...]", tx_length_);
  tx_start_time_ = millis();
  tx_confirmed_ = true;
  tx_ongoing_ = true;
  return true;
}

bool BleManager::writeDataChunk() {
  if (txCharacteristic_ == nullptr || connected_ == false ||  //
      tx_confirmed_ == false || tx_data_ == nullptr || tx_length_ == 0) {
    return false;
  }

  // Reset timeout timer for every chunk sent
  tx_start_time_ = millis();

  if (tx_index_ < tx_length_) {
    // More data to send, continue with next write cycle
    size_t remaining = tx_length_ - tx_index_;
    size_t length = (remaining < net_mtu_) ? remaining : net_mtu_;
    DEBUG_INFO("Send data (len: %zu) chunk ...", length);

    txCharacteristic_->setValue(tx_data_ + tx_index_, length);
    if (txCharacteristic_->indicate() == false) {
      DEBUG_ERROR("Failed to send indication!");
    }
    tx_index_ += length;  // Update index for next TX operation
    tx_confirmed_ = false;
    return true;
  }

  DEBUG_INFO("All data (len: %zu) sent [OK]", tx_length_);
  tx_ongoing_ = false;  // Reset flag after write operation
  tx_confirmed_ = false;
  return true;
}

void BleManager::run() {
  // Handle ongoing TX operations
  if (tx_ongoing_ == true) {
    // Check if next chunk can be sent
    if (tx_confirmed_ == true) {
      if (writeDataChunk() == false) {
        DEBUG_ERROR("Failed to write data chunk, aborting TX operation!");
        tx_ongoing_ = false;
        tx_confirmed_ = false;
      }
    } else if (millis() - tx_start_time_ >= TxTimeout) {
      DEBUG_ERROR("TX timeout expired, aborting TX operation!");
      tx_ongoing_ = false;
      tx_confirmed_ = false;
    }
  }

  BleOta::getInstance().run();
}
