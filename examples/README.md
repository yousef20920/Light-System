# Example Configuration

This directory contains example configuration files to help you get started quickly.

## Quick Setup Checklist

### 1. Hardware Assembly
- [ ] Connect Sensor 1 (TRIG: GPIO5, ECHO: GPIO18)
- [ ] Connect Sensor 2 (TRIG: GPIO4, ECHO: GPIO19) 
- [ ] Connect LCD (SDA: GPIO21, SCL: GPIO22)
- [ ] Verify all power connections (5V, GND)

### 2. Software Configuration
- [ ] Update WiFi credentials in main.cpp
- [ ] Set up IFTTT webhooks
- [ ] Configure smart home integration
- [ ] Upload code to ESP32

### 3. Testing
- [ ] Verify sensor readings on LCD
- [ ] Test web dashboard access
- [ ] Confirm IFTTT triggers work
- [ ] Test entry/exit detection logic

### 4. Deployment
- [ ] Mount sensors in final position
- [ ] Configure internet access (optional)
- [ ] Set up monitoring/alerts
- [ ] Document your specific setup

## Configuration Templates

See the files in this directory for ready-to-use configuration examples:
- `wifi_config.h` - WiFi and network settings
- `ifttt_config.h` - IFTTT webhook configuration
- `sensor_config.h` - Sensor calibration settings
- `energy_config.h` - Energy calculation parameters