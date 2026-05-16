# ⚡ Priority Based Load Management System

> **3 Relay | 1 Voltage Sensor | Arduino-based Smart Load Switching**

A smart energy management system that prioritizes critical electrical loads based on supply voltage levels and automatically switches between **Day Mode** and **Night Mode** priority profiles. Built around an Arduino microcontroller with 3 relay outputs and 1 voltage sensor.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Hardware Requirements](#hardware-requirements)
- [Circuit Diagram](#circuit-diagram)
- [Priority Logic](#priority-logic)
- [Day/Night Mode Switching](#daynight-mode-switching)
- [Pin Configuration](#pin-configuration)
- [Installation & Setup](#installation--setup)
- [Code Structure](#code-structure)
- [How It Works](#how-it-works)
- [Troubleshooting](#troubleshooting)
- [License](#license)

---

## 🔍 Overview

This system monitors supply voltage in real-time and intelligently manages three relay-controlled loads. When supply voltage drops below thresholds, lower-priority loads are automatically shed (switched OFF) to protect high-priority loads. Time-of-day awareness allows different priority profiles for daytime and nighttime operation.

```
[AC Supply] → [Voltage Sensor] → [Arduino MCU] → [Relay 1] → [Load 1 - HIGH Priority]
                                               → [Relay 2] → [Load 2 - MEDIUM Priority]
                                               → [Relay 3] → [Load 3 - LOW Priority]
```

---

## ✨ Features

- ✅ Real-time voltage monitoring via analog sensor
- ✅ 3-tier priority load management (HIGH / MEDIUM / LOW)
- ✅ Automatic load shedding when voltage drops
- ✅ Day Mode and Night Mode priority switching
- ✅ Relay status display via Serial Monitor (or LCD optional)
- ✅ Hysteresis to prevent relay chattering
- ✅ Configurable voltage thresholds
- ✅ Expandable to more relays

---

## 🔧 Hardware Requirements

| Component | Qty | Description |
|---|---|---|
| Arduino Uno / Nano | 1 | Main microcontroller (ATmega328P) |
| 5V Relay Module | 3 | Normally Open (NO) relay boards |
| Voltage Sensor Module | 1 | 0–25V DC voltage sensor (ADC-based) |
| RTC Module (DS3231) | 1 | For day/night time detection |
| 10kΩ Resistor | 2 | Voltage divider (if custom sensor) |
| 16x2 LCD (optional) | 1 | Status display |
| 12V / 5V DC Supply | 1 | Power supply for relays and Arduino |
| Jumper Wires | — | For connections |
| Breadboard / PCB | 1 | For prototyping |

---

## 📐 Circuit Diagram

```
                      +5V
                       │
           ┌───────────┴────────────┐
           │       ARDUINO UNO      │
           │                        │
  Vsensor──┤A0               D2─────┤──[RELAY 1 IN]──[LOAD 1]
  SCL RTC──┤A5               D3─────┤──[RELAY 2 IN]──[LOAD 2]
  SDA RTC──┤A4               D4─────┤──[RELAY 3 IN]──[LOAD 3]
           │                  GND───┤──[RELAY GND]
           └────────────────────────┘

  Voltage Sensor:
  [AC/DC Input]──[Resistor Divider]──[A0 pin]──[GND]
```

Full KiCad schematic is in `/hardware/schematic/` folder.

---

## ⚙️ Priority Logic

The system operates on three voltage threshold bands:

| Voltage Range | Action |
|---|---|
| V ≥ V_HIGH | All 3 loads ON |
| V_MED ≤ V < V_HIGH | Load 1 (HIGH) ON, Load 2 ON, Load 3 OFF |
| V_LOW ≤ V < V_MED | Load 1 (HIGH) ON, Load 2 OFF, Load 3 OFF |
| V < V_LOW | All loads OFF (undervoltage protection) |

Default thresholds (configurable in `config.h`):

```cpp
#define V_HIGH  220.0   // All loads ON above this
#define V_MED   200.0   // Shed LOW priority load
#define V_LOW   180.0   // Shed MEDIUM priority load
#define V_CRIT  150.0   // All OFF (undervoltage)
```

---

## 🌅 Day/Night Mode Switching

The system reads real time from a DS3231 RTC module and switches priority profiles:

| Mode | Active Hours | Priority Profile |
|---|---|---|
| **Day Mode** | 06:00 – 21:59 | LIGHTING > HVAC > FAN |
| **Night Mode** | 22:00 – 05:59 | SECURITY > LIGHTING > HVAC |

Configure in `config.h`:

```cpp
#define DAY_START_HOUR   6
#define NIGHT_START_HOUR 22
```

---

## 📌 Pin Configuration

| Arduino Pin | Connected To | Description |
|---|---|---|
| A0 | Voltage Sensor OUT | Analog voltage reading |
| A4 | RTC SDA | I2C Data (DS3231) |
| A5 | RTC SCL | I2C Clock (DS3231) |
| D2 | Relay 1 IN | HIGH Priority Load control |
| D3 | Relay 2 IN | MEDIUM Priority Load control |
| D4 | Relay 3 IN | LOW Priority Load control |
| D13 | LED | Status indicator (optional) |
| 5V | Relay VCC | Relay module power |
| GND | Common GND | Common ground |

---

## 🚀 Installation & Setup

### 1. Clone the Repository

```bash
git clone https://github.com/yourusername/priority-based-load-management-system.git
cd priority-based-load-management-system
```

### 2. Install Arduino IDE

Download from [https://www.arduino.cc/en/software](https://www.arduino.cc/en/software)

### 3. Install Required Libraries

Open Arduino IDE → Tools → Manage Libraries, then install:

- `RTClib` by Adafruit
- `Wire` (built-in)
- `LiquidCrystal` (optional, for LCD)

### 4. Configure Thresholds

Open `src/config.h` and adjust voltage thresholds and pin assignments to match your hardware.

### 5. Upload the Code

1. Open `src/priority_load_manager.ino` in Arduino IDE
2. Select Board: **Arduino Uno** (or Nano)
3. Select correct COM Port
4. Click **Upload**

### 6. Open Serial Monitor

Set baud rate to **9600** to view real-time relay status and voltage readings.

---

## 📁 Code Structure

```
priority-load-management-system/
│
├── src/
│   ├── priority_load_manager.ino   # Main Arduino sketch
│   ├── config.h                    # Thresholds & pin definitions
│   ├── relay_control.h             # Relay logic functions
│   └── voltage_sensor.h            # Voltage reading & calibration
│
├── hardware/
│   ├── schematic/                  # KiCad schematic files
│   ├── bom.csv                     # Bill of Materials
│   └── pcb_layout/                 # PCB design files (optional)
│
├── docs/
│   ├── wiring_diagram.png          # Visual wiring guide
│   ├── flowchart.png               # System logic flowchart
│   └── theory_of_operation.md     # Detailed explanation
│
├── firmware/
│   └── calibration_tool.ino       # Voltage sensor calibration sketch
│
├── images/
│   └── banner.png                  # Repository banner
│
├── LICENSE
└── README.md
```

---

## 🔄 How It Works

1. **Voltage Sensing**: The voltage sensor outputs a scaled analog signal (0–5V) proportional to the input voltage. The Arduino reads this on pin A0 and maps it to actual voltage using a calibration factor.

2. **Time Reading**: The DS3231 RTC provides current hour. The Arduino checks if it is day or night and loads the corresponding priority profile.

3. **Priority Decision**: Based on the measured voltage and the active priority profile, the Arduino decides which relays to energize (ON) or de-energize (OFF).

4. **Hysteresis**: A small deadband prevents relays from rapidly switching when voltage hovers near a threshold.

5. **Output**: Relay states are updated, loads are connected or disconnected, and status is printed to Serial Monitor (and optionally to LCD).

---

## 🛠️ Troubleshooting

| Issue | Possible Cause | Fix |
|---|---|---|
| All relays OFF always | Voltage reading too low | Check sensor wiring, recalibrate `VOLT_FACTOR` |
| Relays chattering | No hysteresis | Increase `HYST_BAND` in `config.h` |
| Wrong day/night mode | RTC not set | Run `firmware/set_rtc_time.ino` once |
| Incorrect voltage reading | Sensor calibration off | Run `firmware/calibration_tool.ino` |
| Relay not switching | Wrong pin or relay logic | Check `RELAY_ACTIVE_LOW` setting |

---

## 📄 License

This project is licensed under the **MIT License** — see [LICENSE](LICENSE) for details.

---

## 🤝 Contributing

Pull requests are welcome! For major changes, please open an issue first to discuss what you would like to change.

---

*Built with ❤️ for smart energy management*
