// Energy Calculation Configuration Template
// Settings for accurate energy consumption tracking

// =============================================================================
// LIGHTING CONFIGURATION
// =============================================================================

// Power consumption of your lights (watts)
const float LIGHT_POWER_WATTS = 60.0;  // Adjust to your actual light wattage

// Number of lights controlled by the system
const int NUMBER_OF_LIGHTS = 2;  // Update based on your setup

// Total system power consumption
const float TOTAL_LIGHT_POWER = LIGHT_POWER_WATTS * NUMBER_OF_LIGHTS;

// =============================================================================
// ENERGY COST CALCULATION
// =============================================================================

// Electricity cost per kilowatt-hour (kWh) in your local currency
const float ENERGY_COST_PER_KWH = 0.12;  // Update with your local rate

// Peak/off-peak pricing (if applicable)
const float PEAK_RATE = 0.15;      // Peak hours rate
const float OFF_PEAK_RATE = 0.08;  // Off-peak hours rate

// Peak hours definition (24-hour format)
const int PEAK_START_HOUR = 17;  // 5 PM
const int PEAK_END_HOUR = 21;    // 9 PM

// =============================================================================
// COMMON LIGHT TYPES AND POWER CONSUMPTION
// =============================================================================

/*
LED Bulbs:
- 5-10W (replaces 40W incandescent)
- 10-15W (replaces 60W incandescent)  
- 15-20W (replaces 75W incandescent)
- 20-25W (replaces 100W incandescent)

CFL Bulbs:
- 9-13W (replaces 40W incandescent)
- 13-18W (replaces 60W incandescent)
- 18-22W (replaces 75W incandescent)
- 23-30W (replaces 100W incandescent)

Incandescent Bulbs:
- 40W, 60W, 75W, 100W (actual consumption)

Fluorescent Tubes:
- T8: 32W per 4-foot tube
- T5: 28W per 4-foot tube
*/

// Example configurations for different setups:

/*
// Single LED bulb setup
const float LIGHT_POWER_WATTS = 12.0;
const int NUMBER_OF_LIGHTS = 1;

// Multiple LED ceiling lights
const float LIGHT_POWER_WATTS = 15.0;
const int NUMBER_OF_LIGHTS = 4;

// Fluorescent office lighting
const float LIGHT_POWER_WATTS = 32.0;
const int NUMBER_OF_LIGHTS = 6;

// Mixed lighting setup
const float LED_POWER = 12.0;
const float HALOGEN_POWER = 50.0;
const float TOTAL_LIGHT_POWER = (LED_POWER * 2) + (HALOGEN_POWER * 1);
*/

// =============================================================================
// ENERGY SAVINGS CALCULATION METHODS
// =============================================================================

// Method 1: Simple time-based calculation
float calculateSimpleEnergySavings(unsigned long timeOccupiedMs) {
    float hoursOccupied = timeOccupiedMs / 3600000.0;  // Convert ms to hours
    float energyUsed = TOTAL_LIGHT_POWER * hoursOccupied / 1000.0;  // kWh
    
    // Assume lights would be on 2x longer without automation
    float energySaved = energyUsed * 1.0;  // 100% savings from automation
    
    return energySaved;
}

// Method 2: Comparison with typical usage patterns
float calculateAdvancedEnergySavings(unsigned long timeOccupiedMs) {
    float hoursOccupied = timeOccupiedMs / 3600000.0;
    
    // Typical manual usage: lights on for average 8 hours/day
    const float TYPICAL_DAILY_HOURS = 8.0;
    float typicalEnergyUsed = TOTAL_LIGHT_POWER * TYPICAL_DAILY_HOURS / 1000.0;
    
    // Actual automated usage
    float actualEnergyUsed = TOTAL_LIGHT_POWER * hoursOccupied / 1000.0;
    
    // Energy saved = typical usage - actual usage
    return max(0.0, typicalEnergyUsed - actualEnergyUsed);
}

