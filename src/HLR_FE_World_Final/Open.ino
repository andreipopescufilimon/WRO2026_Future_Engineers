// ============================================================================
// HYPERLINE ROBOTICS | WRO 2026 FUTURE ENGINEERS
// Open challenge | wall-triggered navigation and speed control
// ============================================================================

// Original Open round: one validated FRONT wall event per 90-degree corner.
// No side-wall or side-end logic is used.
static const int OPEN_CONFIRM_FRAMES = 3;
static const int OPEN_FRONT_TURN_MM = 700;  // Start turning after 3 fresh valid frames below 700 mm.
static const int OPEN_PAIR_MAX_DIFFERENCE_MM = 220;
static const float OPEN_REARM_STRAIGHT_CM = 25.0f;
static const uint32_t OPEN_REARM_MIN_MS = 600UL;
// Open round speed control: brake only when the verified front wall is
// under 1000 mm, otherwise accelerate toward normal straight speed.
// If both cells are invalid or disagree, slow down conservatively.
static const int OPEN_BRAKE_START_MM = 1000;
static const int OPEN_BRAKE_FULL_MM = 700;
static const int OPEN_APPROACH_PWM = 80;
static const int OPEN_ACCEL_PWM_PER_SEC = 300;
static const int OPEN_BRAKE_PWM_PER_SEC = 1900;
static const uint32_t OPEN_DISTANCE_MAX_AGE_MS = 150UL;
// Do not drive the old extra 10 cm after reaching the 700 mm trigger.
static const int OPEN_PRETURN_ADVANCE_CM = 0;
static int openRampedSpeed = OPEN_APPROACH_PWM;
static int openVerifiedFrontMM = -1;
static uint32_t openVerifiedFrontMs = 0;
static uint32_t openSpeedLastMs = 0;

// Calculate a distance-dependent PWM target. Interpolate between approach and
// cruise speed, using the slow target when range is unavailable.
static int open_speed_target(int maximumPWM, int frontMM) {
  const int maxSpeed = constrain(maximumPWM, 0, 255);
  const int slowSpeed = min(maxSpeed, OPEN_APPROACH_PWM);
  if (frontMM <= 0) return slowSpeed;  // Sensor unavailable, do not accelerate blindly.
  if (frontMM >= OPEN_BRAKE_START_MM) return maxSpeed;
  if (frontMM <= OPEN_BRAKE_FULL_MM) return slowSpeed;
  const int span = OPEN_BRAKE_START_MM - OPEN_BRAKE_FULL_MM;
  return slowSpeed + (int)((long)(maxSpeed - slowSpeed) *
                          (frontMM - OPEN_BRAKE_FULL_MM) / span);
}

// Rate-limit changes in PWM using elapsed milliseconds. Braking and
// acceleration have independent rates; cap the time step after a delayed
// iteration.
static int open_ramp_speed(int targetPWM) {
  const uint32_t now = millis();
  if (!openSpeedLastMs) openSpeedLastMs = now;
  const uint32_t dt = min((uint32_t)(now - openSpeedLastMs), (uint32_t)100UL);
  if (!dt) return openRampedSpeed;
  openSpeedLastMs = now;
  const int rate = targetPWM < openRampedSpeed ? OPEN_BRAKE_PWM_PER_SEC : OPEN_ACCEL_PWM_PER_SEC;
  const int step = max(1, (int)((long)rate * dt / 1000L));
  if (targetPWM < openRampedSpeed) openRampedSpeed = max(targetPWM, openRampedSpeed - step);
  else openRampedSpeed = min(targetPWM, openRampedSpeed + step);
  return openRampedSpeed;
}

static uint8_t openFrontNearFrames = 0;
static uint32_t openLastFrontFrameMs = 0;
static uint32_t openCornerFinishedMs = 0;
static bool openWaitingForStraight = false;

// Initialize the Open-round control gains, corner counter, speed ramp and
// fresh-frame tracking.
void open_configure() {
  max_turns = 12;
  robot_speed = 250;
  kp = 0.028;
  ki = 0;
  kd = 0.080;
  delayTurns = 1000;
  openCornerActive = false;
  openWaitingForStraight = false;
  openFrontNearFrames = 0;
  openLastFrontFrameMs = 0;
  openVerifiedFrontMM = -1;
  openVerifiedFrontMs = 0;
  openRampedSpeed = OPEN_APPROACH_PWM;
  openSpeedLastMs = 0;
  turn_count = 0;
}

// Latch the travel direction from the first accepted floor marker; subsequent
// samples cannot change that direction.
void open_color_poll() {
  if (turn_direction != 0) return;
  if (!tcs_read_raw_fast()) return;
  tcsCurrentColor = tcs_detect_color_fast();
  if (tcsCurrentColor == TCS_BLUE) turn_direction = -1;
  else if (tcsCurrentColor == TCS_ORANGE || tcsCurrentColor == TCS_RED)
    turn_direction = 1;
  if (turn_direction != 0) {
    tcsLatchedColor = tcsCurrentColor;
    if (debug) Serial.println(turn_direction == -1 ?
       "OPEN: first marker BLUE, turn LEFT" : "OPEN: first marker RED, turn RIGHT");
  }
}

