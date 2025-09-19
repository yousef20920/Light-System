# Deployment and Internet Access Guide

## Local Network Access

### Finding ESP32 IP Address
1. Check LCD display - IP shown on boot
2. Use router admin panel to find device
3. Use network scanner tools (e.g., Fing app)
4. Check serial monitor during ESP32 boot

### Accessing Dashboard Locally
1. Connect device to same WiFi network as ESP32
2. Open web browser
3. Navigate to: `http://[ESP32_IP_ADDRESS]`
4. Example: `http://192.168.1.100`

## Internet Deployment Options

### Option 1: Port Forwarding (Simple)

#### Router Configuration
1. Access router admin panel (usually 192.168.1.1)
2. Find "Port Forwarding" or "Virtual Server" settings
3. Add new rule:
   - **Internal IP**: ESP32 IP address
   - **Internal Port**: 80
   - **External Port**: 8080 (or any available port)
   - **Protocol**: TCP
4. Save settings and reboot router

#### Security Considerations
- **⚠️ Warning**: Exposes device to internet
- Use strong WiFi passwords
- Consider changing default ESP32 port
- Monitor access logs regularly

#### Access from Internet
- Find your public IP: [whatismyipaddress.com](https://whatismyipaddress.com)
- Access via: `http://[PUBLIC_IP]:[EXTERNAL_PORT]`
- Example: `http://123.456.789.012:8080`

### Option 2: ngrok (Recommended for Testing)

#### Installation
```bash
# Download ngrok from https://ngrok.com
# Sign up for free account
# Install ngrok on computer connected to same network as ESP32

# Windows
choco install ngrok

# macOS  
brew install ngrok

# Linux
sudo snap install ngrok
```

#### Setup and Usage
```bash
# Authenticate ngrok
ngrok authtoken YOUR_AUTH_TOKEN

# Create tunnel to ESP32
ngrok http 192.168.1.100:80

# Use provided ngrok URL for internet access
# Example: https://abc123.ngrok.io
```

#### Benefits
- ✅ Secure HTTPS tunnel
- ✅ No router configuration needed
- ✅ Easy to start/stop
- ✅ Free tier available

#### Limitations
- ⚠️ URL changes each restart (free tier)
- ⚠️ Limited bandwidth
- ⚠️ Requires computer running ngrok

### Option 3: Cloud Platforms

#### Blynk IoT Platform
```cpp
// Add to ESP32 code
#include <BlynkSimpleEsp32.h>

char auth[] = "YOUR_BLYNK_TOKEN";
char ssid[] = "YOUR_WIFI_SSID";
char pass[] = "YOUR_WIFI_PASSWORD";

void setup() {
    Blynk.begin(auth, ssid, pass);
    // Your existing setup code
}

void loop() {
    Blynk.run();
    // Your existing loop code
}
```

#### ThingSpeak Integration
```cpp
// Send data to ThingSpeak
#include <ThingSpeak.h>

unsigned long channelNumber = YOUR_CHANNEL_NUMBER;
const char* writeAPIKey = "YOUR_WRITE_API_KEY";

void sendToThingSpeak() {
    ThingSpeak.setField(1, roomOccupied ? 1 : 0);
    ThingSpeak.setField(2, occupantCount);
    ThingSpeak.setField(3, energySavedToday);
    ThingSpeak.writeFields(channelNumber, writeAPIKey);
}
```

### Option 4: VPN Access

#### Setting Up VPN Server
1. **OpenVPN**: Install on router or dedicated device
2. **WireGuard**: Modern, fast VPN solution
3. **Commercial VPN**: Use services like NordVPN, ExpressVPN

#### Benefits
- ✅ Secure encrypted access
- ✅ Access all local devices
- ✅ No port forwarding needed
- ✅ Mobile app support

## Mobile Access

### Progressive Web App (PWA)
The dashboard is designed as a PWA for mobile devices:

1. **Add to Home Screen**:
   - iOS: Safari → Share → Add to Home Screen
   - Android: Chrome → Menu → Add to Home Screen

2. **Offline Capability**: Basic caching for improved performance

3. **Mobile Optimized**: Responsive design for all screen sizes

### QR Code Access
Generate QR code for easy sharing:
```
Content: http://[ESP32_IP_OR_URL]
```
Use online QR generators or create custom QR in dashboard.

## Domain Name Setup

### Free Domain Options
1. **No-IP**: Dynamic DNS service
2. **DuckDNS**: Free subdomain service  
3. **FreeDNS**: Free DNS hosting

### Custom Domain Configuration
```bash
# Example with No-IP
1. Register account at no-ip.com
2. Create hostname: mysmartlights.ddns.net
3. Configure router to update IP automatically
4. Point domain to your public IP
```

## SSL/HTTPS Setup

### Let's Encrypt with Reverse Proxy
```nginx
# Nginx configuration
server {
    listen 443 ssl;
    server_name mysmartlights.example.com;
    
    ssl_certificate /path/to/cert.pem;
    ssl_certificate_key /path/to/key.pem;
    
    location / {
        proxy_pass http://192.168.1.100:80;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

## Performance Optimization

### Caching Strategy
```cpp
// Add cache headers in ESP32 code
server.sendHeader("Cache-Control", "public, max-age=300");
server.sendHeader("ETag", "\"v1.0\"");
```

### Bandwidth Optimization
- Minimize JSON payload size
- Compress static assets
- Use CDN for external libraries
- Implement data pagination for history

## Monitoring and Analytics

### Google Analytics Integration
```html
<!-- Add to dashboard HTML -->
<script async src="https://www.googletagmanager.com/gtag/js?id=GA_TRACKING_ID"></script>
<script>
  window.dataLayer = window.dataLayer || [];
  function gtag(){dataLayer.push(arguments);}
  gtag('js', new Date());
  gtag('config', 'GA_TRACKING_ID');
</script>
```

### Uptime Monitoring
1. **UptimeRobot**: Free monitoring service
2. **Pingdom**: Professional monitoring
3. **StatusCake**: Basic uptime checks

## Backup and Recovery

### Configuration Backup
```cpp
// Save settings to SPIFFS
void saveConfig() {
    File file = SPIFFS.open("/config.json", "w");
    StaticJsonDocument<512> doc;
    doc["ssid"] = ssid;
    doc["energySavedTotal"] = energySavedYear;
    serializeJson(doc, file);
    file.close();
}
```

### Automatic Updates
```cpp
// OTA update capability
#include <ArduinoOTA.h>

void setupOTA() {
    ArduinoOTA.setHostname("smart-light-system");
    ArduinoOTA.setPassword("admin");
    ArduinoOTA.begin();
}
```

## Security Best Practices

### Network Security
- Use WPA3 encryption on WiFi
- Regular firmware updates
- Separate IoT network segment
- Monitor network traffic

### Application Security
- Input validation on all endpoints
- Rate limiting for API calls
- Authentication for admin functions
- Regular security audits

### Data Privacy
- Local data storage when possible
- Encrypted communication channels
- User consent for data collection
- Data retention policies