// Sensor Configuration Template
// Calibration settings for optimal performance

// =============================================================================
// SENSOR PIN CONFIGURATION
// =============================================================================

// Sensor 1 (Entrance sensor - outside room)
#define TRIG_PIN_1 5   // Trigger pin for sensor 1
#define ECHO_PIN_1 18  // Echo pin for sensor 1

// Sensor 2 (Exit sensor - inside room)  
#define TRIG_PIN_2 4   // Trigger pin for sensor 2
#define ECHO_PIN_2 19  // Echo pin for sensor 2

// Alternative pin configurations (uncomment if needed)
/*
// Configuration A: Different GPIO pins
#define TRIG_PIN_1 12
#define ECHO_PIN_1 14
#define TRIG_PIN_2 13
#define ECHO_PIN_2 15

// Configuration B: For limited pin availability
#define TRIG_PIN_1 2
#define ECHO_PIN_1 4
#define TRIG_PIN_2 16
#define ECHO_PIN_2 17
*/

// =============================================================================
// DETECTION PARAMETERS
// =============================================================================

// Distance threshold for detection (in centimeters)
const int SENSOR_THRESHOLD = 75;  // Adjust based on your doorway width

// Sequence timeout for entry/exit detection (milliseconds)
const unsigned long SEQUENCE_TIMEOUT = 3000;  // 3 seconds

// Sensor reading delay (milliseconds)
const int SENSOR_DELAY = 100;  // Time between sensor readings

// Minimum stable reading count before triggering
const int MIN_STABLE_READINGS = 3;  // Helps prevent false triggers

// =============================================================================
// CALIBRATION SETTINGS
// =============================================================================

// Maximum reliable distance for HC-SR04 (centimeters)
const int MAX_SENSOR_DISTANCE = 400;

// Minimum reliable distance (centimeters)  
const int MIN_SENSOR_DISTANCE = 2;

// Temperature compensation (optional)
const float TEMPERATURE_CELSIUS = 20.0;  // Room temperature
const float SPEED_OF_SOUND = 343.0 + (0.6 * TEMPERATURE_CELSIUS);  // m/s

// =============================================================================
// ROOM-SPECIFIC CONFIGURATIONS
// =============================================================================

/*
Small Room (like bathroom):
const int SENSOR_THRESHOLD = 50;
const unsigned long SEQUENCE_TIMEOUT = 2000;

Large Room (like living room):
const int SENSOR_THRESHOLD = 100;
const unsigned long SEQUENCE_TIMEOUT = 5000;

Narrow Hallway:
const int SENSOR_THRESHOLD = 30;
const unsigned long SEQUENCE_TIMEOUT = 1500;

Wide Doorway:
const int SENSOR_THRESHOLD = 150;
const unsigned long SEQUENCE_TIMEOUT = 4000;
*/

// =============================================================================
// SENSOR PLACEMENT GUIDELINES
// =============================================================================

/*
Optimal Sensor Positioning:

1. Height: 1.0 - 1.5 meters from floor
   - Too low: May trigger on pets/objects
   - Too high: May miss detection

2. Distance Apart: 0.5 - 1.0 meters
   - Closer: Better precision, may cause interference
   - Farther: Less precise, more stable

3. Mounting Angle: Horizontal across doorway
   - Slight downward tilt (5-10°) can help
   - Avoid mounting in corners

4. Environment Considerations:
   - Avoid direct sunlight on sensors
   - Keep away from heat sources
   - Ensure stable mounting (vibration affects readings)
   - Clear line of sight across detection area
*/

// =============================================================================
// TROUBLESHOOTING THRESHOLDS
// =============================================================================

// For debugging: Uncomment to enable detailed sensor logging
#define DEBUG_SENSORS 1

#ifdef DEBUG_SENSORS
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif

// Error thresholds
const int SENSOR_ERROR_THRESHOLD = 10;     // Max consecutive errors before alert
const int MAX_READING_VARIANCE = 50;       // Max variance in readings (cm)
const unsigned long SENSOR_TIMEOUT = 30000; // Sensor timeout (30 seconds)

// =============================================================================
// ADAPTIVE THRESHOLD CALCULATION
// =============================================================================

/*
// Example of dynamic threshold adjustment based on environment
int calculateAdaptiveThreshold() {
    static int readings[10];
    static int readingIndex = 0;
    static bool samplesReady = false;
    
    // Take baseline measurements when room is empty
    readings[readingIndex] = (distance1 + distance2) / 2;
    readingIndex = (readingIndex + 1) % 10;
    
    if (readingIndex == 0) samplesReady = true;
    
    if (samplesReady) {
        int sum = 0;
        for (int i = 0; i < 10; i++) {
            sum += readings[i];
        }
        int average = sum / 10;
        return average * 0.7;  // 70% of average distance
    }
    
    return SENSOR_THRESHOLD;  // Use default until calibrated
}
*/

// =============================================================================
// SENSOR FILTERING OPTIONS
// =============================================================================

/*
// Moving average filter to smooth readings
int getFilteredDistance(int trigPin, int echoPin) {
    const int numReadings = 5;
    static int readings1[numReadings] = {0};
    static int readings2[numReadings] = {0};
    static int readIndex1 = 0, readIndex2 = 0;
    static int total1 = 0, total2 = 0;
    
    if (trigPin == TRIG_PIN_1) {
        total1 -= readings1[readIndex1];
        readings1[readIndex1] = readDistance(trigPin, echoPin);
        total1 += readings1[readIndex1];
        readIndex1 = (readIndex1 + 1) % numReadings;
        return total1 / numReadings;
    } else {
        total2 -= readings2[readIndex2];
        readings2[readIndex2] = readDistance(trigPin, echoPin);
        total2 += readings2[readIndex2];
        readIndex2 = (readIndex2 + 1) % numReadings;
        return total2 / numReadings;
    }
}
*/