// =============================================================================
// COST CALCULATION FUNCTIONS
// =============================================================================

// Calculate cost based on time of day
float calculateEnergyCost(float energyKwh, int currentHour) {
    if (currentHour >= PEAK_START_HOUR && currentHour <= PEAK_END_HOUR) {
        return energyKwh * PEAK_RATE;  // Peak rate
    } else {
        return energyKwh * OFF_PEAK_RATE;  // Off-peak rate
    }
}

// Simple cost calculation
float calculateSimpleCost(float energyKwh) {
    return energyKwh * ENERGY_COST_PER_KWH;
}

// =============================================================================
// REGIONAL ELECTRICITY RATES (EXAMPLES)
// =============================================================================

/*
United States (average rates per kWh):
- Residential: $0.10 - $0.25
- California: ~$0.20
- Texas: ~$0.12
- New York: ~$0.18

Europe (average rates per kWh):
- Germany: ~€0.30 ($0.33)
- UK: ~£0.15 ($0.18)
- France: ~€0.18 ($0.20)

Other Regions:
- Canada: ~CAD $0.12 ($0.09 USD)
- Australia: ~AUD $0.25 ($0.16 USD)
- Japan: ~¥25 ($0.18 USD)
*/

// =============================================================================
// CARBON FOOTPRINT CALCULATION (OPTIONAL)
// =============================================================================

// Carbon emissions per kWh (kg CO2) - varies by region and energy source
const float CO2_PER_KWH = 0.5;  // Global average, update for your region

/*
Regional CO2 emissions (kg CO2 per kWh):
- Coal-heavy grids: 0.8 - 1.0
- Natural gas: 0.4 - 0.5
- Nuclear/Hydro: 0.02 - 0.05
- Solar/Wind: 0.01 - 0.05

Examples by country:
- Australia: ~0.85 (coal-heavy)
- Germany: ~0.45 (mixed)
- France: ~0.08 (nuclear-heavy)
- Norway: ~0.02 (hydro-heavy)
*/

float calculateCO2Savings(float energySavedKwh) {
    return energySavedKwh * CO2_PER_KWH;  // kg CO2 saved
}

// =============================================================================
// REPORTING INTERVALS
// =============================================================================

// Reset intervals for different reporting periods
const unsigned long HOUR_IN_MS = 3600000;
const unsigned long DAY_IN_MS = 86400000;
const unsigned long WEEK_IN_MS = 604800000;
const unsigned long MONTH_IN_MS = 2629746000;  // Average month
const unsigned long YEAR_IN_MS = 31556952000;  // Average year

// =============================================================================
// VALIDATION AND LIMITS
// =============================================================================

// Maximum reasonable values for validation
const float MAX_DAILY_ENERGY_KWH = 10.0;    // Sanity check
const float MAX_HOURLY_ENERGY_KWH = 1.0;    // Sanity check
const float MIN_ENERGY_THRESHOLD = 0.001;   // Minimum trackable energy

// Validate energy calculation
bool isValidEnergyValue(float energyKwh) {
    return (energyKwh >= 0 && energyKwh <= MAX_DAILY_ENERGY_KWH);
}

// =============================================================================
// EXAMPLE USAGE IN MAIN CODE
// =============================================================================

/*
// In your main loop or energy update function:
void updateEnergyCalculations() {
    if (roomOccupiedSince > 0 && !roomOccupied) {
        unsigned long sessionTime = millis() - roomOccupiedSince;
        float energySaved = calculateSimpleEnergySavings(sessionTime);
        float costSaved = calculateSimpleCost(energySaved);
        float co2Saved = calculateCO2Savings(energySaved);
        
        // Update running totals
        energySavedToday += energySaved;
        energySavedWeek += energySaved;
        energySavedMonth += energySaved;
        energySavedYear += energySaved;
        
        // Log the savings
        Serial.printf("Session: %.3f kWh saved, $%.2f, %.2f kg CO2\n", 
                     energySaved, costSaved, co2Saved);
    }
}
*/