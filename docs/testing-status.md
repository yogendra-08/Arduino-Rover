# Testing Status

This page separates what has actually been checked from what has not. Update it as you test your own build.

## Verified (done without hardware)

| Check | How | Result |
|-------|-----|--------|
| Sketch compiles for Arduino Uno (ATmega328P) | avr-gcc 7.3 with the official `ArduinoCore-avr` and its bundled `SoftwareSerial`, Arduino-IDE-style flags | Passed. About 5.4 KB flash and 340 bytes RAM |
| Compiler warnings | Built again with `-Wall -Wextra` | One harmless warning (a compile-time constant compared with 0) |
| Command logic | The real sketch built on a PC against a mock Arduino API, with 21 checks | All passed: direction pins and PWM for every command (`F B L R G I H J S`), speed levels `0`–`9` and `q`, the `MAX_PWM` cap, ignored characters, STATE failsafe timing, debounce of short glitches, USB commands exempt from the failsafe |
| Pin consistency | Pin numbers in the sketch, `wiring.md`, `tinkercad.md` and the diagram were compared by script | All matching |
| Wiring diagram | SVG rendered and inspected visually | Renders correctly |

## Not verified (needs your hardware)

Nothing below has been physically tested. This project's author-side checks stop at the table above.

| Item | Status |
|------|--------|
| Motors turn the right way with this wiring | Not tested |
| PWM speed control on real motors, including the best `MIN_PWM` value | Not tested (90 is an estimate) |
| HC-05 pairing with an Android phone | Not tested |
| HC-05 STATE pin behavior (HIGH = connected) on your module | Not tested. Modules vary |
| Any specific Android app (Serial Bluetooth Terminal, RC car apps) | Not tested |
| Battery run time, motor current, L298N temperature | Not measured. Numbers in `power.md` are estimates |
| Tinkercad simulation steps | Not tested in Tinkercad. Bluetooth cannot be simulated there |
| Compilation in the Arduino IDE itself (as opposed to avr-gcc directly) | Not run. Expected to be the same |

## Your test checklist

Copy the results into your own notes, or edit this file and commit it with your photos.

- [ ] Voltage divider output measured at about 3.3 V
- [ ] Sketch uploaded; Serial Monitor shows the ready message
- [ ] USB-typed `F` / `B` / `L` / `R` / `S` move the wheels the right way (wheels lifted)
- [ ] Speed levels `1` and `9` produce visibly different speeds
- [ ] HC-05 paired with phone; LED shows the connected pattern
- [ ] Bluetooth commands work from the phone
- [ ] Car stops when Bluetooth is disconnected while moving (failsafe)
- [ ] Ground test at low speed in a clear area
- [ ] L298N temperature checked after 5 minutes of driving
- [ ] Photos and a short demo video added to `media/`
