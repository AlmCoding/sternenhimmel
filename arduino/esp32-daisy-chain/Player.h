#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

class Player {
  constexpr static uint32_t RampStepSizeMin = 1;
  constexpr static uint32_t RampTickTimeMinMs = 5;
  constexpr static uint32_t RampTickCountMax = static_cast<uint32_t>(BrgName::MAX) / RampStepSizeMin;
  constexpr static int RunTogglePin = 38;  // TP1 on pcb

 public:
  Player(const Player&) = delete;
  Player& operator=(const Player&) = delete;

  static Player& getInstance() {
    static Player instance;
    return instance;
  }

  void initialize();
  bool isIdle() const;
  void abort();
  void playSequence(const SequenceStep sequence[], size_t count);
  void run();

 private:
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
  void playStep(const SequenceStep& step);
  bool isStepValid(const SequenceStep& step) const;
  uint32_t elapsedTime(uint32_t start_ms) const;
  bool runRampDown();
  bool runPause();
  bool runRampUp(bool return_to_idle);
  bool runPulse(bool return_to_idle);

  State state_ = State::IDLE;
  uint32_t last_run_ms_ = 0;

  const SequenceStep* sequence_ = nullptr;
  size_t step_count_ = 0;
  size_t step_index_ = 0;

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
  bool return_to_idle_ = false;
};

#endif  // PLAYER_H