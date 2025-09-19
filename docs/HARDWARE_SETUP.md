# Hardware Setup Guide

## Sensor Placement

### Optimal Positioning
- **Sensor 1 (Entrance)**: Mount outside the room, facing the doorway
- **Sensor 2 (Exit)**: Mount inside the room, near the entrance
- **Distance**: 0.5-1 meter apart for best detection
- **Height**: 1-1.5 meters from ground level

### Mounting Tips
- Use brackets or 3D printed mounts
- Ensure sensors point horizontally across doorway
- Avoid mounting near corners or obstacles
- Test detection range before permanent installation

## Wiring Guide

### ESP32 Pin Connections
```
Component          ESP32 Pin    Notes
---------          ---------    -----
Sensor 1 VCC   →   5V          Power supply
Sensor 1 GND   →   GND         Ground
Sensor 1 TRIG  →   GPIO5       Trigger signal
Sensor 1 ECHO  →   GPIO18      Echo signal

Sensor 2 VCC   →   5V          Power supply  
Sensor 2 GND   →   GND         Ground
Sensor 2 TRIG  →   GPIO4       Trigger signal
Sensor 2 ECHO  →   GPIO19      Echo signal

LCD VCC        →   5V          Power supply
LCD GND        →   GND         Ground
LCD SDA        →   GPIO21      I2C Data
LCD SCL        →   GPIO22      I2C Clock
```

### Power Requirements
- ESP32: 3.3V (via USB or external supply)
- HC-SR04 Sensors: 5V (500mA total)
- LCD Display: 5V (50mA)
- Total: ~550mA at 5V

## Installation Steps

1. **Prepare Components**
   - Check all components for damage
   - Gather tools: screwdriver, wire strippers, multimeter

2. **Create Connections**
   - Use breadboard for prototyping
   - Double-check all connections
   - Test continuity with multimeter

3. **Test Sensors**
   - Upload test code
   - Verify sensor readings
   - Adjust mounting if needed

4. **Final Installation**
   - Secure all connections
   - Mount ESP32 in protective enclosure
   - Run power cable to permanent supply

## Troubleshooting Hardware Issues

### Sensor Not Reading
- Check 5V power supply
- Verify TRIG/ECHO connections
- Test with multimeter
- Replace sensor if faulty

### LCD Not Displaying
- Check I2C address (usually 0x3F or 0x27)
- Verify SDA/SCL connections
- Check power supply
- Test with I2C scanner code

### Intermittent Connections
- Secure all jumper wires
- Use soldered connections for permanent install
- Check for loose breadboard connections