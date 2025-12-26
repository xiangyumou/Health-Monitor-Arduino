#include "src/Button.h"
#include "src/DHTSensor.h"
#include "src/Light.h"
#include "src/SoundSensor.h"

#define DEVICE_CODE "b5ab92178e4b404b9f83a98abe2d7ce3"

// Pins
#define DHT11_PIN 2
#define SOUND_SENSOR_PIN 3
#define LIGHT_PIN 4
#define BUTTON_CANCEL_PIN 5
#define BUTTON_ACTIVATE_PIN 6
#define RX 7
#define TX 9

// Settings
#define REPORT_INTERVAL 3000 // Report every 3 seconds
#define PORT 115200

// Objects
DHTSensor dht(DHT11_PIN, DHT11);
SoundSensor sds(SOUND_SENSOR_PIN, 30);
Light lgt(LIGHT_PIN, 1);
Button cel_btn(BUTTON_CANCEL_PIN);
Button act_btn(BUTTON_ACTIVATE_PIN);

// Global Variables
unsigned long last_report_time = 0;

void setup() {
  Serial.begin(PORT);
  while (!Serial)
    delay(10);

  // ESP8266/WiFi Module Serial
  Serial2.begin(9600);
  while (!Serial2)
    delay(10);

  dht.begin();
  sds.begin();
  lgt.begin();
  cel_btn.begin();
  act_btn.begin();
}

void loop() {
  // 1. Update Sensors (Non-blocking internal logic)
  dht.update();
  sds.update();

  // 2. Handle Buttons with Debounce
  int cancel_pressed = cel_btn.read();
  if (cancel_pressed) {
    lgt.close();
  }

  int activate_pressed = act_btn.read();
  if (activate_pressed) {
    lgt.open();
  }

  // 3. Periodic Reporting (Non-blocking)
  unsigned long current_time = millis();
  if (current_time - last_report_time >= REPORT_INTERVAL) {
    last_report_time = current_time;

    // Prepare JSON buffer. Size calculation:
    // DHT: "humidity": 100.00, "temperature": 100.00, ~ 50 chars
    // Device: "deviceCode": "32chars", ~ 50 chars
    // Sound: "breathing": 1000, ~ 20 chars
    // Light: "alert": 1, ~ 15 chars
    // Overhead: { } ~ 5 chars
    // Total safe buffer: 256 bytes
    char json_buffer[256];
    char temp_buffer[64];

    // Start JSON coding
    strcpy(json_buffer, "{");

    // Append Sound
    sds.get_json(temp_buffer, sizeof(temp_buffer));
    strcat(json_buffer, temp_buffer);

    // Append DHT
    dht.get_json(temp_buffer, sizeof(temp_buffer));
    strcat(json_buffer, temp_buffer);

    // Append Device ID
    snprintf(temp_buffer, sizeof(temp_buffer), "\"deviceCode\": \"%s\",",
             DEVICE_CODE);
    strcat(json_buffer, temp_buffer);

    // Append Light (last item) - Note: Original code had trailing space/bracket
    // logic Original: "json }"[i == n-1] which usually implies no trailing
    // comma for last item? Actually standard JSON doesn't allow trailing comma.
    // The previous get_json implementation likely left a trailing comma if not
    // careful, or the loop `res += data_list[i] + " }"[i == n - 1];` handled
    // it. Let's be manual but safe.

    // We have trailing commas in current get_json implementations...
    // Let's fix that by NOT putting commas in individual get_json, or stripping
    // the last one. Or just putting the Light state last and manually closing.

    lgt.get_state_json("alert", temp_buffer, sizeof(temp_buffer));
    // temp_buffer has trailing comma from previous implementation?
    // Let's check Light.cpp... Yes: snprintf(buffer, size, "\"%s\": %d,", key,
    // get_current_state()); We should probably remove that trailing comma for
    // the Main JSON.
    strcat(json_buffer, temp_buffer);

    // Remove last trailing comma if exists
    size_t len = strlen(json_buffer);
    if (json_buffer[len - 1] == ',') {
      json_buffer[len - 1] =
          ' '; // Replace with space or null term? JSON can end with " }"
    }

    strcat(json_buffer, "}");

    // Send
    Serial2.println(json_buffer);
    // Serial.println(json_buffer); // Debug
  }

  // 4. Handle Incoming Commands (Non-blocking)
  while (Serial2.available() > 0) {
    char c = Serial2.read();
    // Simple protocol: '0' = close, others (if meaningful?)
    // Original: readString() and == "0" -> close, else -> open.
    // This implies any character that is NOT '0' opens it?
    // Or rather "received: [message]" print.
    // Let's handle simple single char command for responsiveness.
    // If it receives a stream, we might need to be careful.
    // Assuming simple control chars for now.

    // Note: readString() waits for timeout (1s default). Terrible.
    // New logic: if we see '0', close. if we see '1', open.
    // If we just see random noise, ignore?
    // Legacy logic: if message == "0" close, else open.
    // This is aggressive. Let's replicate "if '0' close, if '1' open".

    if (c == '0') {
      lgt.close();
      Serial.print("received: [0] -> CLOSE");
    } else if (c == '1' || c == '2' || isAlphaNumeric(c)) {
      // Interpreting non-0 as Open, but filtering newlines/whitespace
      lgt.open();
      Serial.print("received: [");
      Serial.print(c);
      Serial.print("] -> OPEN");
    }
  }
}
