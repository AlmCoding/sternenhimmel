#include "Player.h"
#include "DaisyChain.h"

#define DEBUG_ENABLE_PLAYER 1
#if ((DEBUG_ENABLE_PLAYER == 1) && (ENABLE_DEBUG_OUTPUT == 1))
#define DEBUG_INFO(f, ...) debug_print("[INF][Player]", f, ##__VA_ARGS__)
#define DEBUG_ERROR(f, ...) debug_print("[ERR][Player]", f, ##__VA_ARGS__)
#else
#define DEBUG_INFO(...)
#define DEBUG_ERROR(...)
#endif

void Player::initialize() {
  DEBUG_INFO("Initialize Player [...]");
  pinMode(RunTogglePin, OUTPUT);
  digitalWrite(RunTogglePin, LOW);
  last_run_ms_ = millis();
  DEBUG_INFO("Initialize Player [OK]");
}

bool Player::is_idle() const {
  return state_ == State::IDLE;
}

void Player::abort() {
  DaisyChain::getInstance().apply_idle_values();
  state_ = State::IDLE;
}

void Player::play_sequence(const SequenceStep sequence[], size_t count) {
  if (count == 0 || sequence == nullptr) {
    DEBUG_ERROR("Invalid sequence or count!");
    return;
  }

  bool invalid_step = false;
  for (size_t i = 0; i < count; i++) {
    if (is_step_valid(sequence[i]) == false) {
      DEBUG_ERROR("Invalid sequence step at index %zu!", i);
      invalid_step = true;
      continue;
    }
  }

  if (invalid_step == true) {
    DEBUG_ERROR("One or more sequence steps are invalid, aborting sequence!");
    return;
  }

  if (state_ != State::IDLE) {
    DEBUG_INFO("Player is not idle, cannot play sequence step!");
    return;
  }

  DEBUG_INFO("Play sequence with %zu steps [...]", count);

  sequence_ = const_cast<SequenceStep*>(sequence);
  step_count_ = count;
  step_index_ = 0;
  play_step(sequence_[step_index_]);
}

void Player::play_step(const SequenceStep& step) {
  leds_ = step.leds;
  size_ = step.size;

  ramp_down_.duration_ms = step.ramp_down_duration_ms;
  ramp_down_.start_ms = 0;
  ramp_down_.started = false;

  pause_.duration_ms = step.pause_duration_ms;
  pause_.start_ms = 0;
  pause_.started = false;

  ramp_up_.duration_ms = step.ramp_up_duration_ms;
  ramp_up_.start_ms = 0;
  ramp_up_.started = false;

  pulse_.duration_ms = step.pulse_duration_ms;
  pulse_.start_ms = 0;
  pulse_.started = false;

  repetitions_ = step.repetitions;
  return_to_idle_ = step.idle_return;

  DEBUG_INFO(
      "Play step (%zu/%zu) with %zu LEDs, ramp down: %u ms, pause: %u ms, ramp up: %u ms, pulse: %u ms, "
      "repetitions: %u",
      step_index_ + 1, step_count_, size_, ramp_down_.duration_ms, pause_.duration_ms, ramp_up_.duration_ms,
      pulse_.duration_ms, repetitions_);
  state_ = State::RAMP_DOWN;
}

bool Player::is_step_valid(const SequenceStep& step) const {
  if (step.leds == nullptr) {
    DEBUG_ERROR("Invalid leds pointer!");
    return false;
  } else if (step.size > LED_COUNT_TOTAL) {
    DEBUG_ERROR("Invalid leds size: %d", step.size);
    return false;
  } else if (step.repetitions == 0) {
    DEBUG_ERROR("Repetitions must be greater than 0!");
    return false;
  }

  for (size_t i = 0; i < step.size; i++) {
    if (static_cast<uint8_t>(step.leds[i].chain_idx) >= static_cast<uint8_t>(ChainIdx::CHAIN_COUNT)) {
      DEBUG_ERROR("Invalid chain index: %u", step.leds[i].chain_idx);
      return false;
    } else if (step.leds[i].led_idx >= LED_COUNT) {
      DEBUG_ERROR("Invalid led index: %u", step.leds[i].led_idx);
      return false;
    } else if (step.leds[i].brightness > static_cast<BrgNumber>(BrgName::MAX)) {
      DEBUG_ERROR("Invalid brightness value: %u", step.leds[i].brightness);
      return false;
    }
  }

  return true;
}

