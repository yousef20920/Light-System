# Troubleshooting Guide

## Common Issues and Solutions

### WiFi Connection Problems

#### Issue: ESP32 Won't Connect to WiFi
**Symptoms**: LCD shows "WiFi Failed" or "Connecting..." indefinitely

**Solutions**:
1. **Check Credentials**:
   ```cpp
   const char* ssid = "YOUR_EXACT_WIFI_NAME";     // Case sensitive!
   const char* password = "YOUR_EXACT_PASSWORD";   // Check for special characters
   ```

2. **Verify WiFi Band**:
   - ESP32 only supports 2.4GHz WiFi
   - Disable 5GHz or use separate 2.4GHz network

3. **Signal Strength**:
   - Move ESP32 closer to router
   - Check for interference from other devices

4. **Router Settings**:
   - Ensure DHCP is enabled
   - Check MAC address filtering
   - Verify WiFi security type (WPA2/WPA3)

#### Issue: Frequent WiFi Disconnections
**Solutions**:
```cpp
// Add reconnection logic
void checkWiFiConnection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected, reconnecting...");
        WiFi.begin(ssid, password);
        int attempts = 0;
        while (WiFi.status() != WL_CONNECTED && attempts < 10) {
            delay(1000);
            attempts++;
        }
    }
}
```

### Sensor Detection Issues

#### Issue: Sensors Not Detecting Movement
**Symptoms**: Distance readings always show maximum value or don't change

**Diagnostic Steps**:
1. **Check Wiring**:
   ```
   Sensor 1: TRIG→GPIO5, ECHO→GPIO18, VCC→5V, GND→GND
   Sensor 2: TRIG→GPIO4, ECHO→GPIO19, VCC→5V, GND→GND
   ```

2. **Test Individual Sensors**:
   ```cpp
   // Add to loop() for testing
   Serial.print("Sensor 1: "); Serial.print(distance1); Serial.print(" cm");
   Serial.print(" | Sensor 2: "); Serial.print(distance2); Serial.println(" cm");
   ```

3. **Power Supply Check**:
   - Measure voltage at sensor VCC pins (should be 5V)
   - Check current draw (sensors need ~15mA each)

4. **Sensor Placement**:
   - Ensure sensors face open space
   - Remove obstacles in detection path
   - Check mounting stability

#### Issue: False Triggering or Erratic Readings
**Solutions**:
1. **Adjust Threshold**:
   ```cpp
   const int SENSOR_THRESHOLD = 100; // Increase if too sensitive
   ```

2. **Add Filtering**:
   ```cpp
   // Average multiple readings
   int readDistanceFiltered(int trigPin, int echoPin) {
       int total = 0;
       for (int i = 0; i < 5; i++) {
           total += readDistance(trigPin, echoPin);
           delay(10);
       }
       return total / 5;
   }
   ```

3. **Environmental Factors**:
   - Avoid placing near heat sources
   - Check for air currents
   - Consider temperature compensation

### LCD Display Problems

#### Issue: LCD Shows Nothing or Garbled Text
**Solutions**:
1. **Check I2C Address**:
   ```cpp
   // Try different addresses
   LiquidCrystal_I2C lcd(0x27, 16, 2); // Common alternative
   // or
   LiquidCrystal_I2C lcd(0x3F, 16, 2); // Current setting
   ```

2. **I2C Scanner Code**:
   ```cpp
   void scanI2C() {
       for (byte i = 8; i < 120; i++) {
           Wire.beginTransmission(i);
           if (Wire.endTransmission() == 0) {
               Serial.print("Found I2C device at 0x");
               Serial.println(i, HEX);
           }
       }
   }
   ```

3. **Wiring Check**:
   ```
   LCD SDA → GPIO21 (or GPIO19 on some ESP32 variants)
   LCD SCL → GPIO22 (or GPIO18 on some ESP32 variants)
   ```

#### Issue: LCD Backlight Not Working
**Solutions**:
- Check power connections (VCC to 5V)
- Verify backlight jumper on LCD module
- Test with known working LCD

### Dashboard Access Issues

#### Issue: Cannot Access Web Dashboard
**Symptoms**: Browser shows "This site can't be reached"

**Diagnostic Steps**:
1. **Find ESP32 IP Address**:
   ```cpp
   // Add to setup() after WiFi connection
   Serial.print("IP address: ");
   Serial.println(WiFi.localIP());
   ```

2. **Network Connectivity**:
   - Ping ESP32 IP from computer: `ping 192.168.1.100`
   - Ensure devices on same network
   - Check firewall settings

3. **Web Server Status**:
   ```cpp
   // Add debug logging
   server.on("/", []() {
       Serial.println("Web request received");
       handleRoot();
   });
   ```

#### Issue: Dashboard Loads but No Data Updates
**Solutions**:
1. **Check API Endpoint**:
   - Test directly: `http://[ESP32_IP]/api/status`
   - Should return JSON data

2. **Browser Console Errors**:
   - Open browser developer tools (F12)
   - Check console for JavaScript errors
   - Verify network requests in Network tab

