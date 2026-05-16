// ============================================================
//  calibration_tool.ino
//  Use this sketch to calibrate your voltage sensor.
//  Upload, open Serial Monitor at 9600 baud, and measure
//  the actual voltage with a multimeter. Adjust VOLT_FACTOR
//  in config.h until the readings match.
// ============================================================

#include "config.h"

void setup() {
  Serial.begin(9600);
  Serial.println(F("=== Voltage Sensor Calibration Tool ==="));
  Serial.println(F("Connect your multimeter and compare readings."));
  Serial.println(F("Adjust VOLT_FACTOR in config.h to match."));
  Serial.println(F("======================================="));
}

void loop() {
  long sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += analogRead(VOLT_SENSOR_PIN);
    delay(10);
  }
  float avgADC = (float)sum / 20.0;
  float sensorVoltage = (avgADC / 1023.0) * 5.0;
  float scaledVoltage = sensorVoltage * VOLT_FACTOR;

  Serial.print(F("Raw ADC: "));
  Serial.print((int)avgADC);
  Serial.print(F("  |  Sensor V: "));
  Serial.print(sensorVoltage, 3);
  Serial.print(F("V  |  Scaled V: "));
  Serial.print(scaledVoltage, 2);
  Serial.print(F("V  |  Factor: "));
  Serial.println(VOLT_FACTOR);

  delay(1000);
}