void Player::run() {
  uint32_t run_delay_ms = millis() - last_run_ms_;
  if (run_delay_ms >= RampTickTimeMinMs) {
    DEBUG_ERROR("Player run delay too long: %u ms", run_delay_ms);
  }
  digitalWrite(RunTogglePin, !digitalRead(RunTogglePin));
  last_run_ms_ = millis();

  if (state_ == State::IDLE) {
    return;
  }

  if (state_ == State::RAMP_DOWN) {
    if (run_ramp_down() == true) {
      state_ = State::PAUSE;
    } else {
      return;
    }
  }

  if (state_ == State::PAUSE) {
    if (run_pause() == true) {
      state_ = State::RAMP_UP;
    } else {
      return;
    }
  }

  bool return_to_idle = (repetitions_ == 1) && (return_to_idle_ == true);

  if (state_ == State::RAMP_UP) {
    if (run_ramp_up(return_to_idle) == true) {
      state_ = State::PULSE;
    } else {
      return;
    }
  }

  if (state_ == State::PULSE) {
    if (run_pulse(return_to_idle) == true) {
      repetitions_--;
      if (repetitions_ > 0) {
        state_ = State::RAMP_DOWN;
      } else {
        state_ = State::IDLE;

        if (step_index_ < step_count_ - 1) {
          step_index_++;
          play_step(sequence_[step_index_]);
        } else {
          DEBUG_INFO("All steps of sequence played, returning to IDLE state [OK]");
        }
      }
    } else {
      return;
    }
  }
}

uint32_t Player::elapsed_time(uint32_t start_ms) const {
  uint32_t now_ms = millis();
  if (now_ms < start_ms) {
    return UINT32_MAX - start_ms + now_ms;
  }
  return millis() - start_ms;
}

bool Player::run_ramp_down() {
  bool complete = false;

  if (ramp_down_.started == false) {
    if (ramp_down_.duration_ms == 0) {
      // DEBUG_INFO("Skip ramp down!");
      complete = true;
      return complete;
    }
    // DEBUG_INFO("Start ramp down!");

    DaisyChain::getInstance().get_active_leds(leds_, size_);
    total_ramp_ticks_ = ramp_down_.duration_ms / RampTickTimeMinMs;

    if (total_ramp_ticks_ > RampTickCountMax) {
      total_ramp_ticks_ = RampTickCountMax;
      ramp_tick_time_ms_ = ramp_down_.duration_ms / total_ramp_ticks_;
      ramp_step_size_ = RampStepSizeMin;

    } else {
      ramp_tick_time_ms_ = RampTickTimeMinMs;
      ramp_step_size_ = static_cast<uint32_t>(BrgName::MAX) / total_ramp_ticks_;
    }
    remaining_ramp_ticks_ = total_ramp_ticks_;

    ramp_down_.start_ms = millis();
    ramp_down_.started = true;

  } else if (elapsed_time(ramp_down_.start_ms) >= ramp_tick_time_ms_) {
    remaining_ramp_ticks_--;

    BrgNumber new_brightness = remaining_ramp_ticks_ * ramp_step_size_;
    for (size_t i = 0; i < size_; i++) {
      if (leds_[i].brightness > new_brightness) {
        leds_[i].brightness = new_brightness;
      }
    }
    DaisyChain::getInstance().set_active_leds(leds_, size_);

    ramp_down_.start_ms = millis();

    if (remaining_ramp_ticks_ == 0) {
      ramp_down_.started = false;
      ramp_down_.start_ms = 0;
      complete = true;
    }
  }

  return complete;
}

