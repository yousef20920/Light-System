// IFTTT Webhook Configuration Template
// Get your webhook URLs from https://ifttt.com/maker_webhooks

// =============================================================================
// IFTTT WEBHOOK URLS - UPDATE WITH YOUR ACTUAL WEBHOOK URLS
// =============================================================================

// Webhook URL triggered when room becomes occupied
const char* ifttt_webhook_occupied = "https://maker.ifttt.com/trigger/room_occupied/with/key/YOUR_WEBHOOK_KEY";

// Webhook URL triggered when room becomes empty
const char* ifttt_webhook_empty = "https://maker.ifttt.com/trigger/room_empty/with/key/YOUR_WEBHOOK_KEY";

// Optional: Additional webhooks for different events
const char* ifttt_webhook_daily_report = "https://maker.ifttt.com/trigger/daily_energy_report/with/key/YOUR_WEBHOOK_KEY";
const char* ifttt_webhook_system_alert = "https://maker.ifttt.com/trigger/system_alert/with/key/YOUR_WEBHOOK_KEY";

// =============================================================================
// IFTTT SETUP INSTRUCTIONS
// =============================================================================

/*
Step 1: Create IFTTT Account
1. Go to https://ifttt.com and sign up
2. Navigate to "My Applets"

Step 2: Get Your Webhook Key
1. Go to https://ifttt.com/maker_webhooks
2. Click "Documentation"
3. Copy your unique webhook key

Step 3: Create Applets

Applet 1 - Room Occupied (Turn Lights ON)
- IF: Webhooks → Receive a web request → Event Name: "room_occupied"
- THEN: Choose your smart home service (Philips Hue, TP-Link, etc.)
- Action: Turn lights ON

Applet 2 - Room Empty (Turn Lights OFF)  
- IF: Webhooks → Receive a web request → Event Name: "room_empty"
- THEN: Same smart home service as above
- Action: Turn lights OFF

Step 4: Test Your Webhooks
Paste the URLs below in your browser to test:
https://maker.ifttt.com/trigger/room_occupied/with/key/YOUR_KEY
https://maker.ifttt.com/trigger/room_empty/with/key/YOUR_KEY

Should see "Congratulations!" message and trigger your devices.
*/

// =============================================================================
// SUPPORTED SMART HOME SERVICES
// =============================================================================

/*
Popular IFTTT Services:
- Philips Hue: Smart bulbs and lighting
- TP-Link Kasa: Smart plugs and switches
- Smart Life/Tuya: Generic smart devices
- Samsung SmartThings: Hub-based automation
- Amazon Alexa: Voice control
- Google Assistant: Voice control
- LIFX: Smart lighting
- Wyze: Smart home devices
- Nest: Thermostats and cameras
*/

// =============================================================================
// ALEXA INTEGRATION EXAMPLE
// =============================================================================

/*
To integrate with Alexa through IFTTT:

1. Create additional applets:
   - IF: Amazon Alexa → Say a specific phrase → "Turn on room lights"
   - THEN: Webhooks → Make a web request → Use occupied webhook URL

   - IF: Amazon Alexa → Say a specific phrase → "Turn off room lights"  
   - THEN: Webhooks → Make a web request → Use empty webhook URL

2. Voice commands will work like:
   - "Alexa, trigger turn on room lights"
   - "Alexa, trigger turn off room lights"

Note: You can customize the trigger phrases in IFTTT.
*/

// =============================================================================
// ADVANCED WEBHOOK EXAMPLES
// =============================================================================

/*
// Send data with webhooks (for more advanced automation)
String webhookData = "{"value1":"" + String(energySavedToday) + "","value2":"" + String(occupantCount) + ""}";

// Multiple room support
const char* bedroom_occupied = "https://maker.ifttt.com/trigger/bedroom_occupied/with/key/YOUR_KEY";
const char* bedroom_empty = "https://maker.ifttt.com/trigger/bedroom_empty/with/key/YOUR_KEY";
const char* living_room_occupied = "https://maker.ifttt.com/trigger/living_room_occupied/with/key/YOUR_KEY";
const char* living_room_empty = "https://maker.ifttt.com/trigger/living_room_empty/with/key/YOUR_KEY";
*/