# Components List

Everything needed to build the rover. Prices are rough, 2026 estimates for India and the US. They vary a lot by seller, so treat them only as a budget guide.

## Required parts

| Qty | Part | Key specification | Notes | Approx. cost |
|----:|------|-------------------|-------|--------------|
| 1 | Arduino Uno R3 + USB-B cable | ATmega328P, 5 V logic, 16 MHz | Clones work if they use a CH340 or ATmega16U2 USB chip (CH340 may need a driver) | ₹500–900 / $10–25 |
| 1 | L298N dual H-bridge motor driver module | Motor supply 5–35 V, **2 A per channel continuous**, onboard 5 V regulator | Red "L298N" board with 3 screw terminals on the bottom edge and a 6-pin header | ₹150–300 / $2–5 |
| 1 | HC-05 Bluetooth module (6-pin breakout) | Bluetooth 2.0 + EDR (Classic SPP), **3.3 V logic on RXD/TXD**, 3.6–6 V supply on breakout boards | Pins: EN/KEY, VCC, GND, TXD, RXD, STATE. See the HC-06 note below | ₹250–450 / $4–8 |
| 1 | 4WD robot chassis kit with 4 TT gear motors and wheels | DC 3–6 V, about 1:48 gearbox, 4 wheels | Kits normally include motors, wheels, mounting hardware and a battery holder (the holder is usually 4×AA, so buy the Li-ion holder below) | ₹700–1,200 / $12–25 |
| 2 | 18650 Li-ion cells, **protected** (built-in protection circuit) | 3.7 V nominal, 4.2 V full, ≥ 2000 mAh, ≥ 5 A discharge | Buy a reputable brand from a reputable seller. Avoid "9900 mAh" claims | ₹300–700 / $6–15 |
| 1 | 2 × 18650 series battery holder with wire leads | Wired in series, 7.4 V nominal | Check that the red lead is + and the black lead is − | ₹50–100 / $1–3 |
| 1 | 2-slot 18650 Li-ion charger | 4.2 V per cell, with cut-off | Use only a Li-ion charger. Never charge unattended | ₹200–500 / $4–10 |
| 1 | Power switch (SPST or slide) | Rated ≥ 3 A | In series with the battery positive lead | ₹20–50 / $1 |
| 1 | Resistor 1 kΩ, ¼ W | 5 % or better | HC-05 voltage divider (upper leg) | ₹1 |
| 1 | Resistor 2 kΩ, ¼ W | 5 % or better | Voltage divider (lower leg). You can use two 1 kΩ resistors in series instead | ₹1 |
| 1 | Mini breadboard | 170 or 400 tie-point | Holds the HC-05 and divider | ₹50–100 / $2 |
| 1 set | Jumper wires | About 15 male-male and 10 male-female | | ₹100 / $3 |
| 1 | Small screwdriver | Phillips and flat | For the L298N screw terminals | |

**Rough total:** about ₹2,300–4,500 (US$45–100) depending on the chassis and cells.

## Strongly recommended

| Qty | Part | Why |
|----:|------|-----|
| 1 | Digital multimeter | Check battery voltage, continuity and the 3.3 V divider output before powering the HC-05 |
| 1 | Inline fuse holder + 3 A fuse | Protects the pack and wiring from a short circuit |
| 4 | 0.1 µF (104) ceramic capacitors | Solder one across each motor's terminals to reduce electrical noise that can reset the Arduino |
| – | Heat-shrink tubing, zip ties, double-sided foam tape | Tidy, insulated wiring and mounting |

## About the HC-05 vs HC-06

* **HC-05** (6 pins, with STATE): used in this project. It works as a slave, which is all this project needs.
* **HC-06** (4 pins: VCC, GND, TXD, RXD, no STATE): also works as a slave. Connect it the same way (skip the STATE wire) and set `USE_STATE_FAILSAFE` to `0` in the sketch.
* Bare HC-05 chips (the green board with no breakout) run at 3.3 V only. Use the common 6-pin breakout board with a regulator, as sold in most kits.

## Why these choices

* **L298N** is cheap, widely available and simple. It is not efficient: it wastes about 2 V as heat. See [power.md](power.md) for the numbers and the upgrade path.
* **Two motors per channel** keeps the project simple: one L298N drives all four wheels as a left side and a right side (skid steering).
* **18650 Li-ion cells** can deliver the current four motors need. AA cells can work too, with limits: see [power.md](power.md).
