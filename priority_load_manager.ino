// ============================================================
//  priority_load_manager.ino
//  Priority Based Load Management System
//  Hardware: Arduino Uno/Nano + 3 Relay Module + Voltage Sensor
//            + DS3231 RTC Module
//
//  Author:  [Your Name]
//  Version: 1.0.0
//  License: MIT
// ============================================================

#include <Wire.h>
#include "RTClib.h"
#include "config.h"
#include "voltage_sensor.h"
#include "relay_control.h"

#if USE_LCD
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
#endif

RTC_DS3231 rtc;

// ─── Global State ────────────────────────────────────────────
bool isNightMode = false;
float currentVoltage = 0.0;

// ─── Setup ───────────────────────────────────────────────────
void setup() {
  Serial.begin(BAUD_RATE);
  Serial.println(F("=== Priority Based Load Management System ==="));
  Serial.println(F("Version 1.0.0 | 3 Relay | 1 Voltage Sensor"));
  Serial.println(F("============================================="));

  // Init relays (all OFF at startup)
  relay_init();

  // Init voltage sensor pin
  pinMode(VOLT_SENSOR_PIN, INPUT);

#if USE_RTC
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println(F("[ERROR] RTC not found! Check wiring."));
    while (1); // Halt
  }
  if (rtc.lostPower()) {
    Serial.println(F("[WARN] RTC lost power. Setting to compile time."));
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  Serial.println(F("[OK] RTC initialized."));
#endif

#if USE_LCD
  lcd.init();
  lcd.backlight();
  lcd.print(F("Load Manager"));
  lcd.setCursor(0, 1);
  lcd.print(F("Starting..."));
  delay(1500);
  lcd.clear();
#endif

  Serial.println(F("[OK] System ready. Starting main loop..."));
  Serial.println();
}

// ─── Main Loop ───────────────────────────────────────────────
void loop() {
  // 1. Read voltage
  currentVoltage = voltage_read();

  // 2. Determine Day/Night mode
#if USE_RTC
  DateTime now = rtc.now();
  int hour = now.hour();
  isNightMode = (hour >= NIGHT_START_HOUR || hour < DAY_START_HOUR);
#else
  // Default: always day mode if no RTC
  isNightMode = false;
#endif

  // 3. Apply priority logic
  apply_priority_logic(currentVoltage, isNightMode);

  // 4. Debug output
#if DEBUG_MODE
  print_status(currentVoltage, isNightMode);
#endif

#if USE_LCD
  update_lcd(currentVoltage, isNightMode);
#endif

  delay(LOOP_DELAY_MS);
}

// ─── Status Print ────────────────────────────────────────────
void print_status(float voltage, bool nightMode) {
  Serial.print(F("[MODE] "));
  Serial.println(nightMode ? F("NIGHT") : F("DAY"));
  Serial.print(F("[VOLT] "));
  Serial.print(voltage, 1);
  Serial.println(F(" V"));
  Serial.print(F("[R1] ")); Serial.println(relay_get_state(1) ? F("ON") : F("OFF"));
  Serial.print(F("[R2] ")); Serial.println(relay_get_state(2) ? F("ON") : F("OFF"));
  Serial.print(F("[R3] ")); Serial.println(relay_get_state(3) ? F("ON") : F("OFF"));
  Serial.println(F("---"));
}

// ─── LCD Update (if enabled) ─────────────────────────────────
#if USE_LCD
void update_lcd(float voltage, bool nightMode) {
  lcd.setCursor(0, 0);
  lcd.print(nightMode ? F("NIGHT ") : F("DAY   "));
  lcd.print(voltage, 0);
  lcd.print(F("V  "));

  lcd.setCursor(0, 1);
  lcd.print(F("R1:"));
  lcd.print(relay_get_state(1) ? F("ON ") : F("OFF"));
  lcd.print(F(" R2:"));
  lcd.print(relay_get_state(2) ? F("ON ") : F("OFF"));
  lcd.print(F(" R3:"));
  lcd.print(relay_get_state(3) ? F("ON") : F("OF"));
}
#endif
