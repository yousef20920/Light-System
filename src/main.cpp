#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <time.h>

const char* ssid = "wifi_SSID";              // Your Wi-Fi SSID
const char* password = "wifi_password";            // Your Wi-Fi Password

const char* ifttt_webhook_occupied = "https://maker.ifttt.com/yourIFTTTURL";
const char* ifttt_webhook_empty = "https://maker.ifttt.com/yourIFTTTURL";

// Dual Ultrasonic Sensor Pins
#define TRIG_PIN_1 5   // Entrance sensor trigger
#define ECHO_PIN_1 18  // Entrance sensor echo
#define TRIG_PIN_2 4   // Exit sensor trigger  
#define ECHO_PIN_2 19  // Exit sensor echo

// Sensor variables
long duration1, duration2;
int distance1, distance2;
bool sensor1_triggered = false;
bool sensor2_triggered = false;
unsigned long sensor1_time = 0;
unsigned long sensor2_time = 0;
const int SENSOR_THRESHOLD = 75; // Distance threshold in cm
const unsigned long SEQUENCE_TIMEOUT = 3000; // 3 seconds timeout for sensor sequence

// Room state variables
bool roomOccupied = false;
bool previousState = false;
int occupantCount = 0;
unsigned long roomOccupiedSince = 0;
unsigned long totalOccupiedTime = 0;
unsigned long dailyOccupiedTime = 0;
unsigned long lastDayReset = 0;

// Analytics variables
float energySavedToday = 0.0;
float energySavedWeek = 0.0;
float energySavedMonth = 0.0;
float energySavedYear = 0.0;
const float LIGHT_POWER_WATTS = 60.0; // Assumed light power consumption
const float ENERGY_COST_PER_KWH = 0.12; // Cost per kWh in currency

// Web server
WebServer server(80);

// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
    Serial.begin(115200);
    
    // Initialize dual sensor pins
    pinMode(TRIG_PIN_1, OUTPUT);
    pinMode(ECHO_PIN_1, INPUT);
    pinMode(TRIG_PIN_2, OUTPUT);
    pinMode(ECHO_PIN_2, INPUT);

    // Initialize the LCD
    lcd.init();
    lcd.backlight();

    // Print a welcome message
    lcd.setCursor(0, 0);
    lcd.print("Smart Light Sys");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(1000);
        attempts++;
        Serial.println("Connecting to WiFi...");
        lcd.setCursor(0, 1);
        lcd.print("WiFi: " + String(attempts) + "/20    ");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Connected");
        lcd.setCursor(0, 1);
        lcd.print(WiFi.localIP());
        
        // Setup web server routes
        server.on("/", handleRoot);
        server.on("/api/status", handleAPIStatus);
        server.begin();
        Serial.println("Web server started");
        
        delay(3000);
    } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Failed");
        lcd.setCursor(0, 1);
        lcd.print("Check Config");
    }
    
    lcd.clear();
    lastDayReset = millis();
}

// Function to read distance from ultrasonic sensor
int readDistance(int trigPin, int echoPin) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2;
}

