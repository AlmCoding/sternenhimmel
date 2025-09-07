#ifndef CHAIN_CONFIG_H
#define CHAIN_CONFIG_H

#include <Arduino.h>

#define ENABLE_DEBUG_OUTPUT 1
constexpr char FIRMWARE_VERSION[] = "V0.0.5";
constexpr char DIVIDER[] = "<=====================================>";
#define DISABLE_HARDWARE 0  // Set to 1 to disable hardware specific code for BLE testing
static_assert((ENABLE_DEBUG_OUTPUT == 0 && DISABLE_HARDWARE == 0) || (ENABLE_DEBUG_OUTPUT == 1),  //
              "ENABLE_DEBUG_OUTPUT must be 1 if DISABLE_HARDWARE is 0");

#if ENABLE_DEBUG_OUTPUT == 1
void debugPrint(const char* prefix, const char* fmt, ...);
#endif

#if ENABLE_DEBUG_OUTPUT == 1
#define ASSERT(condition)                 \
  do {                                    \
    if (!(condition)) {                   \
      Serial.print(F("ASSERT FAILED: ")); \
      Serial.print(F(__FILENAME__));      \
      Serial.print(F(":"));               \
      Serial.print(__LINE__);             \
      Serial.print(F(" => "));            \
      Serial.println(F(#condition));      \
      while (true) {                      \
        delay(100);                       \
      }                                   \
    }                                     \
  } while (0)
#else
#define ASSERT(condition)
#endif

// Number of chained boards
constexpr size_t CHAIN_COUNT = 6;
constexpr size_t CHAIN_SIZE = 10;
constexpr size_t LED_COUNT = 12;
constexpr size_t LED_COUNT_TOTAL = CHAIN_COUNT * CHAIN_SIZE * LED_COUNT;

enum class ChainIdx : uint8_t {
  CHAIN_0 = 0,
  CHAIN_1 = 1,
  CHAIN_2 = 2,
  CHAIN_3 = 3,
  CHAIN_4 = 4,
  CHAIN_5 = 5,
  CHAIN_COUNT = CHAIN_COUNT,
};

typedef uint8_t BrgNumber;

struct LedObj {
  ChainIdx chain_idx;
  uint8_t pcb_idx;
  uint8_t led_idx;
  BrgNumber brightness;
};

struct SequenceStep {
  LedObj* leds;
  size_t size;
  uint32_t ramp_down_duration_ms;
  uint32_t pause_duration_ms;
  uint32_t ramp_up_duration_ms;
  uint32_t pulse_duration_ms;
  uint32_t repetitions;  // Actually repetitions + 1
  // BrgNumber pause_brightness;
  // BrgNumber pulse_brightness;
  // bool pause_brightness_to_default;
  // bool pulse_brightness_to_default;
  bool idle_return;
};

enum class BrgName : BrgNumber {
  OFF = 0,
  MAX = 100,
};

constexpr size_t SystemIdMaxLength = 64;
const char* getSystemId();
void setSystemId(const char* identifier);

#endif  // CHAIN_CONFIG_H
