## **Smart Light System with Dual Sensors & Analytics Dashboard**

### **Overview**
The Smart Light System is an advanced home automation solution that uses dual ultrasonic sensors to detect room entry/exit patterns and provides comprehensive analytics through a web dashboard. The system accurately tracks occupancy, calculates energy savings, and integrates with smart home platforms including Alexa via IFTTT.

---

### **🔥 Key Features**
- **Dual Sensor Entry/Exit Detection**: Accurately determines if someone is entering or leaving the room
- **Real-time Web Dashboard**: Beautiful analytics interface showing occupancy data and energy savings
- **Energy Analytics**: Track savings by day, week, month, and year
- **Occupancy Analytics**: Monitor room usage patterns and duration
- **Smart Home Integration**: Works with Alexa, Google Home, and other IFTTT-compatible devices
- **Live Status Display**: 16x2 LCD shows real-time sensor data and room status
- **WiFi Connectivity**: Remote monitoring and control capabilities

---

### **🛠️ Hardware Requirements**

#### **Essential Components**
- **ESP32 Development Board** (1x) - Main microcontroller
- **HC-SR04 Ultrasonic Sensors** (2x) - For entry/exit detection
- **16x2 I2C LCD Display** (1x) - Local status display
- **Breadboard or PCB** (1x) - For connections
- **Jumper Wires** (Male-to-Male, Male-to-Female) - For connections
- **Micro USB Cable** (1x) - For programming and power
- **5V Power Supply** (Optional) - For standalone operation

#### **Connection Diagram**
```
ESP32 Pin    Component           Notes
GPIO5    →   Sensor 1 TRIG      (Entrance sensor)
GPIO18   →   Sensor 1 ECHO      (Entrance sensor)
GPIO4    →   Sensor 2 TRIG      (Exit sensor)
GPIO19   →   Sensor 2 ECHO      (Exit sensor)
GPIO21   →   LCD SDA            (I2C Data)
GPIO22   →   LCD SCL            (I2C Clock)
5V       →   Sensors VCC & LCD VCC
GND      →   Sensors GND & LCD GND
```

#### **Shopping List with Links**
| Item | Quantity | Estimated Cost | Purpose |
|------|----------|----------------|---------|
| ESP32 DevKit | 1 | $8-12 | Main controller |
| HC-SR04 Ultrasonic Sensor | 2 | $3-5 each | Distance measurement |
| 16x2 I2C LCD | 1 | $5-8 | Local display |
| Breadboard | 1 | $3-5 | Prototyping |
| Jumper Wires Kit | 1 | $5-8 | Connections |
| **Total** | | **$32-48** | Complete system |

---

### **💻 Software Installation**

#### **1. Development Environment Setup**
```bash
# Install Python (if not already installed)
# Download from: https://python.org

# Install PlatformIO
pip install platformio

# Or use Arduino IDE with ESP32 support
# Download from: https://arduino.cc/downloads
```

#### **2. Required Libraries** (Auto-installed with PlatformIO)
- ArduinoJson (v7.1.0+)
- LiquidCrystal_I2C
- WiFi (ESP32 built-in)
- WebServer (ESP32 built-in)
- HTTPClient (ESP32 built-in)

#### **3. Project Setup**
```bash
# Clone the repository
git clone https://github.com/yousef20920/Light-System.git
cd Light-System

# Build the project
pio run

# Upload to ESP32 (connect via USB)
pio run --target upload

# Monitor serial output
pio device monitor
```

---

### **⚙️ Configuration**

#### **1. WiFi Setup**
Edit `src/main.cpp` and update these lines:
```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

#### **2. IFTTT Integration**
1. Create account at [IFTTT.com](https://ifttt.com)
2. Create two applets:
   - **Room Occupied**: Webhooks → Smart Device On
   - **Room Empty**: Webhooks → Smart Device Off
3. Get webhook URLs and update in code:
```cpp
const char* ifttt_webhook_occupied = "https://maker.ifttt.com/trigger/room_occupied/with/key/YOUR_KEY";
const char* ifttt_webhook_empty = "https://maker.ifttt.com/trigger/room_empty/with/key/YOUR_KEY";
```

#### **3. Alexa Integration via IFTTT**
1. In IFTTT, connect Webhooks to Amazon Alexa
2. Set up voice commands:
   - "Alexa, turn on room lights" → Triggers when room occupied
   - "Alexa, turn off room lights" → Triggers when room empty

---

### **🚀 Deployment & Internet Access**

#### **Option 1: Local Network Access**
- Access dashboard via ESP32's IP address (shown on LCD)
- Example: `http://192.168.1.100`