3. **CORS Issues** (for custom domains):
   ```cpp
   server.sendHeader("Access-Control-Allow-Origin", "*");
   ```

### IFTTT Integration Problems

#### Issue: IFTTT Webhooks Not Triggering
**Diagnostic Steps**:
1. **Test Webhook URLs Manually**:
   - Copy webhook URL from code
   - Paste in browser address bar
   - Should see "Congratulations!" message

2. **Check IFTTT Applet Status**:
   - Login to IFTTT.com
   - Verify applets are enabled
   - Check activity log for triggers

3. **Network Connectivity**:
   ```cpp
   void sendIFTTTRequest(const char* url) {
       if (WiFi.status() == WL_CONNECTED) {
           HTTPClient http;
           http.begin(url);
           int httpResponseCode = http.GET();
           
           Serial.print("IFTTT Response: ");
           Serial.println(httpResponseCode);
           
           if (httpResponseCode > 0) {
               String payload = http.getString();
               Serial.println(payload);
           }
           http.end();
       }
   }
   ```

#### Issue: Smart Devices Not Responding
**Solutions**:
1. **Test Device Control**:
   - Use device's native app
   - Verify internet connectivity
   - Check device power status

2. **IFTTT Service Authorization**:
   - Re-authorize smart home service in IFTTT
   - Check service status page
   - Update device credentials if changed

### Performance Issues

#### Issue: Slow Response Times
**Solutions**:
1. **Reduce Sensor Reading Frequency**:
   ```cpp
   delay(200); // Increase from 100ms to reduce load
   ```

2. **Optimize Web Server**:
   ```cpp
   // Add connection limits
   server.setServerTimeout(5000);
   ```

3. **Memory Management**:
   ```cpp
   // Monitor free heap
   Serial.print("Free heap: ");
   Serial.println(ESP.getFreeHeap());
   ```

#### Issue: System Crashes or Reboots
**Solutions**:
1. **Power Supply**:
   - Use adequate power supply (>1A)
   - Check for voltage drops
   - Add capacitors for noise filtering

2. **Memory Leaks**:
   ```cpp
   // Check for memory leaks
   void loop() {
       static unsigned long lastCheck = 0;
       if (millis() - lastCheck > 30000) { // Every 30 seconds
           Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
           lastCheck = millis();
       }
   }
   ```

3. **Watchdog Timer**:
   ```cpp
   #include <esp_task_wdt.h>
   
   void setup() {
       esp_task_wdt_init(30, true); // 30 second timeout
       esp_task_wdt_add(NULL);
   }
   
   void loop() {
       esp_task_wdt_reset(); // Reset watchdog
       // Your code here
   }
   ```

## Advanced Debugging

### Serial Monitor Debugging
```cpp
#define DEBUG 1

void debugPrint(String message) {
    #if DEBUG
    Serial.println("[DEBUG] " + message);
    #endif
}

// Usage
debugPrint("Sensor 1 triggered");
```

### Network Debugging
```cpp
void printWiFiStatus() {
    Serial.println("=== WiFi Status ===");
    Serial.print("SSID: "); Serial.println(WiFi.SSID());
    Serial.print("Signal strength (RSSI): "); Serial.print(WiFi.RSSI()); Serial.println(" dBm");
    Serial.print("IP address: "); Serial.println(WiFi.localIP());
    Serial.print("MAC address: "); Serial.println(WiFi.macAddress());
    Serial.println("==================");
}
```

### Memory Debugging
```cpp
void printMemoryStats() {
    Serial.println("=== Memory Stats ===");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Largest free block: %d bytes\n", ESP.getMaxAllocHeap());
    Serial.printf("Flash size: %d bytes\n", ESP.getFlashChipSize());
    Serial.println("===================");
}
```

## Hardware Testing Tools

### Multimeter Tests
1. **Power Supply**: Measure 5V and 3.3V rails
2. **Continuity**: Check all connections
3. **Signal Testing**: Verify TRIG/ECHO signals

### Oscilloscope Analysis
- Monitor ultrasonic sensor signals
- Check timing of TRIG pulses
- Verify ECHO signal integrity

### Logic Analyzer
- Capture I2C communication to LCD
- Analyze sensor timing sequences
- Debug communication protocols

## Getting Help

### Information to Collect
When seeking help, provide:
1. **Hardware Setup**: Wiring diagram and component list
2. **Software Version**: Code version and library versions
3. **Error Messages**: Complete error logs from serial monitor
4. **Network Details**: Router model, WiFi settings
5. **Environment**: Physical installation details

### Useful Commands for Diagnostics
```cpp
// System information
Serial.printf("ESP32 Chip model: %s\n", ESP.getChipModel());
Serial.printf("Chip revision: %d\n", ESP.getChipRevision());
Serial.printf("CPU frequency: %d MHz\n", ESP.getCpuFreqMHz());
Serial.printf("Flash size: %d bytes\n", ESP.getFlashChipSize());
Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
```

### Community Resources
- **ESP32 Forum**: Official Espressif community
- **Arduino Forum**: General Arduino/ESP32 help
- **Reddit**: r/esp32, r/homeautomation
- **GitHub Issues**: Project-specific problems