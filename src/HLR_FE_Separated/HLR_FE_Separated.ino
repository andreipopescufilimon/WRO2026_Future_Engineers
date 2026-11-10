#include <Wire.h>
#include <math.h>
#include <ESP32Encoder.h>
#include <ESP32Servo.h>
#include "BMI088.h"
#include <SparkFun_VL53L5CX_Library.h>

// Mode is latched on startup. D11 selects Final; D12 selects Open.
#define MODE_FINAL_PIN D11
#define MODE_OPEN_PIN D12
enum RoundMode : uint8_t { MODE_NONE, MODE_OPEN, MODE_FINAL };
RoundMode activeMode = MODE_NONE;


// ================ Pin Definitions ================
#define PWMA D7
#define DIRA D8

#define PWM_IMPELLER D9

#define START_BTN D10

#define P4 D0
#define P5 D1

#define DEBUG_LED1 D6
#define DEBUG_LED2 D3

// VL53L5CX LPn pins. All sensors share the primary I2C bus.
#define TOF_LPN_FRONT A3
#define TOF_LPN_LEFT  A2
#define TOF_LPN_RIGHT A1
#define TOF_LPN_BACK  A0

// ======= Distance sensor direction enum =======
enum DistanceDir : uint8_t { FRONT_DIR,
                             LEFT_DIR,  
                             RIGHT_DIR,
                             BACK_DIR };


// ================ Speed Control ================
int robot_speed = 100;  // 37 - Cubes ; 60-70 - Open
int exit_speed = 100;
int park_speed = 45;
int impeller_speed = 0;
int cube_last = 1;

bool tcsExecutingMarker = false;

// === Encoder pins ===
#define ENCODER_A D5
#define ENCODER_B D4
volatile int32_t encoder_ticks = 0;

#define PPR 12.0f  // 12 pulses per rev, single-edge
#define QUAD_EDGES (PPR * 4.0f)
#define GEAR_RATIO 1.0f
#define WHEEL_DIAM 29.0f  // mm
#define MM_PER_TICK 1.831f

// ================ Motors and PID ================3
double current_angle_gyro = 0.0;  // steering asymmetry
double kp = 0.044;
double ki = 0;
double kd = 0.082;
double pid_error = 0, pid_last_error = 0;

// Integration & drift
double gyro_last_read_time = 0;
double drifts_z = 0;  // drift in °/s
double gz = 0;        // integrated angle in degrees


#define PWM_MOTOR_CHANNEL 5
#define PWM_MOTOR_FREQ 1000
#define PWM_MOTOR_RESOLUTION 8  

#define PWM_IMPELLER_CHANNEL 4
#define PWM_IMPELLER_FREQ 1000
#define PWM_IMPELLER_RESOLUTION 8


#define STEERING_SERVO D2
Servo steeringServo;
#define STEERING_LEFT 39
#define STEERING_CENTER 85
#define STEERING_RIGHT 131

#define STEERING_LEFT_PARK 40
#define STEERING_RIGHT_PARK 138

#define CORRECTION_ANGLE 35

#define FOLLOW_CUBE_DEAD_TIME 350

// ================ UART Communication ================
#define RX_PIN D0
#define TX_PIN D1
String receivedMessage = "";
String command = "";
String current_color = "";

// ================ Turns count and direction ================
int turn_direction = 0;
int turn_count = 0;
int max_turns = 12;

// Open round: gyro-targeted turn, no small oscillatory yaw-rate steering corrections.
// After the first corner, straight-line speed is PWM 200.
bool openCornerActive = false;
const int OPEN_CORNER_SPEED = 80;
const double OPEN_CORNER_DONE_ANGLE_DEG = 1.0;
const int OPEN_FINISH_SPEED = 80; // Timed, gyro-stabilized finish
const unsigned long OPEN_FINISH_DURATION_MS = 1200UL;

unsigned long lastTurnTime = 0, lastLineDetectedTime = 0;

long last_follow_cube = 0;


