# IFTTT and Alexa Integration Guide

## IFTTT Setup

### Creating IFTTT Account
1. Go to [IFTTT.com](https://ifttt.com)
2. Sign up for a free account
3. Navigate to "My Applets" section

### Setting Up Webhooks

#### Step 1: Enable Webhooks Service
1. Go to [ifttt.com/maker_webhooks](https://ifttt.com/maker_webhooks)
2. Click "Connect" to enable the service
3. Click "Documentation" to get your webhook key

#### Step 2: Create Room Occupied Applet
1. Click "Create" to make new applet
2. Click "If This" → Search "Webhooks" → "Receive a web request"
3. Enter event name: `room_occupied`
4. Click "Then That" → Choose your smart device service (e.g., "Smart Life", "TP-Link", "Philips Hue")
5. Configure action to turn lights ON
6. Click "Create action" and "Finish"

#### Step 3: Create Room Empty Applet  
1. Create another applet
2. "If This" → Webhooks → "Receive a web request"
3. Event name: `room_empty`
4. "Then That" → Same smart device service
5. Configure action to turn lights OFF
6. Save the applet

#### Step 4: Get Webhook URLs
Your webhook URLs will be:
```
Occupied: https://maker.ifttt.com/trigger/room_occupied/with/key/YOUR_WEBHOOK_KEY
Empty: https://maker.ifttt.com/trigger/room_empty/with/key/YOUR_WEBHOOK_KEY
```

### Testing Webhooks
Test by pasting URLs in browser - should trigger your smart devices.

## Alexa Integration

### Method 1: Direct IFTTT-Alexa Connection

#### Setup Voice Commands
1. In IFTTT, create new applet
2. "If This" → Amazon Alexa → "Say a specific phrase"
3. Example phrases:
   - "Turn on room lights"
   - "Turn off room lights"
   - "Check room status"
4. "Then That" → Webhooks → "Make a web request"
5. Use your webhook URLs from above

### Method 2: Smart Home Device Integration

#### Using Smart Plugs/Switches
1. Connect smart devices to Alexa app
2. Use IFTTT to control these devices
3. Voice commands work through device names:
   - "Alexa, turn on bedroom lights"
   - "Alexa, turn off living room lights"

### Method 3: Custom Alexa Skill (Advanced)
Create custom skill for more advanced voice interactions.

## Supported Smart Home Platforms

### Popular IFTTT-Compatible Services
- **Philips Hue**: Smart bulbs and lighting systems
- **TP-Link Kasa**: Smart switches and plugs  
- **Smart Life/Tuya**: Generic smart home devices
- **Samsung SmartThings**: Hub-based automation
- **Google Assistant**: Voice control integration
- **Apple HomeKit**: iOS/macOS integration

### Configuration Examples

#### Philips Hue
1. IFTTT → Philips Hue service
2. Action: "Turn lights on/off"
3. Select specific lights or rooms
4. Set brightness and color (optional)

#### TP-Link Kasa
1. IFTTT → TP-Link Kasa service  
2. Action: "Turn device on/off"
3. Select specific smart plugs/switches
4. Works with any connected device

## Advanced Integration

### Multiple Room Support
1. Create separate webhook events for each room:
   - `bedroom_occupied` / `bedroom_empty`
   - `living_room_occupied` / `living_room_empty`
2. Update ESP32 code with room-specific webhooks
3. Create corresponding IFTTT applets

### Conditional Logic
Use IFTTT filters for advanced automation:
- Only turn on lights if after sunset
- Different brightness levels by time of day
- Weekend vs weekday schedules

### Notifications
Add smartphone notifications:
1. IFTTT → Notifications service
2. Send alerts when room occupied/empty
3. Daily/weekly usage summaries

## Troubleshooting Integration

### IFTTT Not Triggering
1. Check webhook URLs in ESP32 code
2. Test URLs directly in browser
3. Verify IFTTT applet status (enabled/disabled)
4. Check service connections

### Alexa Not Responding
1. Ensure Alexa app has proper permissions
2. Re-link IFTTT account in Alexa app
3. Try different voice command phrases
4. Check internet connectivity

### Smart Device Issues
1. Verify device is online in respective app
2. Test manual control through device app
3. Check WiFi connectivity
4. Re-authorize IFTTT service connections

## Security Considerations

### Webhook Security
- Keep webhook keys private
- Don't share webhook URLs publicly
- Regularly check IFTTT activity logs
- Disable unused applets

### Network Security
- Use WPA2/WPA3 WiFi encryption
- Consider separate IoT network
- Regular firmware updates on all devices
- Monitor network traffic for anomalies