#ifndef SoundSensor_h
#define SoundSensor_h

#include <Arduino.h>

#define MAX_WINDOW_SIZE 64

class SoundSensor {
public:
  SoundSensor(uint8_t _pin, int _window_size);
  void begin();
  void update();
  void get_json(char *buffer, size_t size);

private:
  uint8_t pin;
  int window_size;
  short breath_buffer[MAX_WINDOW_SIZE];
  int head_index;
  int current_size;

  int last_state;
  int bre_cnt;
};

#endif