// ================ Avoid Cubes Variables ================
float follow_cube_angle = 0;  // PID steering correction for cube following.

unsigned long last_cube_time = 0;  // remember when we last avoided one


// ================ Robot States ================
enum RobotState {
  PID,
  FOLLOW_CUBE,
  AVOID_CUBE,
  AFTER_CUBE,
  PARK
};

RobotState currentState = PID;
char cube_avoid_direction = 'R';  // 'R' for red cube avoidance, 'L' for green cube avoidance
float desiredSteering = 0.0;

// ================ Debug Mode ================
bool debug = false;
bool debuggyro = false;
bool debugcam = false;
bool firstcube = false;



// ================ TCS34725 Fast Color Sensor ================

#define TCS34725_ADDR 0x29

#define TCS_CMD 0x80
#define TCS_AUTO_INC 0x20

#define TCS_ENABLE 0x00
#define TCS_ATIME 0x01
#define TCS_CONTROL 0x0F
#define TCS_STATUS 0x13
#define TCS_CDATAL 0x14

#define TCS_PON 0x01
#define TCS_AEN 0x02

// Fastest real sample time is around 2.4 ms
#define TCS_ATIME_FAST 0xFF

// Gain:
// 0x00 = 1x
// 0x01 = 4x
// 0x02 = 16x
// 0x03 = 60x
#define TCS_GAIN_FAST 0x02

enum TCSColor : uint8_t {
  TCS_NONE,
  TCS_WHITE,
  TCS_BLUE,
  TCS_ORANGE,
  TCS_RED,
  TCS_GREEN,
  TCS_UNKNOWN
};

bool tcsReady = false;

uint16_t tcsC = 0;
uint16_t tcsR = 0;
uint16_t tcsG = 0;
uint16_t tcsB = 0;

TCSColor tcsCurrentColor = TCS_NONE;
TCSColor tcsLatchedColor = TCS_NONE;

unsigned long tcsLastReadMicros = 0;
unsigned long tcsLastMarkerMs = 0;
unsigned long tcsLastWhiteMs = 0;

bool tcsMarkerArmed = true;

// Tune these after testing
#define TCS_MARKER_LOCK_MS 10
#define TCS_REARM_WHITE_MS 2

// Minimum brightness. Increase if false detections happen.
#define TCS_MIN_CLEAR 10

void setup() {
  pinMode(MODE_FINAL_PIN, INPUT_PULLDOWN);
  pinMode(MODE_OPEN_PIN, INPUT_PULLDOWN);
  // Exactly one mode should be asserted. If both are HIGH, do not run.
  bool finalSelected = digitalRead(MODE_FINAL_PIN) == HIGH;
  bool openSelected = digitalRead(MODE_OPEN_PIN) == HIGH;
  if (finalSelected == openSelected) {
    pinMode(LED_BUILTIN, OUTPUT);
    while (true) { digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); delay(250); }
  }
  activeMode = finalSelected ? MODE_FINAL : MODE_OPEN;
  if (activeMode == MODE_FINAL) final_configure();
  else open_configure();

  steering_servo_setup();
  motor_driver_setup();
  impeller_setup();

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(DEBUG_LED1, OUTPUT);
  pinMode(DEBUG_LED2, OUTPUT);
  tof_prepare_pins();
  encoder_setup();
  Serial.begin(115200);
  Serial0.begin(19200);
  while (!Serial0) ;
  blink_led(DEBUG_LED1, 200);
  gyro_setup();
  tcs_setup_fast();
  tof_setup();
  blink_led(DEBUG_LED2, 200);
  digitalWrite(DEBUG_LED2, HIGH);
  pinMode(START_BTN, INPUT_PULLUP);
  while (digitalRead(START_BTN) == 1) { move(0); flush_messages(); }
  digitalWrite(DEBUG_LED1, HIGH);
  custom_delay(500);
  if (activeMode == MODE_FINAL) final_exit_parking();
}

void loop() {
  if (activeMode == MODE_OPEN) open_execute();
  else if (activeMode == MODE_FINAL) final_execute();
}