bool Player::run_pause() {
  bool complete = false;

  if (pause_.started == false) {
    if (pause_.duration_ms == 0) {
      // DEBUG_INFO("Skip pause!");
      complete = true;
      return complete;
    }
    // DEBUG_INFO("Start pause!");

    for (size_t i = 0; i < size_; i++) {
      leds_[i].brightness = static_cast<BrgNumber>(BrgName::OFF);
    }
    DaisyChain::getInstance().set_active_leds(leds_, size_);

    pause_.start_ms = millis();
    pause_.started = true;

  } else if (elapsed_time(pause_.start_ms) >= pause_.duration_ms) {
    pause_.started = false;
    pause_.start_ms = 0;
    complete = true;
  }

  return complete;
}

bool Player::run_ramp_up(bool return_to_idle) {
  bool complete = false;

  if (ramp_up_.started == false) {
    if (ramp_up_.duration_ms == 0) {
      // DEBUG_INFO("Skip ramp up!");
      complete = true;
      return complete;
    }
    // DEBUG_INFO("Start ramp up!");

    DaisyChain::getInstance().get_active_leds(leds_, size_);
    total_ramp_ticks_ = ramp_up_.duration_ms / RampTickTimeMinMs;

    if (total_ramp_ticks_ > RampTickCountMax) {
      total_ramp_ticks_ = RampTickCountMax;
      ramp_tick_time_ms_ = ramp_up_.duration_ms / total_ramp_ticks_;
      ramp_step_size_ = RampStepSizeMin;

    } else {
      ramp_tick_time_ms_ = RampTickTimeMinMs;
      ramp_step_size_ = static_cast<uint32_t>(BrgName::MAX) / total_ramp_ticks_;
    }
    remaining_ramp_ticks_ = total_ramp_ticks_;

    ramp_up_.start_ms = millis();
    ramp_up_.started = true;

  } else if (elapsed_time(ramp_up_.start_ms) >= ramp_tick_time_ms_) {
    remaining_ramp_ticks_--;

    BrgNumber new_brightness = (total_ramp_ticks_ - remaining_ramp_ticks_) * ramp_step_size_;
    for (size_t i = 0; i < size_; i++) {
      if (return_to_idle == true) {
        DaisyChain::getInstance().get_idle_leds(leds_ + i, 1);
        if (new_brightness < leds_[i].brightness) {
          // Set new brightness because it is less than idle value
          leds_[i].brightness = new_brightness;
        }
      } else if (leds_[i].brightness < new_brightness) {
        leds_[i].brightness = new_brightness;
      }
    }
    DaisyChain::getInstance().set_active_leds(leds_, size_);

    ramp_up_.start_ms = millis();

    if (remaining_ramp_ticks_ == 0) {
      ramp_up_.started = false;
      ramp_up_.start_ms = 0;
      complete = true;
    }
  }

  return complete;
}

bool Player::run_pulse(bool return_to_idle) {
  bool complete = false;

  if (pulse_.started == false) {
    if (pulse_.duration_ms == 0) {
      // DEBUG_INFO("Skip pulse!");
      complete = true;
      return complete;
    }

    // DEBUG_INFO("Start pulse!");
    if (return_to_idle == true) {
      // Set all LEDs to idle brightness
      DaisyChain::getInstance().get_idle_leds(leds_, size_);
    } else {
      // Set all LEDs to maximum brightness
      for (size_t i = 0; i < size_; i++) {
        leds_[i].brightness = static_cast<BrgNumber>(BrgName::MAX);
      }
    }
    DaisyChain::getInstance().set_active_leds(leds_, size_);

    pulse_.start_ms = millis();
    pulse_.started = true;

  } else if (elapsed_time(pulse_.start_ms) >= pulse_.duration_ms) {
    pulse_.started = false;
    pulse_.start_ms = 0;
    complete = true;
  }

  return complete;
}
