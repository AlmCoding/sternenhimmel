#include "common.h"
#include <Preferences.h>

#define DEBUG_ENABLE_COMMON 1
#if ((DEBUG_ENABLE_COMMON == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) debugPrint("[INF][common]", f, ##__VA_ARGS__)
#define DEBUG_ERROR(f, ...) debugPrint("[ERR][common]", f, ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#define DEBUG_ERROR(...)
#endif

static char system_id_[SystemIdMaxLength] = "";

void debugPrint(const char* prefix, const char* fmt, ...) {
  uint32_t ms = (uint32_t)millis();
  uint32_t sec = (ms / 1000) & 0x0F;  // wrap seconds 0..15
  uint16_t msec = ms % 1000;

  Serial.printf("[%u.%03u] %s: ", sec, msec, prefix);

  va_list ap;
  va_start(ap, fmt);
  Serial.vprintf(fmt, ap);
  va_end(ap);

  Serial.print("\n");
}

const char* getSystemId() {
  if (system_id_[0] == '\0') {
    Preferences preferences;
    preferences.begin("system", false);  // open (create if needed) the namespace in RW mode
    if (preferences.isKey("id") == true) {
      preferences.getString("id", system_id_, SystemIdMaxLength);
    } else {
      snprintf(system_id_, SystemIdMaxLength, "ESP32-%06X", ESP.getEfuseMac() & 0xFFFFFF);
      preferences.putString("id", system_id_);
      DEBUG_INFO("Generated new system ID: %s", system_id_);
    }
    preferences.end();
    system_id_[SystemIdMaxLength - 1] = '\0';  // Ensure null-termination
  }
  return system_id_;
}

void setSystemId(const char* identifier) {
  size_t len = strnlen(identifier, SystemIdMaxLength);
  if (identifier == nullptr || len == 0 || len >= SystemIdMaxLength) {
    DEBUG_ERROR("Invalid system identifier!");
    return;
  }

  Preferences preferences;
  preferences.begin("system", false);
  preferences.putString("id", identifier);
  preferences.end();
  strncpy(system_id_, identifier, SystemIdMaxLength);
  system_id_[SystemIdMaxLength - 1] = '\0';  // Ensure null-termination
}
