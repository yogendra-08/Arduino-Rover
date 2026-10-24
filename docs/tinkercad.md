# Tinkercad Circuits Simulation

> **Short version:** Tinkercad can check the motor-control logic (directions and PWM speed) using the USB Serial Monitor. **It cannot test Bluetooth.** This guide is untested in Tinkercad itself. The pin mapping is derived from the L293D datasheet and the verified sketch.

## What can and cannot be simulated

| Item | In Tinkercad? |
|------|---------------|
| Arduino Uno running this sketch | Yes (paste the code into the Code editor) |
| Motor direction and PWM speed logic | Yes, using an **L293D** as a stand-in for the L298N |
| Serial Monitor commands (`F`, `B`, `L`, `R`, `S`, `0`–`9`) | Yes |
| **HC-05 Bluetooth radio, phone pairing, Android app** | **No.** I could not confirm that Tinkercad Circuits has an HC-05 or any Bluetooth component. Search "Bluetooth" in its Components panel to check. Even if one appears, it would not connect to a real phone |
| L298N board | Not as a module. Use the L293D chip |
| Real motor current, stall, heat, battery voltage drop | No |
| HC-05 3.3 V voltage-divider behavior | Not meaningful in a simulator. Verify with a multimeter on the real build |
| STATE failsafe | Not applicable (commands from the Serial Monitor are exempt) |

## Build the simulation

Create a new circuit, then add: Arduino Uno R3, L293D, two DC motors, and a battery (for example 9 V) or power supply, and optionally a breadboard.

**L293D wiring** (pin numbers are the standard L293D datasheet numbers, with the notch at the top and pin 1 on the top left):

| L293D pin | Name | Connect to | Equivalent on the real car |
|-----------|------|------------|-----------------------------|
| 1 | 1,2EN | Arduino **D5** | ENA |
| 2 | 1A | Arduino **D7** | IN1 |
| 7 | 2A | Arduino **D8** | IN2 |
| 3 | 1Y | Left motor terminal 1 | OUT1 |
| 6 | 2Y | Left motor terminal 2 | OUT2 |
| 9 | 3,4EN | Arduino **D6** | ENB |
| 10 | 3A | Arduino **D9** | IN3 |
| 15 | 4A | Arduino **D10** | IN4 |
| 11 | 3Y | Right motor terminal 1 | OUT3 |
| 14 | 4Y | Right motor terminal 2 | OUT4 |
| 16 | VCC1 | Arduino 5V | Logic supply |
| 8 | VCC2 | Battery + (about 6–9 V) | Motor supply |
| 4, 5, 12, 13 | GND | Arduino GND and battery − | Common ground |

Use one motor per side in the simulation (it stands in for the pair on the real car). Leave D2, D3 and D4 unconnected.

## Run it

1. Open **Code → Text** and paste the full contents of `rover_bluetooth.ino`.
2. Click **Start Simulation**, then open the **Serial Monitor** at the bottom of the code panel.
3. Type a character and send it:

| Send | Expected in the simulation |
|------|----------------------------|
| `F` | Both motors spin the same way |
| `B` | Both motors spin the opposite way |
| `L` | Left motor reverses, right motor forward |
| `R` | Left motor forward, right motor reverses |
| `S` | Both motors stop |
| `3` then `F` | Motors turn slower than at `9` |

If the Serial Monitor does not respond, check that the code was pasted fully and that the board is a Uno. The L293D tolerates about 600 mA per channel in real life, so it is only a logic stand-in, not a replacement for the L298N.

Save screenshots of the circuit in `media/screenshots/` (see [`media/README.md`](../media/README.md)).