// Function to detect direction of movement
void detectMovement() {
    distance1 = readDistance(TRIG_PIN_1, ECHO_PIN_1);
    distance2 = readDistance(TRIG_PIN_2, ECHO_PIN_2);
    
    unsigned long currentTime = millis();
    
    // Check sensor 1 (entrance)
    if (distance1 < SENSOR_THRESHOLD && !sensor1_triggered) {
        sensor1_triggered = true;
        sensor1_time = currentTime;
    } else if (distance1 >= SENSOR_THRESHOLD && sensor1_triggered) {
        sensor1_triggered = false;
    }
    
    // Check sensor 2 (exit)
    if (distance2 < SENSOR_THRESHOLD && !sensor2_triggered) {
        sensor2_triggered = true;
        sensor2_time = currentTime;
    } else if (distance2 >= SENSOR_THRESHOLD && sensor2_triggered) {
        sensor2_triggered = false;
    }
    
    // Detect entry sequence: sensor1 then sensor2
    if (sensor1_triggered && sensor2_time > sensor1_time && 
        (currentTime - sensor1_time) < SEQUENCE_TIMEOUT) {
        occupantCount++;
        roomOccupied = true;
        if (!previousState) {
            roomOccupiedSince = currentTime;
        }
        Serial.println("Person entered room. Count: " + String(occupantCount));
        sensor1_triggered = false;
        sensor2_triggered = false;
    }
    
    // Detect exit sequence: sensor2 then sensor1
    if (sensor2_triggered && sensor1_time > sensor2_time && 
        (currentTime - sensor2_time) < SEQUENCE_TIMEOUT) {
        occupantCount = max(0, occupantCount - 1);
        if (occupantCount == 0) {
            roomOccupied = false;
            if (previousState && roomOccupiedSince > 0) {
                unsigned long sessionTime = currentTime - roomOccupiedSince;
                totalOccupiedTime += sessionTime;
                dailyOccupiedTime += sessionTime;
                updateEnergySavings(sessionTime);
            }
        }
        Serial.println("Person exited room. Count: " + String(occupantCount));
        sensor1_triggered = false;
        sensor2_triggered = false;
    }
    
    // Reset sensors if timeout exceeded
    if ((currentTime - sensor1_time) > SEQUENCE_TIMEOUT) {
        sensor1_triggered = false;
    }
    if ((currentTime - sensor2_time) > SEQUENCE_TIMEOUT) {
        sensor2_triggered = false;
    }
}

