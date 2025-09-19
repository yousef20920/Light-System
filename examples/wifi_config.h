// WiFi Configuration Template
// Copy this content to the top of your main.cpp file and update with your details

// =============================================================================
// WiFi CONFIGURATION - UPDATE THESE VALUES
// =============================================================================

// Your WiFi Network Name (SSID) - Must be 2.4GHz network
const char* ssid = "YOUR_WIFI_NETWORK_NAME";

// Your WiFi Password
const char* password = "YOUR_WIFI_PASSWORD";

// Optional: Static IP Configuration (comment out for DHCP)
/*
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);
*/

// =============================================================================
// NETWORK SETTINGS
// =============================================================================

// Web server port (80 is standard for HTTP)
const int webServerPort = 80;

// Connection timeout in seconds
const int wifiTimeoutSeconds = 30;

// Maximum connection retry attempts
const int maxRetryAttempts = 20;

// =============================================================================
// EXAMPLE CONFIGURATIONS FOR POPULAR NETWORKS
// =============================================================================

/* 
// Example 1: Home WiFi
const char* ssid = "MyHomeWiFi";
const char* password = "MySecurePassword123";

// Example 2: Mobile Hotspot
const char* ssid = "iPhone";
const char* password = "hotspotpass";

// Example 3: University/Corporate WiFi (WPA2-Enterprise)
// Note: ESP32 supports WPA2-Enterprise but requires additional configuration
const char* ssid = "UniversityWiFi";
const char* username = "student123";
const char* password = "studentpass";
*/

// =============================================================================
// TROUBLESHOOTING NOTES
// =============================================================================

/*
Common WiFi Issues:
1. SSID not found: Ensure 2.4GHz band is enabled on router
2. Authentication failed: Double-check password case sensitivity
3. Connection drops: Move ESP32 closer to router
4. Cannot access dashboard: Check firewall settings

Network Requirements:
- 2.4GHz WiFi band (ESP32 doesn't support 5GHz)
- WPA2 or WPA3 security (WEP not recommended)
- DHCP enabled (unless using static IP)
- Firewall allowing port 80 traffic
*/