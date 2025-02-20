#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

class Player {
 public:
  Player(const Player&) = delete;
  Player& operator=(const Player&) = delete;

  static Player& getInstance() {
    static Player instance;
    return instance;
  }

  void initialize();
  bool is_idle() const;
  void abort();
  void play(const SequenceStep& step);
  void run();

 private:
  constexpr static uint32_t RampStepSizeMin = 1;
  constexpr static uint32_t RampTickTimeMinMs = 10;
  constexpr static uint32_t RampTickCountMax = static_cast<uint32_t>(BrgName::BRG_MAX) / RampStepSizeMin;

  enum class State {
    IDLE = 0,
    RAMP_DOWN,
    PAUSE,
    RAMP_UP,
    PULSE,
  };

  struct Step {
    uint32_t duration_ms;
    uint32_t start_ms;
    bool started;
  };

  Player() = default;
  bool is_step_valid(const SequenceStep& step) const;
  uint32_t elapsed_time(uint32_t start_ms) const;
  bool run_ramp_down();
  bool run_pause();
  bool run_ramp_up();
  bool run_pulse();

  State state_ = State::IDLE;

  LedObj* leds_ = nullptr;
  size_t size_ = 0;

  Step ramp_down_ = { 0, 0, false };
  Step pause_ = { 0, 0, false };
  Step ramp_up_ = { 0, 0, false };
  Step pulse_ = { 0, 0, false };

  uint32_t total_ramp_ticks_ = 0;
  uint32_t remaining_ramp_ticks_ = 0;
  uint32_t ramp_tick_time_ms_ = 0;
  uint32_t ramp_step_size_ = 0;

  uint32_t repetitions_ = 0;
};

#endif  // PLAYER_H