## **README for Room Occupancy Light System**

### **Overview**
The Room Occupancy Light System automates lighting control using an ultrasonic sensor and ESP32 microcontroller. It detects room occupancy in real time and integrates with smart home systems via IFTTT webhooks, saving energy and enhancing convenience.

---

### **Features**
- Real-time occupancy detection.
- Automatic light control using IFTTT webhooks.
- Wi-Fi connectivity for smart home integration.
- Live status updates on an LCD display.

---

### **Hardware Components**
- ESP32 Microcontroller
- Ultrasonic Sensor (HC-SR04)
- 16x2 LiquidCrystal_I2C Display
- Jumper wires and breadboard

---

### **Setup**
1. **Hardware**:
   - Connect the ultrasonic sensor to ESP32 pins (TRIG: GPIO5, ECHO: GPIO18).
   - Attach the LCD to the ESP32 I2C pins.
2. **Software**:
   - Install libraries: `LiquidCrystal_I2C`, `WiFi`, `HTTPClient`.
   - Update the code with your Wi-Fi and IFTTT webhook details.
3. **Upload**:
   - Flash the code onto the ESP32 using Arduino IDE.

---

### **How It Works**
- The ultrasonic sensor detects distance, determining occupancy.
- State changes (occupied/unoccupied) trigger HTTP requests to IFTTT webhooks.
- The LCD displays the distance and occupancy status.

---

### **Future Improvements**
- Add PIR sensors for better accuracy.
- Enable power-saving modes.
- Log occupancy data for insights.

---

This system is simple yet effective for energy-efficient automation.
