#include "SoundSensor.h"

SoundSensor::SoundSensor(uint8_t _pin, int _window_size) {
  pin = _pin;
  if (_window_size > MAX_WINDOW_SIZE) {
    window_size = MAX_WINDOW_SIZE;
  } else {
    window_size = _window_size;
  }
}

void SoundSensor::begin() {
  bre_cnt = 0;
  head_index = 0;
  current_size = 0;
  last_state = 0;
  pinMode(pin, INPUT);

  // Initialize buffer
  for (int i = 0; i < MAX_WINDOW_SIZE; i++)
    breath_buffer[i] = 0;
}

void SoundSensor::update() {
  short t = digitalRead(pin);

  // Add to circular buffer
  breath_buffer[head_index] = t;
  head_index = (head_index + 1) % window_size;
  if (current_size < window_size)
    current_size++;

  // Only calculate when we have a full window
  if (current_size == window_size) {
    int ones = 0;
    int zeros = 0;

    // Count ones and zeros in the current window
    for (int i = 0; i < window_size; i++) {
      if (breath_buffer[i] == 1)
        ones++;
      else
        zeros++;
    }

    int state = (ones > zeros) ? 1 : 0;
    if (state != last_state) {
      last_state = state;
      bre_cnt++;
    }
  }
}

void SoundSensor::get_json(char *buffer, size_t size) {
  // The original logic shifted bre_cnt by 1 (divide by 2)
  int bc = bre_cnt >> 1;
  bre_cnt = 0;
  snprintf(buffer, size, "\"breathing\": %d,", bc);
}