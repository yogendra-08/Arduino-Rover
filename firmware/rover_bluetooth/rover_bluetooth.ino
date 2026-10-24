/*
  Arduino-Rover: Bluetooth-controlled 4-motor car
  Board  : Arduino Uno (ATmega328P)
  Driver : L298N dual H-bridge (two motors per channel, wired in parallel)
  Radio  : HC-05 Bluetooth Classic (SPP) module at 9600 baud

  Pin map (must match docs/wiring.md and media/diagrams/wiring.svg):
    D2  <- HC-05 TXD          (SoftwareSerial RX)
    D3  -> HC-05 RXD          (SoftwareSerial TX, THROUGH 1k/2k VOLTAGE DIVIDER)
    D4  <- HC-05 STATE        (HIGH = phone connected; used as a failsafe)
    D5  -> L298N ENA (PWM)    left-side speed
    D6  -> L298N ENB (PWM)    right-side speed
    D7  -> L298N IN1          left-side direction
    D8  -> L298N IN2          left-side direction
    D9  -> L298N IN3          right-side direction
    D10 -> L298N IN4          right-side direction
    D13 -> onboard LED        ON while the car is moving

  Commands (one ASCII character each, from Bluetooth OR the USB Serial Monitor):
    F forward      B backward     L spin left     R spin right
    G fwd-left     I fwd-right    H back-left     J back-right
    S stop
    0..9 speed level (0 = motors off, 9 = maximum), q = maximum
  Upper and lower case are both accepted. Other characters are ignored.

  Status: compiles for Arduino Uno. Behaviour on real hardware is tracked in
  docs/testing-status.md.
*/

#include <SoftwareSerial.h>

// ---------------------------------------------------------------- Pins ------
const uint8_t PIN_BT_RX    = 2;   // Arduino receives  <- HC-05 TXD
const uint8_t PIN_BT_TX    = 3;   // Arduino transmits -> divider -> HC-05 RXD
const uint8_t PIN_BT_STATE = 4;   // HC-05 STATE (connection indicator)

const uint8_t PIN_ENA = 5;        // PWM  left motors speed
const uint8_t PIN_ENB = 6;        // PWM  right motors speed
const uint8_t PIN_IN1 = 7;        // left motors direction
const uint8_t PIN_IN2 = 8;
const uint8_t PIN_IN3 = 9;        // right motors direction
const uint8_t PIN_IN4 = 10;

// ------------------------------------------------------- Configuration ------
const long     BT_BAUD            = 9600;  // HC-05 factory data rate
const long     USB_BAUD           = 9600;  // Serial Monitor speed

// Highest PWM value ever sent to the driver. 255 = full battery voltage minus
// the L298N drop. 230 (~90 %) keeps 3-6 V TT motors inside their rating on a
// freshly charged 2S Li-ion pack (8.4 V). Lower it for higher-voltage packs.
const uint8_t  MAX_PWM            = 230;

// Lowest PWM at which the loaded car reliably starts moving (speed level 1).
// Tune this value for your motors and battery.
const uint8_t  MIN_PWM            = 90;

const uint8_t  DEFAULT_LEVEL      = 7;     // speed level at power-up (0..9)

// Percentage of speed kept on the inner side during G/I/H/J curved moves.
const uint8_t  CURVE_INNER_PERCENT = 40;

// Failsafe 1: stop if HC-05 STATE stays LOW (phone disconnected) this long.
// Set USE_STATE_FAILSAFE to 0 if you did not wire HC-05 STATE to D4.
#define        USE_STATE_FAILSAFE 1
const unsigned long STATE_LOW_MS  = 500;

// Failsafe 2: stop if no Bluetooth command arrives for this long (0 = off).
// Leave at 0 for apps that send one character per press and 'S' on release.
const unsigned long COMMAND_TIMEOUT_MS = 0;

// ------------------------------------------------------------- State --------
enum Move : uint8_t {
  MOVE_STOP, MOVE_FWD, MOVE_BACK, MOVE_LEFT, MOVE_RIGHT,
  MOVE_FWD_LEFT, MOVE_FWD_RIGHT, MOVE_BACK_LEFT, MOVE_BACK_RIGHT
};

SoftwareSerial bt(PIN_BT_RX, PIN_BT_TX);

Move          currentMove      = MOVE_STOP;
uint8_t       speedLevel       = DEFAULT_LEVEL;
bool          lastCmdFromBt    = false;   // failsafes only apply to Bluetooth control
unsigned long lastBtCmdMs      = 0;
bool          stateLow         = false;
unsigned long stateLowSinceMs  = 0;

// --------------------------------------------------------- Motor layer ------

// Convert speed level 0..9 to a PWM value.
uint8_t levelToPwm(uint8_t level) {
  if (level == 0) return 0;
  return map(level, 1, 9, MIN_PWM, MAX_PWM);
}

