# IoT Flood Monitoring System

This repository contains a simple flood-level monitoring and early warning prototype built for the ESP32 and simulated in Wokwi.

## What this project does

The system uses an HC-SR04 ultrasonic sensor to estimate water level and classify flood risk into three bands:

- **LOW**: green LED, no buzzer
- **MEDIUM**: yellow LED, slow buzzer pattern
- **HIGH**: red LED, rapid buzzer pattern

It also prints structured JSON telemetry to Serial output:

```json
{"distance":23.0,"level":77.0,"risk":"HIGH"}
```

## Repository structure

- **`sketch.ino`**  
  Main firmware source code (Arduino-style C++ for ESP32).  
  Includes sensor reading, water-level conversion, risk classification, LED/buzzer control, and serial telemetry.

- **`diagram.json`**  
  Wokwi hardware layout and wiring definition for:
  - ESP32 DevKit
  - HC-SR04 ultrasonic sensor
  - 3 LEDs (green/yellow/red) with resistors
  - Active buzzer

- **`wokwi-project.txt`**  
  Link/metadata pointing to the original Wokwi simulation project.

## Key technologies used

- **ESP32 microcontroller**
- **Arduino framework (C/C++)**
- **HC-SR04 ultrasonic sensing**
- **Wokwi simulator** for virtual prototyping and testing

## Code organization (`sketch.ino`)

The code is organized into clear functional sections:

1. **Pin and threshold configuration**  
   Constants define GPIO assignments and level thresholds.

2. **Measurement helpers**  
   - `measureDistance()` triggers and reads HC-SR04 distance.
   - `distanceToLevel()` converts distance to percentage level.

3. **Actuator control helpers**  
   - `setLEDs()` controls LOW/MEDIUM/HIGH indicators.
   - `setBuzzer()` configures beep timing.
   - `updateBuzzer()` runs a non-blocking buzzer state machine.

4. **`setup()`**  
   Initializes Serial and pin modes.

5. **`loop()`**  
   Performs repeated sensing, risk classification, alert signaling, and telemetry output.

## How to run

1. Open the project in Wokwi (using the link referenced in `https://wokwi.com/projects/461170422572852225`.
2. Start the simulation.
3. Change the HC-SR04 simulated distance value to emulate rising/falling water.
4. Observe LED/buzzer behavior and JSON logs in the serial monitor.
