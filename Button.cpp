#include "Button.h"

Button::Button(int _pin) {
  pin = _pin;
  last_state = LOW;
  last_debounce_time = 0;
  button_state = LOW;
}

void Button::begin() { pinMode(pin, INPUT); }

int Button::read() {
  int reading = digitalRead(pin);

  if (reading != last_state) {
    last_debounce_time = millis();
  }

  if ((millis() - last_debounce_time) > 50) {
    if (reading != button_state) {
      button_state = reading;
    }
  }

  last_state = reading;
  return button_state;
}