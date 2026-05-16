// ============================================================
//  config.h — Priority Based Load Management System
//  Configure all thresholds, pins, and mode settings here
// ============================================================

#ifndef CONFIG_H
#define CONFIG_H

// ─── Relay Output Pins ────────────────────────────────────────
#define RELAY1_PIN   2    // HIGH Priority load
#define RELAY2_PIN   3    // MEDIUM Priority load
#define RELAY3_PIN   4    // LOW Priority load

// Set to 1 if your relay module triggers on LOW signal (common for optocoupler boards)
#define RELAY_ACTIVE_LOW  1

// ─── Voltage Sensor ──────────────────────────────────────────
#define VOLT_SENSOR_PIN   A0

// Calibration: actual_voltage = (adc_reading / 1023.0) * 5.0 * VOLT_FACTOR
// Adjust VOLT_FACTOR based on your voltage divider ratio or sensor specs
// For a 0-25V sensor module: typical factor is 5.0
#define VOLT_FACTOR       5.0

// Number of ADC samples to average per reading (reduces noise)
#define ADC_SAMPLES       10

// ─── Voltage Thresholds (in Volts) ───────────────────────────
// All loads ON (normal supply)
#define V_HIGH   220.0

// Shed LOW priority load (Relay 3 OFF)
#define V_MED    200.0

// Shed MEDIUM priority load (Relay 2 OFF)
#define V_LOW    180.0

// Undervoltage: ALL loads OFF (protect all equipment)
#define V_CRIT   150.0

// Hysteresis band — prevents chattering near threshold edges (in Volts)
#define HYST_BAND  3.0

// ─── Day / Night Mode ────────────────────────────────────────
// Uses DS3231 RTC (I2C: SDA=A4, SCL=A5)
#define USE_RTC          1      // Set 0 to disable RTC, use fixed mode
#define DAY_START_HOUR   6      // 06:00 — Day mode begins
#define NIGHT_START_HOUR 22     // 22:00 — Night mode begins

// ─── Day Mode Priority (load labels for Serial output) ───────
// Relay 1 = Lighting, Relay 2 = HVAC, Relay 3 = Fan
#define DAY_LOAD1  "LIGHTING"
#define DAY_LOAD2  "HVAC"
#define DAY_LOAD3  "FAN"

// ─── Night Mode Priority ─────────────────────────────────────
// Relay 1 = Security System, Relay 2 = Lighting, Relay 3 = HVAC
#define NIGHT_LOAD1  "SECURITY"
#define NIGHT_LOAD2  "LIGHTING"
#define NIGHT_LOAD3  "HVAC"

// ─── Optional LCD ────────────────────────────────────────────
#define USE_LCD        0     // Set 1 to enable 16x2 LCD output
#define LCD_ADDR    0x27     // I2C address of LCD (common: 0x27 or 0x3F)
#define LCD_COLS       16
#define LCD_ROWS        2

// ─── Serial Debug ────────────────────────────────────────────
#define BAUD_RATE     9600
#define DEBUG_MODE       1   // 1 = print relay states every loop cycle

// ─── Loop Timing ─────────────────────────────────────────────
#define LOOP_DELAY_MS  1000  // Main loop delay in milliseconds

#endif // CONFIG_H
