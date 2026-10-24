# Arduino IDE Setup, Compile and Upload

No extra libraries are needed. The sketch uses only `SoftwareSerial`, which ships with the Arduino AVR core.

## 1. Install the IDE

1. Download **Arduino IDE 2.x** from <https://www.arduino.cc/en/software> and install it.
2. Plug the Uno into the PC with a USB cable.
3. Clones with a CH340 USB chip may need the CH340 driver. The genuine Uno and ATmega16U2 clones do not.

## 2. Open the sketch

1. Clone or download this repository.
2. In the IDE use **File → Open…** and select `firmware/rover_bluetooth/rover_bluetooth.ino`.
3. Keep the folder name `rover_bluetooth` the same as the file name. The IDE requires them to match.

## 3. Select board and port

* **Tools → Board → Arduino AVR Boards → Arduino Uno**
* **Tools → Port →** the port that appears when the Uno is connected (for example `COM3` on Windows, `/dev/ttyACM0` on Linux, `/dev/cu.usbmodem…` on macOS).

## 4. Compile (Verify)

Click the **✓ Verify** button. A successful build ends with a message like:

```
Sketch uses about 5.4 KB (16%) of program storage space.
Global variables use about 340 bytes (16%) of dynamic memory.
```

The figures above come from my test compile of this sketch with avr-gcc 7.3 and the official Arduino AVR core. Your IDE's numbers can differ slightly with another toolchain version.

## 5. Upload

1. **Wheels off the ground. Battery switch off** (USB power is enough for uploading).
2. Click **→ Upload**.
3. The HC-05 uses pins D2/D3, not D0/D1, so it does **not** have to be unplugged for uploads.

## 6. Check it with the Serial Monitor

1. **Tools → Serial Monitor**, set the speed to **9600 baud**.
2. You should see: `Arduino-Rover ready. Commands: F B L R G I H J S 0-9 q`
3. Type `F` and press Send. The monitor shows something like:
   `USB cmd 'F'  move=1  level=7  pwm=195`
4. With the battery on and wheels lifted, the wheels should spin. Type `S` to stop.

Meaning of `move=`: 0 stop, 1 forward, 2 backward, 3 left, 4 right, 5 fwd-left, 6 fwd-right, 7 back-left, 8 back-right.

## Command-line option (arduino-cli)

```bash
arduino-cli core install arduino:avr
arduino-cli compile --fqbn arduino:avr:uno firmware/rover_bluetooth
arduino-cli upload  --fqbn arduino:avr:uno -p COM3 firmware/rover_bluetooth   # use your port
```

## Settings you may want to tune (top of the sketch)

| Constant | Default | Purpose |
|----------|---------|---------|
| `MAX_PWM` | 230 | Highest PWM allowed. Lower it for higher-voltage batteries |
| `MIN_PWM` | 90 | Lowest PWM at which the car starts moving. Raise it if level 1 only hums |
| `DEFAULT_LEVEL` | 7 | Speed level at power-up (0–9) |
| `CURVE_INNER_PERCENT` | 40 | Inner-wheel speed during curved moves |
| `USE_STATE_FAILSAFE` | 1 | Set to 0 if HC-05 STATE is not wired to D4 |
| `COMMAND_TIMEOUT_MS` | 0 | Stop if no Bluetooth command arrives in this time (0 = off) |
