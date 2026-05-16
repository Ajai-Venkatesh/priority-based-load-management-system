// ============================================================
//  relay_control.h
//  Relay initialization, control, and priority load logic
// ============================================================

#ifndef RELAY_CONTROL_H
#define RELAY_CONTROL_H

#include "config.h"

// Internal relay state tracking
static bool relay_state[4] = {false, false, false, false}; // index 1-3 used

// ─── Helper: Write to relay pin ──────────────────────────────
// Handles ACTIVE_LOW vs ACTIVE_HIGH relay logic transparently
void relay_write(uint8_t pin, bool state) {
#if RELAY_ACTIVE_LOW
  digitalWrite(pin, state ? LOW : HIGH);
#else
  digitalWrite(pin, state ? HIGH : LOW);
#endif
}

// ─── Initialize all relays as OUTPUT and turn OFF ────────────
void relay_init() {
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  relay_write(RELAY1_PIN, false);
  relay_write(RELAY2_PIN, false);
  relay_write(RELAY3_PIN, false);
  relay_state[1] = false;
  relay_state[2] = false;
  relay_state[3] = false;
}

// ─── Set individual relay state ──────────────────────────────
void relay_set(uint8_t relayNum, bool state) {
  uint8_t pin;
  switch (relayNum) {
    case 1: pin = RELAY1_PIN; break;
    case 2: pin = RELAY2_PIN; break;
    case 3: pin = RELAY3_PIN; break;
    default: return;
  }
  relay_write(pin, state);
  relay_state[relayNum] = state;
}

// ─── Get current relay state ─────────────────────────────────
bool relay_get_state(uint8_t relayNum) {
  if (relayNum < 1 || relayNum > 3) return false;
  return relay_state[relayNum];
}

// ─── Core Priority Logic ─────────────────────────────────────
//
//  Voltage zones:
//   V >= V_HIGH             → All 3 relays ON
//   V_MED <= V < V_HIGH     → Relay 1 ON, Relay 2 ON, Relay 3 OFF
//   V_LOW <= V < V_MED      → Relay 1 ON, Relay 2 OFF, Relay 3 OFF
//   V < V_LOW               → All relays OFF (undervoltage protection)
//
//  Hysteresis prevents chattering at threshold boundaries:
//   Turn OFF at threshold X
//   Turn back ON only when voltage rises to X + HYST_BAND
//
void apply_priority_logic(float voltage, bool nightMode) {
  // Determine active thresholds with hysteresis
  // We track previous states (relay_state) to apply hysteresis
  bool r1_on, r2_on, r3_on;

  if (voltage < V_CRIT) {
    // Critical undervoltage — all off
    r1_on = false;
    r2_on = false;
    r3_on = false;
  }
  else if (voltage < V_LOW) {
    // Only HIGH priority load gets power
    r1_on = true;
    r2_on = false;
    r3_on = false;
  }
  else if (voltage < V_MED) {
    // HIGH + MEDIUM priority loads
    r1_on = true;
    r2_on = true;
    r3_on = false;
  }
  else {
    // All loads can run
    r1_on = true;
    r2_on = true;
    r3_on = true;
  }

  // Apply hysteresis: only turn a relay back ON if voltage
  // has risen sufficiently above the threshold that turned it off
  if (!relay_state[1] && r1_on && voltage < V_LOW + HYST_BAND) r1_on = false;
  if (!relay_state[2] && r2_on && voltage < V_MED + HYST_BAND) r2_on = false;
  if (!relay_state[3] && r3_on && voltage < V_HIGH + HYST_BAND) r3_on = false;

  // Write to relays
  relay_set(1, r1_on);
  relay_set(2, r2_on);
  relay_set(3, r3_on);

  // In night mode the same relay numbers control different physical loads
  // (managed by physical wiring / labeling, priority order stays the same)
  // The priority profile name is reflected in Serial output only
}

#endif // RELAY_CONTROL_H
