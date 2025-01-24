#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid = "wifi_SSID";              // Your Wi-Fi SSID
const char* password = "wifi_password";            // Your Wi-Fi Password

const char* ifttt_webhook_occupied = "https://maker.ifttt.com/yourIFTTTURL";
const char* ifttt_webhook_empty = "https://maker.ifttt.com/yourIFTTTURL";

#define TRIG_PIN 5  // Define the pin for Trig
#define ECHO_PIN 18 // Define the pin for Echo

long duration;
int distance;
bool roomOccupied = false;
bool previousState = false; // Track the previous state to detect changes

// Set the LCD address to 0x3F for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() {
    Serial.begin(115200);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);

    // Initialize the LCD
    lcd.init();
    lcd.backlight();

    // Print a welcome message
    lcd.setCursor(0, 0);
    lcd.print("Initializing...");
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
        lcd.setCursor(0, 1);
        lcd.print("Connecting...");
    }
    Serial.println("Connected to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    delay(2000);
    lcd.clear();
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
    // Send a 10us pulse to trigger the ultrasonic sensor
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Read the time it takes for the echo to return
    duration = pulseIn(ECHO_PIN, HIGH);
    distance = duration * 0.034 / 2;

    // Display the distance on the LCD
    lcd.setCursor(0, 0);
    lcd.print("Distance: ");
    lcd.print(distance);
    lcd.print(" cm    ");  // Adding spaces to clear any leftover characters

    // Determine if the room is occupied based on the distance
    if (distance < 75) {  // Adjust the distance threshold as needed
        if (!roomOccupied) {
            roomOccupied = true;
        } else {
            roomOccupied = false;
        }
        delay(500);
    }

    // Check for state change and update accordingly
    if (roomOccupied != previousState) {
        if (roomOccupied) {
            Serial.println("Room Occupied. Sending turn on request.");
            lcd.setCursor(0, 1);
            lcd.print("Room: Occupied   ");  // Adding spaces to clear any leftover characters
            sendIFTTTRequest(ifttt_webhook_occupied);
        } else {
            Serial.println("Room Empty. Sending turn off request.");
            lcd.setCursor(0, 1);
            lcd.print("Room: Empty      ");  // Adding spaces to clear any leftover characters
            sendIFTTTRequest(ifttt_webhook_empty);
        }
        previousState = roomOccupied; // Update the previous state
    }
    delay(500);
}
