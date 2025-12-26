#include "DHTSensor.h"

DHTSensor::DHTSensor(uint8_t pin, uint8_t type) : dht(pin, type) {
  humidity = 0.0;
  temperature = 0.0;
  last_read_time =
      0 - DHT_UPDATE_INTERVAL; // Force immediate read on first update
}

void DHTSensor::begin() { dht.begin(); }

void DHTSensor::update() {
  unsigned long current_time = millis();
  if (current_time - last_read_time >= DHT_UPDATE_INTERVAL) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Check if any reads failed and exit early (to keep old values)
    if (isnan(h) || isnan(t)) {
      return;
    }

    humidity = h;
    temperature = t;
    last_read_time = current_time;
  }
}

void DHTSensor::get_json(char *buffer, size_t size) {
  // Integers are often enough for DHT11, but using 1 decimal place is safer
  int h_int = (int)humidity;
  int t_int = (int)temperature;
  // Or use float formatting if supported, but standard printf %f is heavy on
  // Arduino Let's stick to simple casting or dtostrf if needed. For simplicity
  // and memory safety in this context, casting to int or minimal decimal logic
  // is best. Actually, standard Arduino Print class supports float, but
  // snprintf %f might not be enabled by default on AVR. Let's manually format
  // to avoid bloat:

  // Format: "humidity": 50.00, "temperature": 25.00,
  // Using simple casting for safety if %f is not available.
  // Note: Standard AVR snprintf does NOT support %f.

  int h_dec = (int)((humidity - h_int) * 100);
  int t_dec = (int)((temperature - t_int) * 100);

  snprintf(buffer, size, "\"humidity\": %d.%02d, \"temperature\": %d.%02d,",
           h_int, abs(h_dec), t_int, abs(t_dec));
}