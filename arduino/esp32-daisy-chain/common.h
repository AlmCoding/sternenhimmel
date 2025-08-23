#ifndef CHAIN_CONFIG_H
#define CHAIN_CONFIG_H

#include <Arduino.h>

#define ENABLE_DEBUG_OUTPUT 1

constexpr uint8_t FIRMWARE_VERSION_MAJOR = 0;
constexpr uint8_t FIRMWARE_VERSION_MINOR = 0;
constexpr uint8_t FIRMWARE_VERSION_PATCH = 1;

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

  GRAY = 5,
  BLUE = 53,
  GREEN = 72,
  RED = 82,
  ORANGE = 88,
  YELLOW = 95,

  MAX = 100,
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
  { BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF,          //
    BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::ORANGE, BrgName::GRAY },   // PCB1
  { BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY,         //
    BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY },     // PCB2
  { BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::OFF, BrgName::OFF,        //
    BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::BLUE, BrgName::BLUE },        // PCB3
  { BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY,      //
    BrgName::GRAY, BrgName::BLUE, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::ORANGE },  // PCB4
  { BrgName::GREEN, BrgName::BLUE, BrgName::ORANGE, BrgName::GRAY, BrgName::GRAY, BrgName::BLUE,   //
    BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::YELLOW },      // PCB5
  { BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF,            //
    BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::BLUE, BrgName::GRAY },    // PCB6
  { BrgName::RED, BrgName::GREEN, BrgName::GREEN, BrgName::GRAY, BrgName::BLUE, BrgName::YELLOW,   //
    BrgName::GREEN, BrgName::GRAY, BrgName::BLUE, BrgName::GREEN, BrgName::GRAY, BrgName::GRAY },  // PCB7
  { BrgName::BLUE, BrgName::BLUE, BrgName::GRAY, BrgName::GREEN, BrgName::GREEN, BrgName::BLUE,    //
    BrgName::BLUE, BrgName::OFF, BrgName::GRAY, BrgName::GREEN, BrgName::BLUE, BrgName::BLUE },    // PCB8
  { BrgName::GREEN, BrgName::OFF, BrgName::ORANGE, BrgName::OFF, BrgName::OFF, BrgName::YELLOW,    //
    BrgName::OFF, BrgName::BLUE, BrgName::OFF, BrgName::BLUE, BrgName::GREEN, BrgName::GRAY },     // PCB9
  { BrgName::GREEN, BrgName::GRAY, BrgName::GREEN, BrgName::GRAY, BrgName::GREEN, BrgName::GRAY,   //
    BrgName::BLUE, BrgName::GREEN, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF },       // PCB10
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_1[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::BLUE, BrgName::BLUE, BrgName::GRAY, BrgName::RED, BrgName::BLUE, BrgName::GRAY,         //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },      // PCB11
  { BrgName::BLUE, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF,             //
    BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GREEN, BrgName::YELLOW },       // PCB12
  { BrgName::BLUE, BrgName::BLUE, BrgName::BLUE, BrgName::YELLOW, BrgName::OFF, BrgName::GRAY,       //
    BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },      // PCB13
  { BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::OFF, BrgName::OFF, BrgName::OFF,           //
    BrgName::ORANGE, BrgName::GRAY, BrgName::BLUE, BrgName::ORANGE, BrgName::GRAY, BrgName::BLUE },  // PCB14
  { BrgName::BLUE, BrgName::GRAY, BrgName::BLUE, BrgName::BLUE, BrgName::BLUE, BrgName::GRAY,        //
    BrgName::RED, BrgName::OFF, BrgName::OFF, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY },         // PCB15
  { BrgName::BLUE, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::OFF, BrgName::OFF,           //
    BrgName::BLUE, BrgName::BLUE, BrgName::GRAY, BrgName::GREEN, BrgName::GRAY, BrgName::GREEN },    // PCB16
  { BrgName::GRAY, BrgName::GREEN, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::RED,        //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::RED },       // PCB17
  { BrgName::YELLOW, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY,       //
    BrgName::OFF, BrgName::GRAY, BrgName::YELLOW, BrgName::OFF, BrgName::GREEN, BrgName::BLUE },     // PCB18
  { BrgName::BLUE, BrgName::GREEN, BrgName::GREEN, BrgName::OFF, BrgName::GRAY, BrgName::GRAY,       //
    BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::BLUE },          // PCB19
  { BrgName::GRAY, BrgName::RED, BrgName::OFF, BrgName::BLUE, BrgName::OFF, BrgName::ORANGE,         //
    BrgName::GRAY, BrgName::OFF, BrgName::RED, BrgName::OFF, BrgName::GREEN, BrgName::GREEN },       // PCB20
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_2[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::BLUE,         //
    BrgName::GREEN, BrgName::OFF, BrgName::GRAY, BrgName::BLUE, BrgName::GREEN, BrgName::GRAY },  // PCB21
  { BrgName::GRAY, BrgName::BLUE, BrgName::ORANGE, BrgName::GREEN, BrgName::GREEN, BrgName::OFF,  //
    BrgName::GREEN, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::GRAY },     // PCB22
  { BrgName::BLUE, BrgName::ORANGE, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY,    //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::GREEN, BrgName::GRAY },  // PCB23
  { BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY,     //
    BrgName::GRAY, BrgName::RED, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },    // PCB24
  { BrgName::YELLOW, BrgName::GRAY, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::BLUE,   //
    BrgName::GRAY, BrgName::GRAY, BrgName::GREEN, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY },  // PCB25
  { BrgName::GRAY, BrgName::GREEN, BrgName::RED, BrgName::GRAY, BrgName::BLUE, BrgName::BLUE,     //
    BrgName::GREEN, BrgName::GRAY, BrgName::RED, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },   // PCB26
  { BrgName::GREEN, BrgName::BLUE, BrgName::BLUE, BrgName::BLUE, BrgName::GRAY, BrgName::OFF,     //
    BrgName::RED, BrgName::OFF, BrgName::RED, BrgName::OFF, BrgName::BLUE, BrgName::GRAY },       // PCB27
  { BrgName::GRAY, BrgName::GREEN, BrgName::OFF, BrgName::YELLOW, BrgName::OFF, BrgName::OFF,     //
    BrgName::RED, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },      // PCB28
  { BrgName::RED, BrgName::RED, BrgName::RED, BrgName::OFF, BrgName::BLUE, BrgName::BLUE,         //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::RED },     // PCB29
  { BrgName::BLUE, BrgName::GREEN, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY,     //
    BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },    // PCB30
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_3[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::OFF,       //
    BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::BLUE, BrgName::RED, BrgName::BLUE },       // PCB31
  { BrgName::RED, BrgName::GREEN, BrgName::OFF, BrgName::BLUE, BrgName::OFF, BrgName::GRAY,       //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },   // PCB32
  { BrgName::GREEN, BrgName::BLUE, BrgName::GREEN, BrgName::GREEN, BrgName::GREEN, BrgName::RED,  //
    BrgName::BLUE, BrgName::GRAY, BrgName::RED, BrgName::BLUE, BrgName::GREEN, BrgName::BLUE },   // PCB33
  { BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY,     //
    BrgName::BLUE, BrgName::GRAY, BrgName::BLUE, BrgName::RED, BrgName::RED, BrgName::YELLOW },   // PCB34
  { BrgName::RED, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::RED, BrgName::OFF,        //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },   // PCB35
  { BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY,     //
    BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::GRAY },       // PCB36
  { BrgName::GREEN, BrgName::GRAY, BrgName::BLUE, BrgName::GREEN, BrgName::RED, BrgName::BLUE,    //
    BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::GRAY },    // PCB37
  { BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY,       //
    BrgName::OFF, BrgName::OFF, BrgName::GREEN, BrgName::GRAY, BrgName::BLUE, BrgName::GRAY },    // PCB38
  { BrgName::RED, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::BLUE, BrgName::GREEN,        //
    BrgName::OFF, BrgName::GRAY, BrgName::ORANGE, BrgName::OFF, BrgName::OFF, BrgName::GRAY },    // PCB39
  { BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::OFF,       //
    BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GRAY },        // PCB40
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_4[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::BLUE,       //
    BrgName::GREEN, BrgName::GRAY, BrgName::OFF, BrgName::GREEN, BrgName::BLUE, BrgName::GRAY },   // PCB41
  { BrgName::GRAY, BrgName::BLUE, BrgName::GRAY, BrgName::RED, BrgName::OFF, BrgName::BLUE,        //
    BrgName::GRAY, BrgName::RED, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY },      // PCB42
  { BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::BLUE, BrgName::OFF, BrgName::OFF,          //
    BrgName::OFF, BrgName::GRAY, BrgName::BLUE, BrgName::BLUE, BrgName::GRAY, BrgName::BLUE },     // PCB43
  { BrgName::BLUE, BrgName::GRAY, BrgName::BLUE, BrgName::BLUE, BrgName::BLUE, BrgName::GREEN,     //
    BrgName::RED, BrgName::BLUE, BrgName::BLUE, BrgName::ORANGE, BrgName::GREEN, BrgName::GRAY },  // PCB44
  { BrgName::BLUE, BrgName::BLUE, BrgName::BLUE, BrgName::RED, BrgName::GREEN, BrgName::GRAY,      //
    BrgName::BLUE, BrgName::RED, BrgName::BLUE, BrgName::BLUE, BrgName::BLUE, BrgName::BLUE },     // PCB45
  { BrgName::ORANGE, BrgName::ORANGE, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::OFF,   //
    BrgName::BLUE, BrgName::ORANGE, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::BLUE },   // PCB46
  { BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::OFF,        //
    BrgName::BLUE, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::OFF, BrgName::GRAY },       // PCB47
  { BrgName::GRAY, BrgName::RED, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY,        //
    BrgName::GRAY, BrgName::RED, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF },         // PCB48
  { BrgName::ORANGE, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::OFF, BrgName::GRAY,       //
    BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::OFF, BrgName::GRAY },        // PCB49
  { BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF,            //
    BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },       // PCB50
};

static const BrgName DEFAULT_BRIGHTNESS_CHAIN_5[CHAIN_SIZE][LED_COUNT] = {
  { BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::OFF,          //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::GRAY },       // PCB51
  { BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::GRAY, BrgName::GRAY,          //
    BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY },       // PCB52
  { BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF,           //
    BrgName::OFF, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY },      // PCB53
  { BrgName::GRAY, BrgName::BLUE, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::OFF,         //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::OFF, BrgName::GRAY, BrgName::GRAY },      // PCB54
  { BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::OFF, BrgName::BLUE, BrgName::OFF,         //
    BrgName::YELLOW, BrgName::OFF, BrgName::BLUE, BrgName::GREEN, BrgName::BLUE, BrgName::BLUE },   // PCB55
  { BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::RED, BrgName::BLUE,        //
    BrgName::GRAY, BrgName::GRAY, BrgName::GRAY, BrgName::GREEN, BrgName::BLUE, BrgName::GRAY },    // PCB56
  { BrgName::GREEN, BrgName::RED, BrgName::RED, BrgName::BLUE, BrgName::RED, BrgName::GRAY,         //
    BrgName::YELLOW, BrgName::GRAY, BrgName::GRAY, BrgName::BLUE, BrgName::GREEN, BrgName::GRAY },  // PCB57
  { BrgName::BLUE, BrgName::YELLOW, BrgName::RED, BrgName::OFF, BrgName::GRAY, BrgName::GRAY,       //
    BrgName::OFF, BrgName::GREEN, BrgName::GREEN, BrgName::GRAY, BrgName::GREEN, BrgName::GRAY },   // PCB58
  { BrgName::GRAY, BrgName::OFF, BrgName::BLUE, BrgName::GRAY, BrgName::YELLOW, BrgName::BLUE,      //
    BrgName::GREEN, BrgName::BLUE, BrgName::BLUE, BrgName::RED, BrgName::RED, BrgName::GRAY },      // PCB59
  { BrgName::BLUE, BrgName::BLUE, BrgName::OFF, BrgName::OFF, BrgName::OFF, BrgName::OFF,           //
    BrgName::GREEN, BrgName::GRAY, BrgName::BLUE, BrgName::BLUE, BrgName::GRAY, BrgName::GRAY },    // PCB60
};

#endif  // CHAIN_CONFIG_H
