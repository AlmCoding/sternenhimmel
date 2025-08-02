#ifndef CHAIN_CONFIG_H
#define CHAIN_CONFIG_H

#include <Arduino.h>

#define ENABLE_DEBUG_OUTPUT 1

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
};

enum class BrgName : BrgNumber {
  BRG_OFF = 0,
  BRG_LOW = 10,
  BRG_MEDIUM = 50,
  BRG_MAX = 100,
};

// Gamma brightness lookup table <https://victornpb.github.io/gamma-table-generator>
// gamma = 2.20 steps = 101 range = 0-65535
static const uint16_t BRIGHTNESS_LINEARIZATION_TABLE[101] = {
  0,     3,     12,    29,    55,    90,    134,   189,   253,   328,   413,   510,   618,   736,   867,   1009,  1163,
  1329,  1507,  1697,  1900,  2115,  2343,  2584,  2838,  3104,  3384,  3677,  3983,  4303,  4636,  4983,  5343,  5717,
  6106,  6508,  6924,  7354,  7798,  8257,  8730,  9217,  9719,  10235, 10766, 11312, 11872, 12448, 13038, 13643, 14263,
  14898, 15548, 16214, 16894, 17590, 18302, 19028, 19770, 20528, 21301, 22090, 22895, 23715, 24551, 25403, 26271, 27154,
  28054, 28970, 29901, 30849, 31813, 32793, 33790, 34802, 35831, 36877, 37939, 39017, 40112, 41223, 42351, 43496, 44657,
  45835, 47029, 48241, 49469, 50714, 51976, 53255, 54551, 55864, 57195, 58542, 59906, 61287, 62686, 64102, 65535,
};

static const char DIVIDER[] = "<=====================================>";
static const char ACCESS_POINT_SSID[] = "Sternenhimmel";
static const char ACCESS_POINT_PASSWORD[] = "Sternenhimmel";
static const int WEB_SERVER_PORT = 80;

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_0[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB0 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB1 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB2 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB3 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB4 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB5 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB6 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB7 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB8 (chain 0)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB9 (chain 0)
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_1[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB0 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB1 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB2 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB3 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB4 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB5 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB6 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB7 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB8 (chain 1)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB9 (chain 1)
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_2[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB0 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB1 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB2 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB3 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB4 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB5 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB6 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB7 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB8 (chain 2)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB9 (chain 2)
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_3[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB0 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB1 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB2 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB3 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB4 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB5 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB6 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB7 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB8 (chain 3)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB9 (chain 3)
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_4[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB0 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB1 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB2 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB3 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB4 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB5 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB6 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB7 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB8 (chain 4)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB9 (chain 4)
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_5[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB0 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB1 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB2 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB3 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB4 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB5 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB6 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB7 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB8 (chain 5)
  { BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF,    //
    BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF, BrgName::BRG_OFF },  // PCB9 (chain 5)
};

#endif  // CHAIN_CONFIG_H
