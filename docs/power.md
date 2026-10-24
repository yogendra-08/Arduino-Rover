# Power, Batteries and Safety

This page checks the selected motor driver, motors and battery against each other, so you can see where the margins are.

## Power architecture

```
 2S Li-ion pack (6.0–8.4 V)
        │ (+)  switch → optional 3 A fuse
        ├──────────────► L298N 12V terminal (VS): motors + L298N logic regulator
        └──────────────► Arduino VIN: Arduino's own 5 V regulator
                              └──► 5V pin ──► HC-05 VCC
 (−) ──────────────────► L298N GND ── Arduino GND ── HC-05 GND  (one common ground)
```

* The motors draw their current from the battery through the L298N. **Nothing a motor needs ever passes through an Arduino pin.** Arduino I/O pins give only a few tens of milliamps, and a motor connected to one can damage the ATmega.
* A common ground is mandatory. Without it, the logic signals have no reference and the car behaves erratically.

## Limits check

### L298N (module)

| Item | Value | Source / note |
|------|-------|---------------|
| Motor supply (VS) | 5–35 V | Module rating. The onboard 5 V regulator needs VS ≥ 7 V |
| 5V-EN jumper | Safe up to about 12 V on VS | Remove it above 12 V and supply logic 5 V separately |
| Current per channel | **2 A continuous**, 3 A peak | L298 chip rating |
| Voltage drop across the driver | About **1.5–3 V**, load-dependent | Bipolar H-bridge. This is the main inefficiency |
| Heat | Roughly drop × current per channel. At 1 A about 2 W | Keep the heat sink uncovered and away from foam or wiring |

### TT gear motors (typical)

| Item | Typical value | Note |
|------|---------------|------|
| Rated voltage | 3–6 V | Do not run at 9 V or more continuously |
| Free-run current | About 0.15–0.25 A at 6 V | Datasheets vary by seller |
| Stall current | Roughly 1 A each at 6 V | **Measure yours** if you can. Cheap motors vary widely |

Typical TT-motor figures vary by seller. These are planning estimates, not measurements from this project's motors.

### Per-channel current (two motors in parallel)

| Condition | Estimated current per channel | vs. 2 A limit |
|-----------|------------------------------|---------------|
| Free-running | ≈ 0.3–0.5 A | Comfortable |
| Driving on carpet or turning | ≈ 0.6–1.0 A | OK |
| **Stalled** (blocked wheels) | **≈ 2 A or more** | **At or above the limit** |

Conclusion: the L298N is adequate for normal driving of this four-motor car. There is **no margin for stalls**. If the car is pushed against a wall with the wheels still commanded, the L298N and the motors can overheat. Press `S` (stop) if the wheels cannot turn. A higher-current driver is listed under future improvements in the [README](../README.md).

### Voltage at the motors

| Pack state | Pack voltage | Approx. voltage at the motors (full PWM) | With `MAX_PWM = 230` |
|------------|-------------|------------------------------------------|----------------------|
| Fully charged | 8.4 V | ≈ 6.4 V (above the 6 V rating) | ≈ 5.8 V |
| Nominal | 7.4 V | ≈ 5.4 V | ≈ 4.9 V |
| Low (stop here) | 6.4 V | ≈ 4.4 V | ≈ 4.0 V |

This is why the sketch caps the PWM at `MAX_PWM = 230` (about 90 %): it keeps a fresh pack inside the 3–6 V motor rating. The drop figure (≈ 2 V) is an estimate. Measure the voltage across a motor while driving, and adjust `MAX_PWM` in the sketch if needed.

### Arduino supply

* VIN accepts 7–12 V. Fresh 2S Li-ion (8.4 V) is fine, and the regulator stays comfortable.
* Below about 6.5 V on VIN, the Uno's 5 V regulator can no longer hold 5 V and the board may reset. That voltage is also roughly where a 2S pack should be recharged, so a resetting car is a "charge me" signal.
* Never feed more than 12 V into VIN.

## Battery options

| Option | Voltage | Verdict |
|--------|---------|---------|
| **2 × 18650 Li-ion in series (recommended)** | 6.0–8.4 V | Best current capability, rechargeable, long run time. Use protected cells, a proper charger, and follow the Li-ion rules below |
| 6 × AA in series | 7.2 V (NiMH) to 9 V (alkaline) | Works. For 9 V alkaline fresh cells set `MAX_PWM` to about 190. NiMH is better than alkaline for current. Heavier |
| 4 × AA in series | 4.8 V (NiMH) to 6.4 V (alkaline) | **Marginal.** Below 7 V the L298N regulator and Uno VIN are borderline and the motors get only about 3–4 V. Usable only for slow testing |
| 9 V PP3 battery | 9 V | **Do not use.** Cannot supply motor current, voltage collapses |
| USB power bank to the motors | 5 V | **Do not use** for the motors. The L298N drop leaves about 3 V |
| Powering motors from the Arduino 5V pin | – | **Never.** Exceeds what the board can supply |

## Safety rules

**Li-ion cells and packs**

1. Use only a **Li-ion charger** (4.2 V per cell). Never charge on a flammable surface or unattended.
2. Prefer **protected** cells. Without protection, stop discharging at 3.0 V per cell (6.0 V pack) to avoid permanent damage.
3. Never short the pack. Fit the optional 3 A fuse, insulate every exposed lead with heat-shrink, and do not leave loose wires near the battery.
4. Do not use swollen, dented, hot or damaged cells. Dispose of them at a battery recycling point.
5. Check polarity twice before the first power-up. Reverse polarity can destroy the Arduino and the L298N instantly.

**While building and testing**

6. Switch off and unplug before changing wiring.
7. First tests: **wheels lifted off the ground** (car on a box or the chassis upside down).
8. Do not run the car near stairs, water, people or pets until you have confirmed stop and failsafe behavior on the bench.
9. Keep the L298N heat sink uncovered. It is normal for it to be warm, and hot enough to hurt is not.
10. Remove the battery if the car will be stored for more than a few days.

**Electronics**

11. Always keep the HC-05 RXD divider in place.
12. Do not connect anything to the L298N 5V terminal while the 5V-EN jumper is on.