// Run one Open mission iteration: refresh yaw and front ranging, validate new
// frames, re-arm corners after straight travel, then update steering and
// propulsion.
void open_execute() {
  if (!gyroAvailable) { move(0); steer(0); return; }
  if (turn_direction == 0) open_color_poll();
  read_gyro_data();
  robot_speed = (turn_count == 0) ? 80 : 250;

  // Only a fresh 4x4 frame is allowed to increment the close-wall counter.
  int a = -1, b = -1;
  const uint32_t previousFrame = openLastFrontFrameMs;
  const bool valid = tof_read_new_pair(FRONT_DIR, openLastFrontFrameMs, a, b);
  const bool newFrame = openLastFrontFrameMs != previousFrame;
  if (newFrame) {
    if (valid && abs(a - b) <= OPEN_PAIR_MAX_DIFFERENCE_MM) {
      // Use the closer of the two center-column zones to brake in time.
      // The disagreement check rejects obvious one-cell reflections.
      openVerifiedFrontMM = min(a, b);
      openVerifiedFrontMs = millis();
    } else {
      // An inconsistent or invalid frame must not authorize acceleration.
      openVerifiedFrontMM = -1;
      openVerifiedFrontMs = 0;
    }
  }

  if (openWaitingForStraight && !openCornerActive) {
    if ((uint32_t)(millis() - openCornerFinishedMs) >= OPEN_REARM_MIN_MS &&
        read_cm() >= OPEN_REARM_STRAIGHT_CM) {
      openWaitingForStraight = false;
      openFrontNearFrames = 0;
      if (debug) Serial.println("OPEN: next corner armed after straight travel");
    }
  }

  const bool canTurn = (turn_direction != 0 && turn_count < max_turns &&
                        currentState == PID && !openCornerActive &&
                        !openWaitingForStraight);
  if (newFrame) {
    if (canTurn && valid && a < OPEN_FRONT_TURN_MM && b < OPEN_FRONT_TURN_MM &&
        abs(a - b) <= OPEN_PAIR_MAX_DIFFERENCE_MM) {
      if (openFrontNearFrames < OPEN_CONFIRM_FRAMES) ++openFrontNearFrames;
    } else {
      openFrontNearFrames = 0;
    }
  }

  if (canTurn && openFrontNearFrames >= OPEN_CONFIRM_FRAMES) {
    openFrontNearFrames = 0;
    open_execute_command("TOF_FRONT");
  }

  if (turn_count >= max_turns && !openCornerActive) {
    move_straight_on_gyro_no_flush(OPEN_FINISH_SPEED, OPEN_FINISH_DURATION_MS);
    move(0);
    steeringServo.write(STEERING_CENTER);
    if (debug) Serial.println("OPEN: completed 12 corners");
    while (true) { delay(100); }
  }

  switch (currentState) {
    case PID: {
      const double err = current_angle_gyro - gz;
      pid_error = err * kp + (err - pid_last_error) * kd;
      pid_last_error = err;
      int commandSpeed = robot_speed;
      double steeringCommand = pid_error;

      if (openCornerActive) {
        if (fabs(err) <= OPEN_CORNER_DONE_ANGLE_DEG ||
            err * turn_direction <= 0.0) {
          openCornerActive = false;
          openWaitingForStraight = true;
          openCornerFinishedMs = millis();
          openFrontNearFrames = 0;
          openRampedSpeed = OPEN_CORNER_SPEED;
          openSpeedLastMs = millis();
          noInterrupts(); encoder_ticks = 0; interrupts();
          pid_error = 0;
          pid_last_error = 0;
          steeringCommand = 0.0;
          if (debug) {
            Serial.print("OPEN: corner "); Serial.print(turn_count);
            Serial.print(" settled gyro error="); Serial.println(err, 1);
          }
        } else {
          // Full steering lock during the square-side change. Ease off only
          // in the final few degrees to prevent overshoot of the gyro target.
          steeringCommand = fabs(err) > 9.0 ? (err > 0.0 ? 1.0 : -1.0)
                                             : constrain(0.10 * err, -1.0, 1.0);
          commandSpeed = OPEN_CORNER_SPEED;
        }
      }

      if (!openCornerActive) {
        const uint32_t now = millis();
        const bool distanceFresh = openVerifiedFrontMM > 0 && openVerifiedFrontMs != 0 &&
          (uint32_t)(now - openVerifiedFrontMs) <= OPEN_DISTANCE_MAX_AGE_MS;
        const int distanceMM = distanceFresh ? openVerifiedFrontMM : -1;
        const int targetPWM = open_speed_target(commandSpeed, distanceMM);
        commandSpeed = open_ramp_speed(targetPWM);
      } else {
        // Corners use their fixed controlled speed, never the straight ramp.
        openRampedSpeed = OPEN_CORNER_SPEED;
        openSpeedLastMs = millis();
      }
      if (turn_direction == 0 && tof_front_wall_close()) {
        commandSpeed = 0;  // First marker has not established the turn direction.
        openRampedSpeed = 0;
        openSpeedLastMs = millis();
        steeringCommand = 0;
      }
      steer(steeringCommand);
      move(commandSpeed);
      break;
    }
    default:
      break;
  }
}

// Handle incoming camera command text in Open mode according to the existing
// command parser.
void open_execute_command(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  if (cmd != "TOF_FRONT" || turn_direction == 0 ||
      openCornerActive || openWaitingForStraight || turn_count >= max_turns)
    return;

  ++turn_count;
  Serial.print("OPEN: verified front ToF corner ");
  Serial.print(turn_count);
  Serial.print(" gyro="); Serial.println(gz, 1);
  if (OPEN_PRETURN_ADVANCE_CM > 0 &&
      fabs(current_angle_gyro - gz) < 10.0 && currentState != AFTER_CUBE)
    move_cm_gyro(OPEN_PRETURN_ADVANCE_CM, OPEN_CORNER_SPEED, current_angle_gyro);

  read_gyro_data();
  current_angle_gyro += turn_direction * 90.0;
  openCornerActive = true;
  openFrontNearFrames = 0;
  pid_error = 0;
  pid_last_error = 0;
  lastTurnTime = millis();
  lastLineDetectedTime = 0;
}
