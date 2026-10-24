# Bluetooth Pairing and Android Control

The HC-05 uses **Bluetooth Classic (Serial Port Profile)**. This works with Android. It does **not** work with iPhones or iPads, which do not support Classic Bluetooth serial modules.

## 1. Check the HC-05 LED

Power the rover (wheels lifted). The HC-05's small LED tells you its state:

| LED pattern | Meaning |
|-------------|---------|
| Fast blinking (about twice per second) | Powered, **waiting** for a connection |
| Slow flash every 2 seconds | AT-command mode (not normal operation) |
| Two quick blinks, then a pause (repeating) | **Connected** to a phone |

(Patterns can differ slightly by module revision.) With the STATE pin wired, the sketch treats STATE = HIGH as "connected".

## 2. Pair the phone

1. Open **Settings → Bluetooth** on your Android phone and turn it on.
2. Scan for devices. Select **HC-05**.
3. Enter the PIN. The factory PIN is **1234** (some modules use **0000**).
4. The phone shows HC-05 as paired. The LED keeps fast-blinking until an app opens the connection.

On Android 12 or newer, an app that connects needs the **Nearby devices** permission (older Android versions ask for Location). Allow it when the app asks.

## 3. Control apps

The sketch reads plain ASCII characters, so any Bluetooth serial app that can send single characters works. These are examples, and I have not tested any of them with this project:

* **Serial Bluetooth Terminal** (by Kai Morich): connect to HC-05, then type commands or assign them to macro buttons (M1–M9). Set "newline" to none, although the sketch ignores newlines anyway.
* **Arduino Bluetooth RC Car** and similar apps: use the same one-letter command style as this sketch (F, B, L, R, S, G, I, H, J and 0–9). The Play Store changes often, so check that the app sends those characters, and use a terminal app if not.

Apps change over time. If a particular app does not work, check which characters it sends.

## 4. Command reference

| Send | Action |
|------|--------|
| `F` | Forward |
| `B` | Backward |
| `L` | Spin left (left side reverse, right side forward) |
| `R` | Spin right |
| `G` | Forward-left (left side slower) |
| `I` | Forward-right (right side slower) |
| `H` | Backward-left |
| `J` | Backward-right |
| `S` | Stop |
| `0` … `9` | Speed level. `0` = motors off, `1` = slowest, `9` = fastest (PWM 90 up to the `MAX_PWM` cap of 230) |
| `q` | Maximum speed (same as `9`) |

* Upper and lower case both work. Unknown characters are ignored.
* The speed level is applied immediately, even while moving. The default level at power-up is 7.
* Many RC apps send one character when a button is pressed and `S` when it is released. The sketch supports that style, so movement continues until `S` arrives.

## 5. Safety features

* **Disconnect failsafe**: if STATE stays LOW for 500 ms while the car is moving under Bluetooth control, the car stops. Requires the STATE→D4 wire. Set `USE_STATE_FAILSAFE` to `0` if you skip it, or if you use an HC-06.
* **Optional timeout**: `COMMAND_TIMEOUT_MS` (default 0, off) stops the car if no command arrives for that many milliseconds. Use it with apps that resend a command repeatedly.
* Commands typed in the USB Serial Monitor are not subject to these failsafes, so bench testing works with Bluetooth disconnected.

## 6. Test without a phone

Open the Arduino IDE Serial Monitor at **9600 baud**, type `F` and press Send. The car (or the simulator) should respond and the monitor prints what it received. See [arduino-ide-setup.md](arduino-ide-setup.md).

## 7. Optional: rename the module or change the PIN (AT mode)

Not required. AT commands differ between HC-05 firmware versions, so check your module's datasheet.

1. Disconnect HC-05 VCC. Hold the small button on the module (or connect EN/KEY to 3.3 V) and reconnect VCC. The LED flashes slowly: AT mode, 38400 baud.
2. Use an Arduino running a serial pass-through sketch, or a USB-TTL adapter, at 38400 baud with "Both NL & CR".
3. Typical commands: `AT` (replies `OK`), `AT+NAME=Arduino-Rover`, `AT+PSWD=1234`.
4. Power-cycle the module to leave AT mode.

If you change the data baud rate with `AT+UART`, also change `BT_BAUD` in the sketch.
