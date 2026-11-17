// Open round: one validated front-wall event per 90-degree corner.
static const int OPEN_CONFIRM_FRAMES = 3;
static const int OPEN_FRONT_TURN_MM = 600;
static const int OPEN_PAIR_MAX_DIFFERENCE_MM = 220;
static const float OPEN_REARM_STRAIGHT_CM = 25.0f;
static const uint32_t OPEN_REARM_MIN_MS = 600UL;

static uint8_t openFrontNearFrames = 0;
static uint32_t openLastFrontFrameMs = 0;
static uint32_t openCornerFinishedMs = 0;
static bool openWaitingForStraight = false;

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
  turn_count = 0;
}

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
          noInterrupts(); encoder_ticks = 0; interrupts();
          pid_error = 0;
          pid_last_error = 0;
          steeringCommand = 0.0;
          if (debug) {
            Serial.print("OPEN: corner "); Serial.print(turn_count);
            Serial.print(" settled gyro error="); Serial.println(err, 1);
          }
        } else {
          steeringCommand = constrain(0.055 * err, -1.0, 1.0);
          commandSpeed = OPEN_CORNER_SPEED;
        }
      }

      if (!openCornerActive) commandSpeed = tof_open_round_speed(commandSpeed);
      if (turn_direction == 0 && tof_front_wall_close()) {
        commandSpeed = 0;  // First marker has not established the turn direction.
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
  if (fabs(current_angle_gyro - gz) < 10.0 && currentState != AFTER_CUBE)
    move_cm_gyro(10, OPEN_CORNER_SPEED, current_angle_gyro);

  read_gyro_data();
  current_angle_gyro += turn_direction * 90.0;
  openCornerActive = true;
  openFrontNearFrames = 0;
  pid_error = 0;
  pid_last_error = 0;
  lastTurnTime = millis();
  lastLineDetectedTime = 0;
}
