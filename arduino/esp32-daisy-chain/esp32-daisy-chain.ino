#include "BleManager.h"
#include "Controller.h"
#include "DaisyChain.h"
#include "Player.h"
#include "common.h"

#define DEBUG_ENABLE_MAIN 1
#if ((DEBUG_ENABLE_MAIN == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) debugPrint("[INF][Main]", f, ##__VA_ARGS__)
#define DEBUG_ERROR(f, ...) debugPrint("[ERR][Main]", f, ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#define DEBUG_ERROR(...)
#endif

constexpr uint32_t RefreshIntervalMs = 3000;
uint32_t last_refresh_ms = 0;
bool force_refresh = false;

void setup() {
  setCpuFrequencyMhz(240);

#if (ENABLE_DEBUG_OUTPUT == 1)
  Serial.begin(921600);
#endif
  DEBUG_INFO("%s", DIVIDER);
  DEBUG_INFO("Setup ESP32-daisy-chain [...]");
  DEBUG_INFO("Firmware version: %s", FIRMWARE_VERSION);
  DEBUG_INFO("System id: '%s'", getSystemId());
  DEBUG_INFO("Free heap: %u bytes", ESP.getFreeHeap());
  DEBUG_INFO("CPU frequency: %d MHz", getCpuFrequencyMhz());
  sleep(0.25);

  DaisyChain::getInstance().initialize();
  Player::getInstance().initialize();
  Controller::getInstance().initialize();
  BleManager::getInstance().initialize();

  DEBUG_INFO("Setup ESP32-daisy-chain [OK]");
  DEBUG_INFO("%s", DIVIDER);
}

void loop() {
  BleManager::getInstance().run();
  Controller::getInstance().run();
  Player::getInstance().run();

  if (Player::getInstance().isIdle() == true && (millis() - last_refresh_ms) >= RefreshIntervalMs) {
    DEBUG_INFO("Periodic refresh of all chains ...");
    last_refresh_ms = millis();
    force_refresh = true;
  }

  DaisyChain::getInstance().flushAll(force_refresh);
  force_refresh = false;
}