// One side of the car. speed is -255..255: sign = direction, 0 = stop.
void setSide(uint8_t pinA, uint8_t pinB, uint8_t pinEn, int speed) {
  if (speed > 0) {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
  } else if (speed < 0) {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
    speed = -speed;
  } else {
    digitalWrite(pinA, LOW);     // both LOW + enable LOW = coast stop
    digitalWrite(pinB, LOW);
  }
  analogWrite(pinEn, constrain(speed, 0, 255));
}

void driveSides(int left, int right) {
  setSide(PIN_IN1, PIN_IN2, PIN_ENA, left);
  setSide(PIN_IN3, PIN_IN4, PIN_ENB, right);
}

// Apply currentMove using the current speed level.
void applyMove() {
  const int full  = levelToPwm(speedLevel);
  const int inner = (full * CURVE_INNER_PERCENT) / 100;

  switch (currentMove) {
    case MOVE_FWD:        driveSides( full,   full);  break;
    case MOVE_BACK:       driveSides(-full,  -full);  break;
    case MOVE_LEFT:       driveSides(-full,   full);  break;  // spin on the spot
    case MOVE_RIGHT:      driveSides( full,  -full);  break;
    case MOVE_FWD_LEFT:   driveSides( inner,  full);  break;
    case MOVE_FWD_RIGHT:  driveSides( full,   inner); break;
    case MOVE_BACK_LEFT:  driveSides(-inner, -full);  break;
    case MOVE_BACK_RIGHT: driveSides(-full,  -inner); break;
    default:              driveSides(0, 0);           break;
  }
  digitalWrite(LED_BUILTIN, (currentMove != MOVE_STOP && full > 0) ? HIGH : LOW);
}

void setMove(Move m) {
  currentMove = m;
  applyMove();
}

void stopCar() {
  setMove(MOVE_STOP);
}

// ---------------------------------------------------- Command handling ------
void handleCommand(char c, bool fromBluetooth) {
  if (c >= 'a' && c <= 'z' && c != 'q') c -= 32;     // to upper case (keep 'q')
  if (c == 'Q') c = 'q';

  bool known = true;
  switch (c) {
    case 'F': setMove(MOVE_FWD);        break;
    case 'B': setMove(MOVE_BACK);       break;
    case 'L': setMove(MOVE_LEFT);       break;
    case 'R': setMove(MOVE_RIGHT);      break;
    case 'G': setMove(MOVE_FWD_LEFT);   break;
    case 'I': setMove(MOVE_FWD_RIGHT);  break;
    case 'H': setMove(MOVE_BACK_LEFT);  break;
    case 'J': setMove(MOVE_BACK_RIGHT); break;
    case 'S': stopCar();                break;
    case 'q': speedLevel = 9; applyMove(); break;
    default:
      if (c >= '0' && c <= '9') {
        speedLevel = c - '0';
        applyMove();                                  // takes effect immediately
      } else {
        known = false;
      }
      break;
  }

  if (known) {
    lastCmdFromBt = fromBluetooth;
    if (fromBluetooth) lastBtCmdMs = millis();
    Serial.print(fromBluetooth ? F("BT  cmd '") : F("USB cmd '"));
    Serial.print(c);
    Serial.print(F("'  move="));
    Serial.print(currentMove);
    Serial.print(F("  level="));
    Serial.print(speedLevel);
    Serial.print(F("  pwm="));
    Serial.println(levelToPwm(speedLevel));
  }
}

// ----------------------------------------------------------- Failsafes ------
void checkFailsafes() {
  if (currentMove == MOVE_STOP || !lastCmdFromBt) return;

#if USE_STATE_FAILSAFE
  if (digitalRead(PIN_BT_STATE) == LOW) {
    if (!stateLow) {
      stateLow = true;
      stateLowSinceMs = millis();
    } else if (millis() - stateLowSinceMs >= STATE_LOW_MS) {
      Serial.println(F("FAILSAFE: Bluetooth disconnected -> stop"));
      stopCar();
      stateLow = false;
      return;
    }
  } else {
    stateLow = false;
  }
#endif

  if (COMMAND_TIMEOUT_MS > 0 && millis() - lastBtCmdMs >= COMMAND_TIMEOUT_MS) {
    Serial.println(F("FAILSAFE: command timeout -> stop"));
    stopCar();
  }
}

// ------------------------------------------------------ Arduino entry -------
void setup() {
  // Drive every output LOW first so the motors cannot twitch at power-up.
  const uint8_t outputs[] = { PIN_ENA, PIN_ENB, PIN_IN1, PIN_IN2, PIN_IN3, PIN_IN4, LED_BUILTIN };
  for (uint8_t i = 0; i < sizeof(outputs); i++) {
    pinMode(outputs[i], OUTPUT);
    digitalWrite(outputs[i], LOW);
  }
  pinMode(PIN_BT_STATE, INPUT);

  Serial.begin(USB_BAUD);
  bt.begin(BT_BAUD);

  stopCar();
  Serial.println(F("Arduino-Rover ready. Commands: F B L R G I H J S 0-9 q"));
}

void loop() {
  while (bt.available() > 0) {
    handleCommand((char)bt.read(), true);
  }
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c != '\n' && c != '\r' && c != ' ') handleCommand(c, false);
  }
  checkFailsafes();
}
