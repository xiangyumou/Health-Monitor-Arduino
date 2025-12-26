#ifndef Light_h
#define Light_h

#include <Arduino.h>

class Light {
public:
  Light(int _pin, int init_state = 0);
  void begin();
  void set(int state);
  void open();
  void close();
  int get_current_state();
  void opposite();
  void get_state_json(const char *key, char *buffer, size_t size);

private:
  void update_state();
  int pin, cur_state;
};

#endif