# Theory of Operation

## Priority Based Load Management System

### 1. Problem Statement

In areas with unstable power supply, voltage sags or drops cause damage to sensitive equipment and erratic operation of all connected loads. Without managed load shedding, all loads share the suffering equally — including critical ones like security systems, lighting, and motors.

This system ensures that when power quality degrades, the most important loads remain powered while less critical loads are disconnected in a defined priority order.

---

### 2. Voltage Sensing

The **voltage sensor module** (a resistive voltage divider followed by a buffer amplifier) scales the input voltage (0–25V DC, or AC via rectifier) down to the Arduino's safe 0–5V analog range.

The Arduino reads this via `analogRead()` on pin A0, which returns a 10-bit value (0–1023). The conversion formula is:

```
actual_voltage = (ADC_value / 1023) × 5.0 × VOLT_FACTOR
```

`VOLT_FACTOR` is determined during calibration and compensates for the sensor's attenuation ratio.

**Noise reduction**: The firmware averages 10 ADC samples per reading cycle, significantly reducing electrical noise from relay switching and inductive loads.

---

### 3. Priority Zones

The voltage range is divided into four zones:

```
                    ┌──────────────────────────────┐
  V ≥ 220V (V_HIGH) │  ALL LOADS ON (R1+R2+R3)     │
                    ├──────────────────────────────┤
200V ≤ V < 220V     │  R1 ON, R2 ON, R3 OFF         │
  (V_MED to V_HIGH) │  (LOW priority load shed)     │
                    ├──────────────────────────────┤
180V ≤ V < 200V     │  R1 ON, R2 OFF, R3 OFF        │
  (V_LOW to V_MED)  │  (MED + LOW loads shed)       │
                    ├──────────────────────────────┤
  V < 180V (V_CRIT) │  ALL LOADS OFF                │
                    │  (Undervoltage protection)    │
                    └──────────────────────────────┘
```

Thresholds are fully configurable in `config.h`.

---

### 4. Hysteresis

Without hysteresis, a voltage fluctuating around 200V would cause Relay 3 to rapidly switch ON and OFF, creating relay wear, noise, and load damage.

Hysteresis adds a recovery band above each threshold:
- Relay 3 is turned OFF when voltage drops below `V_HIGH` (220V)
- Relay 3 is only turned back ON when voltage rises above `V_HIGH + HYST_BAND` (223V)

This 3V dead-band prevents chattering and is configurable via `HYST_BAND` in `config.h`.

---

### 5. Day/Night Mode Switching

The **DS3231 RTC** provides accurate time-keeping with temperature compensation, maintaining time even during power outages via a CR2032 backup battery.

The firmware reads the current hour and determines the active mode:

- **Day Mode** (06:00–21:59): Relay 1 = Lighting, Relay 2 = HVAC, Relay 3 = Fan
- **Night Mode** (22:00–05:59): Relay 1 = Security, Relay 2 = Lighting, Relay 3 = HVAC

The priority order (Relay 1 > Relay 2 > Relay 3) remains the same — what changes is which physical load is wired to which relay. In practice, the "priority wiring" means Relay 1 always stays ON longest, so the most critical load for that time period should be physically connected to Relay 1.

Alternatively, with a relay matrix or re-mapping table in firmware, the loads can be software-switched between day/night profiles without rewiring.

---

### 6. Relay Driver Circuit

Each relay module typically includes:
- An optocoupler for isolation between Arduino logic and relay coil
- A transistor driver for the relay coil current
- A flyback diode to suppress inductive kick when the coil de-energizes
- An LED indicator showing relay state

The Arduino GPIO pin sources only the optocoupler LED current (~5–10 mA), well within the 40 mA per-pin limit. The relay coil is powered directly from the 5V supply.

---

### 7. System Flow

```
                    [Power ON]
                        │
                    [Init Relays OFF]
                        │
                    [Init RTC]
                        │
              ┌─────────▼──────────┐
              │    READ VOLTAGE     │ ← analogRead(A0) × 10 samples
              └─────────┬──────────┘
                        │
              ┌─────────▼──────────┐
              │  READ RTC TIME     │ ← DS3231 over I2C
              └─────────┬──────────┘
                        │
              ┌─────────▼──────────┐
              │  DAY or NIGHT?     │
              └───┬────────────────┘
             DAY  │  NIGHT
                  │
         ┌────────▼──────────────┐
         │  APPLY PRIORITY LOGIC  │
         │  V ≥ V_HIGH → R1R2R3  │
         │  V < V_HIGH → R1R2    │
         │  V < V_MED  → R1      │
         │  V < V_CRIT → none    │
         └────────┬──────────────┘
                  │
         ┌────────▼──────────────┐
         │  SET RELAY OUTPUTS     │
         └────────┬──────────────┘
                  │
         ┌────────▼──────────────┐
         │  SERIAL / LCD OUTPUT   │
         └────────┬──────────────┘
                  │
             [DELAY 1s]
                  │
              [REPEAT]
```

---

### 8. Safety Notes

- ⚠️ This system is designed for **DC loads** when used with Arduino directly. For **AC mains** (230V/110V), all high-voltage wiring must be performed by a qualified electrician and housed in a proper enclosure.
- The relay contacts are rated for their specified voltage/current — never exceed the relay's rating.
- Always include a main fuse on the supply side.
- The Arduino and relay control circuits must be electrically isolated from mains voltage.