// Web server handlers
void handleRoot() {
    String html = R"html(
<!DOCTYPE html>
<html>
<head>
    <title>Smart Light System Dashboard</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style>
        body { font-family: Arial, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { text-align: center; color: #333; margin-bottom: 30px; }
        .card { background: white; padding: 20px; margin: 10px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }
        .status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; }
        .status-occupied { background-color: #e8f5e8; border-left: 4px solid #4caf50; }
        .status-empty { background-color: #fff3e0; border-left: 4px solid #ff9800; }
        .metrics { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }
        .metric { text-align: center; }
        .metric-value { font-size: 2em; font-weight: bold; color: #2196f3; }
        .metric-label { color: #666; margin-top: 5px; }
        .server-status { display: inline-block; width: 12px; height: 12px; border-radius: 50%; margin-right: 8px; }
        .online { background-color: #4caf50; }
        .refresh-btn { background: #2196f3; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; }
        .refresh-btn:hover { background: #1976d2; }
    </style>
    <script>
        function refreshData() {
            fetch('/api/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('room-status').textContent = data.occupied ? 'Occupied' : 'Empty';
                    document.getElementById('room-card').className = 'card ' + (data.occupied ? 'status-occupied' : 'status-empty');
                    document.getElementById('occupant-count').textContent = data.occupantCount;
                    document.getElementById('energy-today').textContent = data.energySavedToday.toFixed(3);
                    document.getElementById('energy-week').textContent = data.energySavedWeek.toFixed(3);
                    document.getElementById('energy-month').textContent = data.energySavedMonth.toFixed(3);
                    document.getElementById('energy-year').textContent = data.energySavedYear.toFixed(3);
                    document.getElementById('occupied-today').textContent = (data.dailyOccupiedTime / 3600000).toFixed(1);
                    document.getElementById('total-occupied').textContent = (data.totalOccupiedTime / 3600000).toFixed(1);
                    document.getElementById('uptime').textContent = (data.uptime / 1000).toFixed(0);
                });
        }
        
        setInterval(refreshData, 5000); // Refresh every 5 seconds
        window.onload = refreshData;
    </script>
</head>
<body>
    <div class="container">
        <h1 class="header">🏠 Smart Light System Dashboard</h1>
        
        <div class="status-grid">
            <div id="room-card" class="card status-empty">
                <h2>Room Status</h2>
                <h3 id="room-status">Empty</h3>
                <p>Current occupants: <span id="occupant-count">0</span></p>
            </div>
            
            <div class="card">
                <h2><span class="server-status online"></span>Server Status</h2>
                <p>System: Online</p>
                <p>Uptime: <span id="uptime">0</span> seconds</p>
                <p>WiFi: Connected</p>
            </div>
        </div>
        
        <div class="card">
            <h2>⚡ Energy Savings</h2>
            <div class="metrics">
                <div class="metric">
                    <div class="metric-value" id="energy-today">0.000</div>
                    <div class="metric-label">kWh Saved Today</div>
                </div>
                <div class="metric">
                    <div class="metric-value" id="energy-week">0.000</div>
                    <div class="metric-label">kWh Saved This Week</div>
                </div>
                <div class="metric">
                    <div class="metric-value" id="energy-month">0.000</div>
                    <div class="metric-label">kWh Saved This Month</div>
                </div>
                <div class="metric">
                    <div class="metric-value" id="energy-year">0.000</div>
                    <div class="metric-label">kWh Saved This Year</div>
                </div>
            </div>
        </div>
        
        <div class="card">
            <h2>⏰ Occupancy Analytics</h2>
            <div class="metrics">
                <div class="metric">
                    <div class="metric-value" id="occupied-today">0.0</div>
                    <div class="metric-label">Hours Occupied Today</div>
                </div>
                <div class="metric">
                    <div class="metric-value" id="total-occupied">0.0</div>
                    <div class="metric-label">Total Hours Occupied</div>
                </div>
            </div>
        </div>
        
        <div style="text-align: center; margin-top: 20px;">
            <button class="refresh-btn" onclick="refreshData()">🔄 Refresh Data</button>
        </div>
    </div>
</body>
</html>
)html";
    server.send(200, "text/html", html);
}

void handleAPIStatus() {
    StaticJsonDocument<512> doc;
    doc["occupied"] = roomOccupied;
    doc["occupantCount"] = occupantCount;
    doc["energySavedToday"] = energySavedToday;
    doc["energySavedWeek"] = energySavedWeek;
    doc["energySavedMonth"] = energySavedMonth;
    doc["energySavedYear"] = energySavedYear;
    doc["dailyOccupiedTime"] = dailyOccupiedTime;
    doc["totalOccupiedTime"] = totalOccupiedTime;
    doc["uptime"] = millis();
    doc["distance1"] = distance1;
    doc["distance2"] = distance2;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void sendIFTTTRequest(const char* url) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(url);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
    } else {
        Serial.println("WiFi Disconnected");
    }
}

void loop() {
    // Handle web server clients
    server.handleClient();
    
    // Detect movement with dual sensors
    detectMovement();
    
    // Reset daily statistics at midnight (24 hours)
    if (millis() - lastDayReset > 86400000) { // 24 hours in milliseconds
        dailyOccupiedTime = 0;
        energySavedToday = 0;
        lastDayReset = millis();
    }

    // Display current status on LCD
    lcd.setCursor(0, 0);
    lcd.print("D1:");
    lcd.print(distance1);
    lcd.print(" D2:");
    lcd.print(distance2);
    lcd.print("    ");

    lcd.setCursor(0, 1);
    if (roomOccupied) {
        lcd.print("Occupied (" + String(occupantCount) + ")   ");
    } else {
        lcd.print("Empty           ");
    }

    // Check for state change and update accordingly
    if (roomOccupied != previousState) {
        if (roomOccupied) {
            Serial.println("Room Occupied. Sending turn on request.");
            sendIFTTTRequest(ifttt_webhook_occupied);
        } else {
            Serial.println("Room Empty. Sending turn off request.");
            sendIFTTTRequest(ifttt_webhook_empty);
        }
        previousState = roomOccupied;
    }
    
    delay(100); // Reduced delay for more responsive sensor reading
}
