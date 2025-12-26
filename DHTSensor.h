#ifndef DHTSensor_h
#define DHTSensor_h

#include "DHT.h"
#include <Arduino.h>

// Define update interval in milliseconds (e.g., 2000ms for DHT11)
#define DHT_UPDATE_INTERVAL 2000

class DHTSensor {
public:
  DHTSensor(uint8_t pin, uint8_t type);
  void begin();
  void update();
  void get_json(char *buffer, size_t size);

private:
  DHT dht;
  float humidity;
  float temperature;
  unsigned long last_read_time;
};

#endif