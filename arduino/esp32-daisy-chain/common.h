#ifndef CHAIN_CONFIG_H
#define CHAIN_CONFIG_H

#include <Arduino.h>

#define ENABLE_DEBUG_OUTPUT 1

constexpr char DIVIDER[] = "<=====================================>";
constexpr uint8_t FIRMWARE_VERSION_MAJOR = 0;
constexpr uint8_t FIRMWARE_VERSION_MINOR = 0;
constexpr uint8_t FIRMWARE_VERSION_PATCH = 2;

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

// Gamma brightness lookup table <https://victornpb.github.io/gamma-table-generator>
// gamma = 2.0 steps = 101 range = 0-65535
static const uint16_t BRIGHTNESS_LINEARIZATION_TABLE[101] = {
  0,     7,     26,    59,    105,   164,   236,   321,   419,   531,   655,   793,   944,   1108,  1284,  1475,  1678,
  1894,  2123,  2366,  2621,  2890,  3172,  3467,  3775,  4096,  4430,  4778,  5138,  5511,  5898,  6298,  6711,  7137,
  7576,  8028,  8493,  8972,  9463,  9968,  10486, 11016, 11560, 12117, 12688, 13271, 13867, 14477, 15099, 15735, 16384,
  17046, 17721, 18409, 19110, 19824, 20552, 21292, 22046, 22813, 23593, 24386, 25192, 26011, 26843, 27689, 28547, 29419,
  30303, 31201, 32112, 33036, 33973, 34924, 35887, 36863, 37853, 38856, 39871, 40900, 41942, 42998, 44066, 45147, 46241,
  47349, 48470, 49603, 50750, 51910, 53083, 54270, 55469, 56681, 57907, 59145, 60397, 61662, 62940, 64231, 65535,
};

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

void debug_print(const char* prefix, const char* fmt, ...);

#endif  // CHAIN_CONFIG_H