#### **Option 2: Internet Deployment via ngrok**
```bash
# Install ngrok
# Download from: https://ngrok.com

# Create tunnel to ESP32
ngrok http 192.168.1.100:80

# Use provided ngrok URL for remote access
```

#### **Option 3: Cloud Deployment (Advanced)**
- Use services like Blynk, ThingSpeak, or AWS IoT
- Forward data to cloud dashboard for global access

---

### **📊 Dashboard Features**

#### **Real-time Metrics**
- **Room Status**: Current occupancy state and count
- **Sensor Data**: Live distance readings from both sensors
- **Server Health**: System uptime and connectivity status

#### **Energy Analytics**
- Daily, weekly, monthly, and yearly energy savings
- Estimated cost savings based on light power consumption
- Visual indicators and trends

#### **Occupancy Analytics**
- Total time room occupied today
- Historical occupancy patterns
- Peak usage hours and trends

#### **Access the Dashboard**
1. Connect ESP32 to power and WiFi
2. Note IP address shown on LCD
3. Open web browser and navigate to: `http://[ESP32_IP_ADDRESS]`
4. Dashboard auto-refreshes every 5 seconds

---

### **🔧 How It Works**

#### **Dual Sensor Logic**
1. **Sensor 1** (Entrance): Positioned at room entrance
2. **Sensor 2** (Exit): Positioned inside room, near entrance
3. **Entry Detection**: Person triggers Sensor 1, then Sensor 2
4. **Exit Detection**: Person triggers Sensor 2, then Sensor 1
5. **Occupant Counting**: System maintains accurate count of people in room

#### **Smart Features**
- **Automatic Light Control**: Lights turn on/off based on actual occupancy
- **Energy Calculation**: Tracks power savings from automated control
- **False Trigger Prevention**: Timeout logic prevents sensor noise issues
- **Web Interface**: Real-time dashboard for monitoring and analytics

---

### **🏠 Smart Home Integration**

#### **IFTTT Webhooks**
- Room occupied → Trigger smart lights ON
- Room empty → Trigger smart lights OFF
- Integration with 600+ IFTTT services

#### **Alexa Commands** (via IFTTT)
- "Alexa, turn on room lights"
- "Alexa, turn off room lights" 
- "Alexa, what's the room status?"

#### **Expandability**
- Add more sensors for multiple rooms
- Integrate with smart thermostats
- Connect to security systems
- Add mobile app notifications

---

### **📈 Analytics & Insights**

The system provides valuable insights into:
- **Energy Efficiency**: How much electricity you're saving
- **Room Usage Patterns**: When and how long the room is occupied
- **Cost Savings**: Monetary value of automated lighting control
- **System Reliability**: Uptime and connectivity metrics

---

### **🛡️ Troubleshooting**

#### **Common Issues**
1. **WiFi Connection Failed**
   - Check SSID and password in code
   - Ensure 2.4GHz WiFi (ESP32 doesn't support 5GHz)

2. **Sensors Not Detecting**
   - Verify wiring connections
   - Check sensor placement (avoid obstacles)
   - Adjust `SENSOR_THRESHOLD` value in code

3. **Dashboard Not Loading**
   - Check ESP32 IP address on LCD
   - Ensure devices on same network
   - Restart ESP32 if needed

4. **IFTTT Not Triggering**
   - Verify webhook URLs in code
   - Test webhooks directly in browser
   - Check IFTTT applet configuration

---

### **🔮 Future Enhancements**
- Mobile app for iOS/Android
- Machine learning for occupancy prediction
- Integration with additional smart home platforms
- Historical data export and advanced analytics
- Multiple room support with centralized dashboard

---

### **📄 License**
This project is open-source and available under the MIT License.

### **🤝 Contributing**
Pull requests are welcome! For major changes, please open an issue first to discuss what you would like to change.

---

**Enjoy your automated smart lighting system! 💡✨**
