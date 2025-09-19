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
        server.on("/manifest.json", handleManifest);
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

void handleRoot() {
    String html = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
    <title>Smart Light System Dashboard</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <meta name="theme-color" content="#2196f3">
    <meta name="description" content="Smart lighting system with occupancy detection and energy analytics">
    <link rel="manifest" href="/manifest.json">
    <style>
        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Oxygen, Ubuntu, Cantarell, sans-serif; margin: 0; padding: 20px; background-color: #f5f5f5; line-height: 1.6; }
        .container { max-width: 1200px; margin: 0 auto; }
        .header { text-align: center; color: #333; margin-bottom: 30px; }
        .header h1 { margin: 0; font-size: 2.5em; font-weight: 300; }
        .card { background: white; padding: 20px; margin: 10px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); transition: transform 0.2s; }
        .card:hover { transform: translateY(-2px); }
        .status-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(280px, 1fr)); gap: 20px; margin-bottom: 20px; }
        .status-occupied { background: linear-gradient(135deg, #e8f5e8, #f1f9f1); border-left: 4px solid #4caf50; }
        .status-empty { background: linear-gradient(135deg, #fff3e0, #fef7f0); border-left: 4px solid #ff9800; }
        .metrics { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }
        .metric { text-align: center; padding: 10px; }
        .metric-value { font-size: 2.2em; font-weight: 600; color: #2196f3; margin-bottom: 5px; }
        .metric-label { color: #666; font-size: 0.9em; text-transform: uppercase; letter-spacing: 0.5px; }
        .server-status { display: inline-block; width: 12px; height: 12px; border-radius: 50%; margin-right: 8px; animation: pulse 2s infinite; }
        .online { background-color: #4caf50; }
        @keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.5; } 100% { opacity: 1; } }
        .refresh-btn { background: linear-gradient(135deg, #2196f3, #1976d2); color: white; border: none; padding: 12px 24px; border-radius: 8px; cursor: pointer; font-size: 16px; transition: all 0.3s; box-shadow: 0 2px 4px rgba(33,150,243,0.3); }
        .refresh-btn:hover { transform: translateY(-1px); box-shadow: 0 4px 8px rgba(33,150,243,0.4); }
        .refresh-btn:active { transform: translateY(0); }
        .card h2 { margin-top: 0; color: #333; font-weight: 500; }
        .install-banner { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 15px; border-radius: 8px; margin-bottom: 20px; text-align: center; display: none; }
        .install-btn { background: rgba(255,255,255,0.2); border: 1px solid rgba(255,255,255,0.3); color: white; padding: 8px 16px; border-radius: 6px; cursor: pointer; margin-left: 10px; }
        
        @media (max-width: 768px) {
            body { padding: 10px; }
            .header h1 { font-size: 2em; }
            .metrics { grid-template-columns: repeat(auto-fit, minmax(150px, 1fr)); }
            .metric-value { font-size: 1.8em; }
            .card { padding: 15px; margin: 5px; }
        }
    </style>
    <script>
        let deferredPrompt;
        
        window.addEventListener('beforeinstallprompt', (e) => {
            e.preventDefault();
            deferredPrompt = e;
            document.getElementById('install-banner').style.display = 'block';
        });
        
        function installApp() {
            if (deferredPrompt) {
                deferredPrompt.prompt();
                deferredPrompt.userChoice.then((choiceResult) => {
                    if (choiceResult.outcome === 'accepted') {
                        document.getElementById('install-banner').style.display = 'none';
                    }
                    deferredPrompt = null;
                });
            }
        }
        
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
                    document.getElementById('distance1').textContent = data.distance1;
                    document.getElementById('distance2').textContent = data.distance2;
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                    document.getElementById('room-status').textContent = 'Connection Error';
                });
        }
        
        setInterval(refreshData, 5000);
        window.onload = refreshData;
        
        // Service worker registration
        if ('serviceWorker' in navigator) {
            navigator.serviceWorker.register('/sw.js').catch(console.error);
        }
    </script>
</head>
<body>
    <div class="container">
        <div id="install-banner" class="install-banner">
            📱 Install this app for the best experience!
            <button class="install-btn" onclick="installApp()">Install</button>
        </div>
        
        <div class="header">
            <h1>🏠 Smart Light System</h1>
            <p>Intelligent occupancy detection and energy monitoring</p>
        </div>
        
        <div class="status-grid">
            <div id="room-card" class="card status-empty">
                <h2>🚪 Room Status</h2>
                <h3 id="room-status">Empty</h3>
                <p>Current occupants: <span id="occupant-count">0</span></p>
                <p style="font-size: 0.9em; color: #666;">
                    Sensor 1: <span id="distance1">--</span>cm | 
                    Sensor 2: <span id="distance2">--</span>cm
                </p>
            </div>
            
            <div class="card">
                <h2><span class="server-status online"></span>🖥️ System Status</h2>
                <p><strong>Status:</strong> Online & Monitoring</p>
                <p><strong>Uptime:</strong> <span id="uptime">0</span> seconds</p>
                <p><strong>WiFi:</strong> Connected</p>
                <p><strong>Sensors:</strong> Dual HC-SR04 Active</p>
            </div>
        </div>
        
        <div class="card">
            <h2>⚡ Energy Savings Analytics</h2>
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
        
        <div style="text-align: center; margin-top: 30px;">
            <button class="refresh-btn" onclick="refreshData()">🔄 Refresh Data</button>
        </div>
        
        <div style="text-align: center; margin-top: 20px; color: #666; font-size: 0.9em;">
            Smart Light System v2.0 | <a href="https://github.com/yousef20920/Light-System" style="color: #2196f3;">GitHub</a>
        </div>
    </div>
</body>
</html>
)html";
    server.send(200, "text/html", html);
}

void handleManifest() {
    String manifest = R"json({
  "name": "Smart Light System Dashboard",
  "short_name": "SmartLights",
  "description": "Monitor and control your smart lighting system with occupancy detection and energy analytics",
  "start_url": "/",
  "display": "standalone",
  "background_color": "#f5f5f5",
  "theme_color": "#2196f3",
  "orientation": "portrait-primary",
  "icons": [
    {
      "src": "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTkyIiBoZWlnaHQ9IjE5MiIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSIjMjE5NmYzIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPjxwYXRoIGQ9Ik05IDIxYzAgLjU1LjQ1IDEgMSAxaDRjLjU1IDAgMS0uNDUgMS0xdi0xSDl2MXptMy0xOUMyIDIgMiA2IDIgNmMwIDIuNjEgMS42NyA0Ljg2IDQgNS43NFYxN2MwIC41NS40NSAxIDEgMWg2Yy41NSAwIDEtLjQ1IDEtMXYtNS4yNmMyLjMzLS44OCA0LTMuMTMgNC01Ljc0IDAtNC00LTQtNC00czQgMCA0IDRjMCAyLjYxLTEuNjcgNC44Ni00IDUuNzRWMTdIMTFWNy43NEM4LjY3IDYuODYgNyA0LjYxIDcgMiA3LjAxIDIgOSAyIDkgMnoiLz48L3N2Zz4=",
      "sizes": "192x192",
      "type": "image/svg+xml"
    },
    {
      "src": "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNTEyIiBoZWlnaHQ9IjUxMiIgdmlld0JveD0iMCAwIDI0IDI0IiBmaWxsPSIjMjE5NmYzIiB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciPjxwYXRoIGQ9Ik05IDIxYzAgLjU1LjQ1IDEgMSAxaDRjLjU1IDAgMS0uNDUgMS0xdi0xSDl2MXptMy0xOUMyIDIgMiA2IDIgNmMwIDIuNjEgMS42NyA0Ljg2IDQgNS43NFYxN2MwIC41NS40NSAxIDEgMWg2Yy41NSAwIDEtLjQ1IDEtMXYtNS4yNmMyLjMzLS44OCA0LTMuMTMgNC01Ljc0IDAtNC00LTQtNC00czQgMCA0IDRjMCAyLjYxLTEuNjcgNC44Ni00IDUuNzRWMTdIMTFWNy43NEM4LjY3IDYuODYgNyA0LjYxIDcgMiA3LjAxIDIgOSAyIDkgMnoiLz48L3N2Zz4=",
      "sizes": "512x512",
      "type": "image/svg+xml"
    }
  ]
})json";
    server.send(200, "application/json", manifest);
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
