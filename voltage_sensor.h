// ============================================================
//  voltage_sensor.h
//  Voltage reading, averaging, and calibration
// ============================================================

#ifndef VOLTAGE_SENSOR_H
#define VOLTAGE_SENSOR_H

#include "config.h"

// ─── Read averaged voltage from sensor ───────────────────────
// Takes ADC_SAMPLES readings and returns the average mapped voltage
float voltage_read() {
  long sum = 0;
  for (int i = 0; i < ADC_SAMPLES; i++) {
    sum += analogRead(VOLT_SENSOR_PIN);
    delay(5); // Small delay between samples for stability
  }
  float avgADC = (float)sum / ADC_SAMPLES;

  // Convert ADC value to voltage
  // ADC reference = 5V, 10-bit resolution (0-1023)
  float sensorVoltage = (avgADC / 1023.0) * 5.0;

  // Scale up by VOLT_FACTOR to get actual line voltage
  // (accounts for voltage divider or sensor attenuation ratio)
  float actualVoltage = sensorVoltage * VOLT_FACTOR;

  return actualVoltage;
}

// ─── Raw ADC read (for calibration) ─────────────────────────
int voltage_raw_adc() {
  return analogRead(VOLT_SENSOR_PIN);
}

#endif // VOLTAGE_SENSOR_H
