# Health Monitor (Arduino)

A non-blocking, efficient health and environment monitor based on Arduino.

## Features
- **Real-time Monitoring**:
    - Temperature & Humidity (DHT11)
    - Sound/Breathing Activity
- **Interaction**:
    - Local Button Control (Activate/Cancel)
    - Remote Serial Control
    - Status Indication (Light/Relay)
- **Architecture**:
    - Non-blocking Loop (millis-based)
    - Zero Dynamic Allocation (No String/malloc)
    - Structured JSON Reporting

## Hardware Pinout
| Component | Pin | Note |
|-----------|-----|------|
| DHT11 | D2 | Temp/Humidity Sensor |
| Sound | D3 | Sound/Microphone Module |
| Light | D4 | LED or Relay Module |
| Button (Cancel) | D5 | Press to Close Light |
| Button (Activate) | D6 | Press to Open Light |
| RX (WiFi) | D7 | SoftwareSerial RX |
| TX (WiFi) | D9 | SoftwareSerial TX |

## Data Protocol
The device sends a JSON packet to `Serial2` every 3 seconds:
```json
{
    "breathing": 12,
    "humidity": 45.00,
    "temperature": 24.50,
    "deviceCode": "...",
    "alert": 0
}
```

## Compilation
1. Open `main.ino` in Arduino IDE.
2. Ensure you have the standard `DHT` library installed (or use the included one if present).
3. Select your board (e.g., Arduino Uno/Nano).
4. Verify and Upload.
