#include "BleOta.h"
#include <NimBLEDis.h>
#include <NimBLEOta.h>

#define DEBUG_ENABLE_BLEOTA 1
#if ((DEBUG_ENABLE_BLEOTA == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) debugPrint("[INF][BleOta]", f, ##__VA_ARGS__)
#define DEBUG_ERROR(f, ...) debugPrint("[ERR][BleOta]", f, ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#define DEBUG_ERROR(...)
#endif

NimBLEOta bleOta;
NimBLEDis bleDis;

class OtaCallbacks : public NimBLEOtaCallbacks {
  void onStart(NimBLEOta* ota, uint32_t firmwareSize, NimBLEOta::Reason reason) override {
    if (reason == NimBLEOta::StartCmd) {
      DEBUG_INFO("Start OTA with firmware size: %d\n", firmwareSize);
      return;
    }

    if (reason == NimBLEOta::Reconnected) {
      DEBUG_INFO("Reconnected, resuming OTA Update");
      ota->stopAbortTimer();
      return;
    }

    DEBUG_INFO("OTA start, firmware size: %d, Reason: %u\n", firmwareSize, reason);
  }

  void onProgress(NimBLEOta* ota, uint32_t current, uint32_t total) override {
    DEBUG_INFO("OTA progress: %.1f%%, cur: %u, tot: %u\n", static_cast<float>(current) / total * 100, current, total);
  }

  void onStop(NimBLEOta* ota, NimBLEOta::Reason reason) override {
    if (reason == NimBLEOta::Disconnected) {
      DEBUG_INFO("OTA stopped, client disconnected");
      ota->startAbortTimer(30);  // abort if client does not restart ota in 30 seconds
      return;
    }

    if (reason == NimBLEOta::StopCmd) {
      DEBUG_INFO("OTA stopped by command - aborting");
      ota->abortUpdate();
      return;
    }

    DEBUG_INFO("OTA stopped, Reason: %u\n", reason);
  }

  void onComplete(NimBLEOta* ota) override {
    DEBUG_INFO("OTA update completed successfully");
    BleOta::getInstance().complete();
  }

  void onError(NimBLEOta* ota, esp_err_t err, NimBLEOta::Reason reason) override {
    DEBUG_INFO("OTA error: %d\n", err);
    if (reason == NimBLEOta::FlashError) {
      DEBUG_INFO("Flash error, aborting OTA update!");
      ota->abortUpdate();
    }
  }
} otaCallbacks;

void BleOta::initialize(NimBLEServer* server) {
  DEBUG_INFO("Initialize BLE OTA [...]");

  bleDis.init();
  bleDis.setManufacturerName("ReardenLabs");
  bleDis.setModelNumber("DaisyChain-01");
  bleDis.setFirmwareRevision(FIRMWARE_VERSION);
  bleDis.setHardwareRevision("V0.1");
  // bleDis.setSoftwareRevision("1.0.0");
  // bleDis.setSystemId(getSystemId());
  // bleDis.setPnp(0x01, 0x02, 0x03, 0x04);

  bleDis.start();
  bleOta.start(&otaCallbacks);

  NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(bleOta.getServiceUUID());

  DEBUG_INFO("Initialize BLE OTA [OK]");
}

void BleOta::complete() {
  complete_ = true;
  complete_time_ = millis();
  DEBUG_INFO("BLE OTA marked as complete, device will restart in %u ms", RestartDelay);
}

void BleOta::run() {
  if (complete_ == true && (millis() - complete_time_ > RestartDelay)) {
    DEBUG_INFO("Restarting device after OTA update ...");
    complete_ = false;
    ESP.restart();
  }
}
