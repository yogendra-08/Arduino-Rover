# Troubleshooting

Work through the sections in order. Always test with the wheels lifted, and power off before touching wiring.

## Power

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| Nothing lights up | Switch off, battery flat, polarity or fuse | Measure battery voltage (≥ 7 V). Check the switch, fuse and + / − leads |
| Arduino resets when motors start | Battery voltage sags, weak cells, bad connection | Charge the pack. Check the contacts in the battery holder. Add 0.1 µF capacitors across each motor |
| Arduino resets when driving, fine when idle | Same as above, or motor noise | Fit the capacitors. Keep motor wires away from the Arduino |
| L298N gets very hot | Stalled or overloaded motors, long full-speed runs | Press `S`. Reduce load. Allow cooling. See the limits in [power.md](power.md) |
| Burning smell | Short circuit or reversed polarity | Disconnect the battery **immediately**. Inspect all wiring before trying again |
| Pack voltage drops below 6.4 V | Pack nearly empty | Recharge. Do not run Li-ion cells flat |

## Uploading

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| No port listed | Cable is charge-only, or driver missing | Use a data USB cable. Install the CH340 driver for clones |
| `avrdude: stk500_getsync()` error | Wrong board or port, or something on D0/D1 | Select Arduino Uno and the correct port. Disconnect any wires on D0/D1 |
| Sketch will not open | Folder name differs from the `.ino` name | Keep `rover_bluetooth/rover_bluetooth.ino` |

## Bluetooth

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| HC-05 LED is off | No power | Check VCC → 5V and GND → GND. Measure 5 V on the module's VCC |
| HC-05 not visible on the phone | Module not powered, or already connected elsewhere | Power-cycle. Make sure no other phone is connected |
| Pairing fails | Wrong PIN | Try `1234`, then `0000`. Remove the old pairing and re-pair |
| Connects, but the car does not react | TX/RX swapped, wrong pins, or wrong baud | HC-05 **TXD → D2** and **RXD ← D3 (via divider)**. Data baud must be 9600 (`BT_BAUD`) |
| Garbage characters in Serial Monitor | Baud mismatch | Set the Serial Monitor to 9600. If you changed the HC-05 baud, update `BT_BAUD` |
| Works for a moment then stops | Failsafe triggered (STATE low) or Bluetooth dropout | Check the STATE → D4 wire. Set `USE_STATE_FAILSAFE` to 0 if unwired. Move closer |
| Car stops by itself the moment a phone connects or disconnects | STATE failsafe doing its job | Expected on disconnect |
| Car moves erratically with STATE not wired | D4 left floating | Wire STATE → D4, or set `USE_STATE_FAILSAFE` to 0 |
| HC-05 gets hot or died | 5 V sent to RXD without the divider | Fit the 1 kΩ / 2 kΩ divider. The module may need replacing |
| Android app cannot connect | Permission not granted | Allow **Nearby devices** (Android 12+) or Location (older). Re-pair |
| iPhone cannot connect | HC-05 is Bluetooth Classic | Not supported on iOS. Use an Android phone |

## Motors and movement

| Symptom | Likely cause | Fix |
|---------|--------------|-----|
| Nothing moves, Serial Monitor shows commands | Motor battery off, or no power on L298N 12V | Check the switch and the L298N 12V and GND terminals |
| Motors hum but do not turn | PWM too low for the load | Raise `MIN_PWM`, use a higher speed level, charge the battery |
| One side never moves | Wiring on IN or EN, or loose OUT wires | Re-check that side against [wiring.md](wiring.md). Swap leads to confirm the motors work |
| Car moves at full speed and ignores speed levels | ENA/ENB jumper caps still on | Remove the jumper caps. Confirm D5 → ENA and D6 → ENB |
| Forward makes one side go backward | Motor leads reversed on that side | Swap that side's two leads at OUT1/OUT2 or OUT3/OUT4 |
| Left and right are swapped | Left and right motor pairs are on the opposite outputs | Swap the pairs at the L298N terminals (OUT1/OUT2 with OUT3/OUT4) |
| The two motors on one side fight each other | One motor is wired in reverse | Flip the leads of one motor so both turn the same way |
| Car drifts to one side | Cheap motors are never perfectly matched; wheel friction differs | Normal at some level. Lower the speed or check for a dragging wheel. Per-side trim would need a code change (see future improvements in the README) |
| Car does not stop on `S` | Command not received | Check the Serial Monitor output. Press the app's stop button. Check the Bluetooth link |

## Quick bench test (no phone)

1. USB connected, battery off, wheels lifted.
2. Serial Monitor at 9600 baud. Type `F`. The monitor should print `USB cmd 'F' …`. If it does not, the sketch did not upload.
3. Switch the battery on. Type `F`, then `S`. If the wheels spin, the L298N side works. If not, review wiring.
4. Only then test with Bluetooth.

## Checking the voltage divider

Do this **before** the HC-05 is connected to the divider. Upload the sketch, then measure from the divider's middle point to GND while the sketch is idle. Arduino D3 idles HIGH (the serial idle level), so you should read about **3.3 V**. A reading near 5 V means the lower resistor is missing or wrong, so fix it before connecting the HC-05 RXD pin.
