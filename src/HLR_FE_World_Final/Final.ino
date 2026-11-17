// Helper APIs are defined in tof.ino.
extern double gyroRateDps;
extern bool gyroAvailable;
bool tof_read_new_pair(DistanceDir dir, uint32_t &lastSeenFrameMs, int &a, int &b);
bool tof_read_new_zone(DistanceDir dir, uint32_t &lastSeenFrameMs,
                       uint8_t row, uint8_t col, int &mm);

bool finalExecutingTurn = false;
bool finalAlignmentFault = false;
bool finalPendingTofCorner = false;
bool finalSkipCornerAdvance = false;
bool finalTofHold = false;
uint32_t finalPendingTofSince = 0;
static uint8_t finalFrontCloseFrames = 0;
static uint32_t finalFrontLastFrameMs = 0;
static const int FINAL_CORNER_TOF_MM = 650;
static const int FINAL_AFTER_CUBE_CORNER_CM = 10;
static const int FINAL_AFTER_CUBE_CLEAR_CORNER_CM = 30;
static const uint32_t FINAL_CUBE_VISIBLE_RECENT_MS = 450UL;
static uint32_t finalLastCubeCameraMs = 0;
// Final only. No camera frames are discarded while avoiding a cube.
enum FinalCubePhase : uint8_t {
  FC_IDLE, FC_TURN_OUT, FC_DRIVE_OUT, FC_RECOVER_TURN, FC_RECOVER_FORWARD
};
FinalCubePhase finalCubePhase = FC_IDLE;
double finalCubeTarget = 0.0;
char finalCubeColor = 0;
char finalNextCloseColor = 0;
uint8_t finalNextCloseCount = 0;
uint8_t finalNextFollowCount = 0;
uint32_t finalCubePhaseMs = 0;
uint32_t finalCubeLastProgressMs = 0;
uint32_t finalCubeLastFollowMs = 0;
// Watch for a new follow target during the LAST part of the outward bypass.
// The early frames may still belong to the cube currently being passed.
uint8_t finalPassFollowCount = 0;
uint32_t finalPassFollowMs = 0;
float finalPassFollowSteering = 0.0f;
uint32_t finalCubeYawProgressMs = 0;
double finalCubeBestYawError = 0.0;
float finalCubeProgressCm = 0.0f;
bool finalDeferredCorner = false;
// Short hard-steer phase after a confirmed floor-marker corner when we are
// NOT already following a cube. This faces the OpenMV camera onto the next
// square side quickly, without smoothing or a prolonged low-speed corner.
bool finalRapidCornerActive = false;
uint32_t finalRapidCornerStartMs = 0;

void final_cube_phase(uint8_t phase) {
  finalCubePhase = (FinalCubePhase)phase;
  finalCubePhaseMs = millis();
  finalCubeLastProgressMs = millis();
  finalCubeProgressCm = 0.0f;
  finalCubeBestYawError = fabs(finalCubeTarget - gz);
  finalCubeYawProgressMs = millis();
  pid_error = pid_last_error = 0;
  if (phase == FC_DRIVE_OUT || phase == FC_RECOVER_FORWARD) {
    noInterrupts(); encoder_ticks = 0; interrupts();
  }
  if (phase == FC_RECOVER_TURN) {
    // An S command on the second cube can now interrupt recovery.
    finalNextFollowCount = 0;
    finalNextCloseCount = 0;
  }
}

void final_begin_avoid(char color) {
  read_gyro_data();
  finalCubeColor = color;
  cube_avoid_direction = color == 'R' ? 'L' : 'R';
  cube_last = color == 'R' ? -1 : 1;
  // Same +/- 50 degrees as the original pass_cube() function.
  finalCubeTarget = gz - cube_last * 50.0;
  finalNextCloseColor = 0;
  finalNextCloseCount = finalNextFollowCount = 0;
  finalPassFollowCount = 0;
  finalPassFollowMs = 0;
  currentState = AVOID_CUBE;
  last_cube_time = millis();
  final_cube_phase(FC_TURN_OUT);
  if (debug) {
    Serial.print("FINAL CUBE new "); Serial.print(color);
    Serial.print(" target="); Serial.println(finalCubeTarget, 1);
  }
}

void final_cube_drive_heading(int pwm, double target) {
  read_gyro_data();
  const double error = target - gz;
  const double cmd = constrain(error * 0.060, -0.78, 0.78);
  steer(cmd);
  move(pwm);
}

void final_cube_control_tick() {
  if (finalCubePhase == FC_IDLE) {
    // No stale avoid state may accidentally command the motors.
    currentState = PID;
    return;
  }
  read_gyro_data();
  if (!gyroAvailable || millis() - finalCubePhaseMs > 12000UL) {
    move(0); steer(0);
    finalAlignmentFault = true;
    currentState = PARK;
    Serial.println("FINAL CUBE STOP: maneuver timed out or gyro unavailable");
    return;
  }

  if (finalCubePhase == FC_TURN_OUT || finalCubePhase == FC_RECOVER_TURN) {
    double err = finalCubeTarget - gz;
    // Original cube avoidance accepted a 10-degree yaw tolerance.
    if (fabs(err) <= 9.0) {
      if (finalCubePhase == FC_TURN_OUT) {
        final_cube_phase(FC_DRIVE_OUT);
      } else {
        final_cube_phase(FC_RECOVER_FORWARD);
      }
      return;
    }
    if (fabs(err) < finalCubeBestYawError - 0.7) {
      finalCubeBestYawError = fabs(err);
      finalCubeYawProgressMs = millis();
    }
    if (millis() - finalCubeYawProgressMs > 2700UL) {
      move(0); steer(0);
      finalAlignmentFault = true;
      currentState = PARK;
      Serial.println("FINAL CUBE STOP: no yaw progress while avoiding");
      return;
    }
    const int pwm = fabs(err) < 17.0 ? robot_speed * 8 / 10 : robot_speed;
    final_cube_drive_heading(constrain(pwm, 48, 100), finalCubeTarget);
    return;
  }

  const float cm = read_cm();
  if (cm > finalCubeProgressCm + 0.15f) {
    finalCubeProgressCm = cm;
    finalCubeLastProgressMs = millis();
  }
  if (millis() - finalCubeLastProgressMs > 2500UL) {
    move(0); steer(0);
    finalAlignmentFault = true;
    currentState = PARK;
    Serial.println("FINAL CUBE STOP: wheel encoder not progressing");
    return;
  }

  if (finalCubePhase == FC_DRIVE_OUT && cm >= 13.0f) {
    // A second cube seen in at least two recent camera frames can take over
    // once the original outward bypass is safely completed.
    if (finalPassFollowCount >= 2 && finalPassFollowMs != 0 &&
        (uint32_t)(millis() - finalPassFollowMs) <= 230UL) {
      finalCubePhase = FC_IDLE;
      currentState = FOLLOW_CUBE;
      follow_cube_angle = finalPassFollowSteering;
      desiredSteering = follow_cube_angle;
      last_follow_cube = millis();
      last_cube_time = millis();
      pid_error = pid_last_error = 0;
      telemetry_note_event("second cube found during bypass");
      return;
    }
    currentState = AFTER_CUBE;
    finalCubeTarget = current_angle_gyro + cube_last * CORRECTION_ANGLE;
    final_cube_phase(FC_RECOVER_TURN);
    if (debug) Serial.println("FINAL CUBE bypass finished, recovery now camera-aware");
    return;
  }
  if (finalCubePhase == FC_RECOVER_FORWARD && cm >= 19.0f) {
    finalCubePhase = FC_IDLE;
    currentState = PID;
    pid_error = pid_last_error = 0;
    last_cube_time = millis();
    return;
  }
  final_cube_drive_heading(robot_speed, finalCubeTarget);
}

// Confirm a NEW cube while the previous one is being passed. Repeated close
// messages for the same sign are ignored until a following signal or a different
// sign establishes a new target. Close reports can be early in the second pass.
bool final_cube_new_close(char color) {
  if (finalCubePhase == FC_IDLE) return true;
  if (finalCubePhase == FC_TURN_OUT) return false;
  if (finalCubePhase == FC_DRIVE_OUT && read_cm() < 7.0f) return false;
  // Two signs may share the same color. Without a follow-frame separation,
  // permit a new same-color target only once we've driven past the first sign.
  const bool farEnoughForSameColor =
      finalCubePhase == FC_RECOVER_FORWARD && read_cm() >= 7.0f &&
      (uint32_t)(millis() - last_cube_time) >= 650UL;
  const bool sameColorWithoutFollow = color == finalCubeColor &&
                                     finalNextFollowCount < 2;
  if (sameColorWithoutFollow && !farEnoughForSameColor) return false;
  if (finalNextCloseColor != color) {
    finalNextCloseColor = color;
    finalNextCloseCount = 0;
  }
  if (finalNextCloseCount < 3) ++finalNextCloseCount;
  if (finalNextCloseCount < (sameColorWithoutFollow ? 3 : 2)) return false;
  return true;
}

// Parking targets use the fixed lap heading grid, not the arrival offset.

void final_configure() {
  max_turns = 13;
  robot_speed = 52;
  exit_speed = 60;
  // Gentler normal square-side steering, still direct and without KI/smoothing.
  kp = 0.038;
  ki = 0;
  kd = 0.070;
  delayTurns = 1500;
  currentState = PID;
  finalFrontCloseFrames = 0;
  finalFrontLastFrameMs = 0;
  finalLastCubeCameraMs = 0;
  finalCubePhase = FC_IDLE;
  finalDeferredCorner = false;
  finalRapidCornerActive = false;
}

void final_exit_parking() {
  float leftMM = -1.0f;
  float rightMM = -1.0f;
  const uint32_t beginMs = millis();
  while ((uint32_t)(millis() - beginMs) < 1500UL) {
    leftMM = readDistanceMM(LEFT_DIR);
    rightMM = readDistanceMM(RIGHT_DIR);
    if (leftMM > 0 && rightMM > 0) break;
    delay(15);
  }
  if (leftMM <= 0 || rightMM <= 0) {
    move(0);
    steer(0);
    finalAlignmentFault = true;
    currentState = PARK;
    Serial.println("FINAL: exit aborted: left/right ToF pair unavailable");
    return;
  }

  if (leftMM < rightMM) {
    move_until_angle_max(exit_speed, 100);
    move_cm_gyro(10, exit_speed, 100);
    turn_direction = 1;
    move_until_angle_max(exit_speed, 0);
    steeringServo.write(STEERING_CENTER);
    move_cm_gyro(25, -exit_speed, current_angle_gyro);
    current_angle_gyro = 0.0;
  } else {
    move_until_angle_max(exit_speed, -100);
    move_cm_gyro(10, exit_speed, -100);
    turn_direction = -1;
    move_until_angle_max(exit_speed, 0);
    steeringServo.write(STEERING_CENTER);
    move_cm_gyro(60, -exit_speed, current_angle_gyro);
    current_angle_gyro = 0.0;
  }
  pid_error = 0;
  pid_last_error = 0;
  tcsMarkerArmed = true;
  tcsLatchedColor = TCS_NONE;
}
void final_execute() {
  if (!gyroAvailable) { move(0); steer(0); return; }
  if (finalAlignmentFault) {
    move(0);
    return;
  }
  final_color_poll();
  read_gyro_data();

  if (finalPendingTofCorner && turn_count < max_turns - 1) {
    if (currentState == AFTER_CUBE) {
      finalPendingTofCorner = false;
      finalFrontCloseFrames = 0;
      finalTofHold = false;
      finalSkipCornerAdvance = false;
      final_perform_line_turn();
    } else if (currentState == FOLLOW_CUBE) {
      // The camera takes priority. Update the corner heading without moving ahead.
      finalPendingTofCorner = false;
      finalFrontCloseFrames = 0;
      finalSkipCornerAdvance = true;
      final_perform_line_turn();
      finalSkipCornerAdvance = false;
    } else if (currentState == PID) {
      int freshA = -1, freshB = -1;
      const uint32_t previousFrame = finalFrontLastFrameMs;
      const bool validNewPair = tof_read_new_pair(
          FRONT_DIR, finalFrontLastFrameMs, freshA, freshB);
      if (finalFrontLastFrameMs != previousFrame) {
        if (validNewPair && freshA < FINAL_CORNER_TOF_MM && freshB < FINAL_CORNER_TOF_MM) {
          if (finalFrontCloseFrames < 2) ++finalFrontCloseFrames;
        } else {
          finalFrontCloseFrames = 0;
        }
      }
      const int a = tof_zone_mm(FRONT_DIR, 2, 3);
      const int b = tof_zone_mm(FRONT_DIR, 3, 3);
      finalTofHold = (a <= 0 || b <= 0) &&
                     (uint32_t)(millis() - finalPendingTofSince) > 500UL;
      if (finalFrontCloseFrames >= 2 && a > 0 && b > 0) {
        finalPendingTofCorner = false;
        finalFrontCloseFrames = 0;
        finalTofHold = false;
        // The front wall is now at 650 mm. Change heading immediately,
        // without another 13 cm or 350 mm wall approach.
        finalSkipCornerAdvance = true;
        final_perform_line_turn();
        finalSkipCornerAdvance = false;
      }
    }
  } else {
    finalFrontCloseFrames = 0;
    finalTofHold = false;
  }
  if (turn_count >= max_turns) currentState = PARK;

  // Process the latest UART camera messages BEFORE computing a motor command.
  // Never discard unread camera frames in Final round.
  uint8_t cameraFrames = 0;
  while (Serial0.available() > 0 && cameraFrames < 20) {
    const char c = (char)Serial0.read();
    if (c == '\r') continue;
    if (c == '\n') {
      String cmd = receivedMessage;
      receivedMessage = "";
      if (cmd.length()) final_execute_command(cmd);
      ++cameraFrames;
    } else if (receivedMessage.length() < 64) {
      receivedMessage += c;
    } else {
      receivedMessage = "";
    }
  }

  switch (currentState) {
    case PID: {
      const double err = current_angle_gyro - gz;
      // Get the camera facing the next side earlier after a floor-marker turn.
      // Only normal PID corners use this; FOLLOW_CUBE / AVOID_CUBE retain
      // their own steering and priority. Front-wall marker gate is unchanged.
      if (finalRapidCornerActive) {
        const bool recent = (uint32_t)(millis() - finalRapidCornerStartMs) < 1050UL;
        if (recent && fabs(err) > 52.0) {
          // Short early corner assist, never full steering lock or a PWM boost.
          steer(err > 0.0 ? 0.63 : -0.63);
          move(finalTofHold ? 0 : max(38, robot_speed - 7));
          pid_last_error = err; // avoid derivative kick on returning to PD
          break;
        }
        finalRapidCornerActive = false;
        pid_last_error = err;
      }
      // Direct PD steering, no smoothing and no integral term.
      const double derivative = err - pid_last_error;
      pid_last_error = err;
      const float demand = constrain(err * kp + derivative * kd, -0.68, 0.68);
      pid_error = demand;
      steer(demand);
      // Just a little slower while changing square sides. No smoothing.
      const int cornerPwm = fabs(err) > 20.0 ? max(38, robot_speed - 7) : robot_speed;
      move(finalTofHold ? 0 : cornerPwm);
      break;
    }

    case FOLLOW_CUBE: {
      if (millis() - last_follow_cube > FOLLOW_CUBE_DEAD_TIME) {
        currentState = PID;
        pid_last_error = current_angle_gyro - gz;
        break;
      }
      steer(follow_cube_angle);
      move(robot_speed);
      break;
    }

    case AVOID_CUBE:
    case AFTER_CUBE: {
      final_cube_control_tick();
      break;
    }

    case PARK: {
      move(0);
      break;
    }
  }
  // A marker encountered while bypassing a cube is applied once the
  // bypass is complete, so it cannot alter the first cube's escape heading.
  if (finalDeferredCorner && currentState == AFTER_CUBE &&
      finalCubePhase == FC_RECOVER_TURN && !finalExecutingTurn) {
    finalDeferredCorner = false;
    final_perform_line_turn();
  }
}

void final_color_poll() {
  tcs_update_fast();
  if (!tcsExecutingMarker && !finalExecutingTurn && !finalAlignmentFault)
    tcs_process_marker();
}

bool tcs_is_expected_marker_detected() {
  if (turn_direction == 0)
    return tcsCurrentColor == TCS_BLUE || tcsCurrentColor == TCS_RED;
  if (turn_direction == -1) return tcsCurrentColor == TCS_BLUE;
  if (turn_direction == 1) return tcsCurrentColor == TCS_RED;
  return false;
}

void tcs_update_fast() {
  if (!tcs_read_raw_fast()) return;
  TCSColor color = tcs_detect_color_fast();
  tcsCurrentColor = color;

  if (color == TCS_WHITE) {
    tcsLastWhiteMs = millis();
    if (millis() - tcsLastMarkerMs > TCS_REARM_WHITE_MS)
      tcsMarkerArmed = true;
  }

  if (turn_direction == 0) {
    if (color == TCS_BLUE || color == TCS_RED) tcsLatchedColor = color;
  } else if (turn_direction == -1 && color == TCS_BLUE) {
    tcsLatchedColor = color;
  } else if (turn_direction == 1 && color == TCS_RED) {
    tcsLatchedColor = color;
  }
}

void tcs_process_marker() {
  if (!tcsMarkerArmed) {
    tcsLatchedColor = TCS_NONE;
    return;
  }
  if (millis() - tcsLastMarkerMs < TCS_MARKER_LOCK_MS) {
    tcsLatchedColor = TCS_NONE;
    return;
  }
  if (tcsLatchedColor == TCS_NONE) return;
  if (turn_direction == 0) {
    if (tcsLatchedColor == TCS_BLUE) turn_direction = -1;
    else if (tcsLatchedColor == TCS_RED) turn_direction = 1;
    else { tcsLatchedColor = TCS_NONE; return; }
  }
  if (turn_direction == -1 && tcsLatchedColor != TCS_BLUE) {
    tcsLatchedColor = TCS_NONE;
    return;
  }
  if (turn_direction == 1 && tcsLatchedColor != TCS_RED) {
    tcsLatchedColor = TCS_NONE;
    return;
  }
  tcsMarkerArmed = false;
  tcsLastMarkerMs = millis();
  tcsExecutingMarker = true;
  final_execute_command("TCS_MARKER");
  tcsExecutingMarker = false;
  tcsLatchedColor = TCS_NONE;
}

// Process camera frames during the otherwise blocking AFTER_CUBE corner advance.
// A cube-follow S command interrupts the encoder move immediately.
bool final_camera_interrupt_corner_advance() {
  while (Serial0.available() > 0) {
    const char c = (char)Serial0.read();
    if (c == '\r') continue;
    if (c != '\n') {
      if (receivedMessage.length() < 64) receivedMessage += c;
      else receivedMessage = "";
      continue;
    }

    String cmd = receivedMessage;
    receivedMessage = "";
    cmd.trim();
    cmd.toUpperCase();
    if (cmd.length() == 0) continue;
    telemetry_note_camera(cmd);

    if (cmd.startsWith("S") && cmd.length() > 1) {
      finalLastCubeCameraMs = millis();
      desiredSteering = cmd.substring(1).toFloat();
      follow_cube_angle = desiredSteering;
      last_follow_cube = millis();
      finalCubePhase = FC_IDLE;
      currentState = FOLLOW_CUBE;
      Serial.println("FINAL: cube seen during corner advance; following now");
      return true;
    }

    char cube = cmd.charAt(0);
    if (cmd.indexOf("RED") != -1) cube = 'R';
    else if (cmd.indexOf("GREEN") != -1) cube = 'G';
    if (cube == 'R' || cube == 'G') {
      final_begin_avoid(cube);
      Serial.println("FINAL: close cube during corner advance; avoiding now");
      return true;
    }
  }
  return false;
}

static const uint8_t FINAL_ADVANCE_COMPLETE = 0;
static const uint8_t FINAL_ADVANCE_CUBE = 1;
static const uint8_t FINAL_ADVANCE_FAULT = 2;

// Hold the OLD square-side heading until the distance is reached or a cube appears.
// The 30 cm variant may stop earlier at two fresh 650 mm front-ToF frames.
uint8_t final_after_cube_corner_advance(double heading,
                                                    int maxCm,
                                                    bool useFrontTof) {
  noInterrupts(); encoder_ticks = 0; interrupts();
  uint32_t lastFrontFrame = 0;
  uint32_t lastProgressMs = millis();
  const uint32_t startMs = lastProgressMs;
  float progressedCm = 0.0f;
  double prevError = 0.0;
  uint8_t nearFrames = 0;
  bool reachedWall = false;

  while (read_cm() < (float)maxCm) {
    // Do not call flush_messages() here: it discards camera commands.
    if (final_camera_interrupt_corner_advance()) {
      move(0);
      return FINAL_ADVANCE_CUBE;
    }

    read_gyro_data();
    const double error = heading - gz;
    const double steering = constrain(error * kp + (error - prevError) * kd,
                                      -0.60, 0.60);
    prevError = error;

    if (useFrontTof) {
      int a = -1, b = -1;
      const uint32_t previousFrame = lastFrontFrame;
      const bool validPair = tof_read_new_pair(FRONT_DIR, lastFrontFrame, a, b);
      if (lastFrontFrame != previousFrame) {
        nearFrames = (validPair && a <= FINAL_CORNER_TOF_MM && b <= FINAL_CORNER_TOF_MM)
                       ? (uint8_t)(nearFrames + 1) : 0;
        if (nearFrames >= 2) { reachedWall = true; break; }
      }
    }

    if (final_camera_interrupt_corner_advance()) {
      move(0);
      return FINAL_ADVANCE_CUBE;
    }

    const float cm = read_cm();
    if (cm > progressedCm + 0.15f) {
      progressedCm = cm;
      lastProgressMs = millis();
    }
    if ((uint32_t)(millis() - lastProgressMs) > 2500UL ||
        (uint32_t)(millis() - startMs) > 30000UL || !gyroAvailable) {
      move(0);
      steer(0);
      finalAlignmentFault = true;
      Serial.println("FINAL: AFTER_CUBE advance: encoder/gyro timeout");
      return FINAL_ADVANCE_FAULT;
    }
    steer(steering);
    move(robot_speed);
    delay(4);
  }
  move(0);
  steer(0);
  pid_error = 0;
  pid_last_error = 0;
  if (debug) {
    Serial.print("FINAL AFTER_CUBE advance cm="); Serial.print(read_cm(), 1);
    Serial.println(reachedWall ? " front <=650mm" : " distance complete");
  }
  return FINAL_ADVANCE_COMPLETE;
}

void final_perform_line_turn() {
  if (finalExecutingTurn || turn_count >= max_turns || finalAlignmentFault) return;
  const bool parkingMarker = (turn_count == max_turns - 1);
  if (!parkingMarker &&
      (uint32_t)(millis() - lastTurnTime) <= (uint32_t)delayTurns) return;

  finalExecutingTurn = true;
  turn_count++;

  if (turn_count == 13) {
    currentState = PARK;
    final_park();
    lastTurnTime = millis();
    lastLineDetectedTime = 0;
    finalExecutingTurn = false;
    return;
  }
  if (debug) Serial.println("FINAL: color-marker corner");

  const double oldSideHeading = current_angle_gyro;
  const double nextSideHeading = oldSideHeading + turn_direction * 90.0;
  if (!finalSkipCornerAdvance) {
    if (currentState == AFTER_CUBE) {
      // A corner was confirmed. Keep its old-side advance interruptible.
      finalCubePhase = FC_IDLE;
      // Preserve the old heading until the advance is finished.
      const bool cubeStillVisible = finalLastCubeCameraMs != 0 &&
          (uint32_t)(millis() - finalLastCubeCameraMs) <= FINAL_CUBE_VISIBLE_RECENT_MS;
      const uint8_t advance = final_after_cube_corner_advance(
          oldSideHeading,
          cubeStillVisible ? FINAL_AFTER_CUBE_CORNER_CM : FINAL_AFTER_CUBE_CLEAR_CORNER_CM,
          !cubeStillVisible);
      if (advance == FINAL_ADVANCE_FAULT) {
        finalExecutingTurn = false;
        currentState = PARK;
        return;
      }
      if (advance == FINAL_ADVANCE_COMPLETE) currentState = PID;
      // If a cube interrupted the advance, preserve FOLLOW_CUBE / AVOID_CUBE.
      // The new square-side target is still committed after the color marker.
    } else if (currentState == PID) {
      // Marker-triggered PID corners normally wait for front R2C3/R3C3
      // to be below 650 mm. No second approach after this event.
    } else if (currentState == FOLLOW_CUBE &&
               fabs(oldSideHeading - gz) < 10.0) {
      // The former move_cm_gyro(8) discarded OpenMV messages. Keep listening
      // throughout the approach, particularly just after the 90-degree corner.
      const uint8_t advance = final_after_cube_corner_advance(
          oldSideHeading, 8, false);
      if (advance == FINAL_ADVANCE_FAULT) {
        finalExecutingTurn = false;
        currentState = PARK;
        return;
      }
      // A camera interruption preserves FOLLOW_CUBE/AVOID_CUBE, as above.
    }
  }
  if (finalAlignmentFault) {
    finalExecutingTurn = false;
    currentState = PARK;
    return;
  }

  // Only now tell the gyro PID to enter the NEW square side.
  // Hard-steer briefly ONLY when the camera is not following another cube.
  finalRapidCornerActive = currentState == PID;
  finalRapidCornerStartMs = millis();
  current_angle_gyro = nextSideHeading;
  pid_error = 0;
  pid_last_error = 0;
  delayTurns = 1500;
  lastTurnTime = millis();
  lastLineDetectedTime = 0;
  finalExecutingTurn = false;
}

// Full parking enabled: continue after first turn and front 320 mm alignment.
#define FINAL_PARK_STOP_AFTER_FRONT_320 0

void final_park() {
  const int parkDir = turn_direction < 0 ? 1 : -1;
  const int normalDir = -parkDir;
  const DistanceDir sideSensor = parkDir > 0 ? LEFT_DIR : RIGHT_DIR;
  const double TURN_DEG = 90.0;
  // The exit resets the 3-lap heading grid to zero; the 12 normal corners
  // advance by +/-90 deg, and the first parking turn is opposite. Thus its
  // absolute target is +/-990 deg, NOT (measured-entry-yaw +/- 90).
  // Otherwise a 32 deg arrival error silently becomes a 32 deg target offset.
  const double fixedFirstTarget = (double)turn_direction *
                                  (double)(max_turns - 2) * TURN_DEG;
  const float FRONT_TARGET_MM = 320.0f;
  // Side-wall detection is based on relative distance EDGES, not a fixed
  // wall-near distance. Two complete far -> near transitions are required.
  const float SIDE_EDGE_MIN_DROP_MM = 65.0f;
  const float SIDE_EDGE_DROP_RATIO = 0.25f;
  const float SIDE_EDGE_MIN_RISE_MM = 65.0f;
  const int PARK_DRIVE_PWM = 52;
  const double PARK_YAW_TOL_DEG = 0.35;
  bool finalYawCorrection = false;

  auto fail = [&](const char *reason) {
    move(0);
    steer(0);
    finalAlignmentFault = true;
    currentState = PARK;
    telemetry_parking_fail(reason);
    Serial.print("FINAL PARK STOP: "); Serial.print(reason);
    Serial.print(" gyro="); Serial.println(gz, 2);
  };

  // Only PARKING distance moves use the gentle heading hold below. The old
  // 0.18 gain with +/-0.90 lock flipped the servo fully left/right at the
  // 320 mm approach. Limit both gain and command changes while moving.
  float parkingHoldSteer = 0.0f;
  auto holdHeading = [&](int pwm, double heading) {
    read_gyro_data();
    const double direction = (pwm < 0) ? -1.0 : 1.0;
    const double error = heading - gz;
    // Rate damping prevents steering into a turn that has already started.
    const double requested =
      (fabs(error) < 0.25 && fabs(gyroRateDps) < 1.0) ? 0.0 :
      constrain(direction * (error * 0.065 - gyroRateDps * 0.010), -0.19, 0.19);
    const float delta = constrain((float)requested - parkingHoldSteer,
                                  -0.020f, 0.020f);
    parkingHoldSteer = constrain(parkingHoldSteer + delta, -0.19f, 0.19f);
    steer(parkingHoldSteer);
    move(pwm);
  };

  auto settle = [&](uint32_t durationMs) {
    move(0);
    steer(0);
    const uint32_t start = millis();
    while ((uint32_t)(millis() - start) < durationMs) {
      read_gyro_data();
      delay(4);
    }
    read_gyro_data();
  };

  // Parking turns are forward Ackermann arcs. If a turn undershoots OR
  // overshoots, steer toward the remaining error and KEEP MOVING FORWARD.
  // Never fake the gyro heading or accept a 10-20 degree residual error.
  auto turnTo = [&](double target, const char *stage) -> bool {
    telemetry_parking_stage(stage, target);
    settle(90);
    const double entry = gz;
    const uint32_t started = millis();
    uint32_t lastLog = started;
    uint32_t lastImprovement = started;
    uint32_t lastFrontCheck = 0;
    int32_t lastTicks = encoder_ticks;
    float traveledMM = 0.0f;
    double previousRemaining = fabs(target - entry);
    int previousSign = (target - entry >= 0.0) ? 1 : -1;
    int stableSamples = 0;
    float turnSteer = 0.0f;

    // A 90-degree Ackermann arc may exceed 430 mm of wheel travel.
    // Short trim corrections also need real wheel motion, not an idle stop.
    const float maxTravelMM = finalYawCorrection ? 430.0f : 950.0f;
    const uint32_t maxTimeMs = finalYawCorrection ? 11000UL : 22000UL;
    Serial.print("FINAL PARK TURN START "); Serial.print(stage);
    Serial.print(" yaw="); Serial.print(gz, 2);
    Serial.print(" target="); Serial.println(target, 2);

    while ((uint32_t)(millis() - started) < maxTimeMs) {
      read_gyro_data();
      if (!gyroAvailable) break;
      const double error = target - gz;
      const double remaining = fabs(error);
      if (!isfinite(remaining) || remaining > 230.0) break;

      const int32_t nowTicks = encoder_ticks;
      traveledMM += (float)abs(nowTicks - lastTicks) * FINAL_MM_PER_TICK;
      lastTicks = nowTicks;
      if (traveledMM > maxTravelMM) {
        Serial.println("FINAL PARK TURN: maximum correction distance reached");
        break;
      }

      // Stop to evaluate the REAL settled angle, then resume movement if
      // the residual error is still larger than the requested tolerance.
      if (remaining <= PARK_YAW_TOL_DEG ||
          (remaining < 4.0 &&
           gyroRateDps * error > 0 &&
           remaining < fabs(gyroRateDps) * 0.10)) {
        settle(100);
        if (fabs(target - gz) <= PARK_YAW_TOL_DEG &&
            fabs(gyroRateDps) <= 3.5) {
          if (++stableSamples >= 2) {
            pid_error = 0;
            pid_last_error = 0;
            Serial.print("FINAL PARK TURN COMPLETE "); Serial.print(stage);
            Serial.print(" actual="); Serial.print(gz, 3);
            Serial.print(" error="); Serial.println(target - gz, 3);
            return true;
          }
          delay(50);
          continue;
        }
        stableSamples = 0;
      }

      // Track local improvement, not the smallest error seen earlier.
      // An overshoot flips the error sign; it must be allowed to correct
      // back towards zero rather than timing out at the old minimum.
      const int errorSign = error >= 0.0 ? 1 : -1;
      if (errorSign != previousSign ||
          remaining + 0.15 < previousRemaining) {
        lastImprovement = millis();
      }
      previousSign = errorSign;
      previousRemaining = remaining;
      if ((uint32_t)(millis() - lastImprovement) > 3800UL) {
        Serial.println("FINAL PARK TURN: gyro not approaching target");
        break;
      }

      // Guard the front wall when straightening near the 320 mm position.
      // Unlike the old version, overshooting does not require a rear ToF
      // reading and does not command a reverse move that may never happen.
      if (finalYawCorrection &&
          (uint32_t)(millis() - lastFrontCheck) >= 80UL) {
        lastFrontCheck = millis();
        tof_poll_one((uint8_t)FRONT_DIR);
        const int r2 = tof_zone_mm(FRONT_DIR, 2, 3);
        const int r3 = tof_zone_mm(FRONT_DIR, 3, 3);
        if ((r2 > 0 && r2 < 180) || (r3 > 0 && r3 < 180)) {
          Serial.println("FINAL PARK TURN: not enough safe front clearance");
          break;
        }
      }

      // Positive gyro error => turn right; negative => turn left.
      // This works for both under- and overshoot with FORWARD motion.
      const int yawDir = error > 0.0 ? 1 : -1;
      const double lock = remaining > 18.0 ? 0.78 :
                          remaining > 4.0 ? 0.62 :
                          constrain(0.19 + remaining * 0.060, 0.19, 0.43);
      int pwm = remaining > 18.0 ? 74 :
                remaining > 4.0 ? 64 : 56;
      if ((uint32_t)(millis() - lastImprovement) > 800UL) {
        pwm = 83;  // Overcome low-speed motor friction if gyro is not moving.
      }
      // Servo must not snap from full left to full right after a tiny overshoot.
      const float requestedLock = (float)(yawDir * lock);
      turnSteer += constrain(requestedLock - turnSteer, -0.075f, 0.075f);
      steer_parking(turnSteer);
      move(pwm);

      if ((uint32_t)(millis() - lastLog) > 350UL) {
        lastLog = millis();
        Serial.print("FINAL PARK TURN "); Serial.print(stage);
        Serial.print(" gyro="); Serial.print(gz, 2);
        Serial.print(" target="); Serial.print(target, 2);
        Serial.print(" error="); Serial.print(error, 2);
        Serial.print(" pwm="); Serial.println(pwm);
      }
      delay(5);
    }

    settle(170);
    Serial.print("FINAL PARK TURN FAILED "); Serial.print(stage);
    Serial.print(" gyro="); Serial.print(gz, 3);
    Serial.print(" target="); Serial.print(target, 3);
    Serial.print(" error="); Serial.println(target - gz, 3);
    return false;
  };

  auto readFront = [&](uint32_t &lastMs, float &meanMM) -> bool {
    const uint32_t previous = lastMs;
    int r2 = -1;
    tof_read_new_zone(FRONT_DIR, lastMs, 2, 3, r2);
    if (lastMs == previous) return false;
    const int r3 = tof_zone_mm(FRONT_DIR, 3, 3);
    if (r2 <= 0 && r3 <= 0) return false;
    if (r2 > 0 && r3 > 0) {
      // When one row sees the floor/background, use the nearer valid return.
      // Three separate consistent frames are still required below.
      meanMM = (abs(r2 - r3) <= 210) ? 0.5f * (r2 + r3)
                                       : (float)((r2 < r3) ? r2 : r3);
    } else {
      meanMM = (float)((r2 > 0) ? r2 : r3);
    }
    return meanMM >= 80.0f && meanMM <= 3500.0f;
  };

  auto encoderMove = [&](float cm, int direction, double heading,
                         int pwm, uint32_t timeoutMs) -> bool {
    noInterrupts(); encoder_ticks = 0; interrupts();
    const uint32_t start = millis();
    uint32_t progressed = start;
    float lastCm = 0;
    while (read_cm() < cm) {
      if ((uint32_t)(millis() - start) > timeoutMs ||
          (uint32_t)(millis() - progressed) > 1800UL) {
        move(0);
        steer(0);
        return false;
      }
      const float travelled = read_cm();
      if (travelled > lastCm + 0.12f) {
        progressed = millis();
        lastCm = travelled;
      }
      holdHeading(direction * pwm, heading);
      delay(4);
    }
    settle(100);
    return true;
  };

  finalRapidCornerActive = false;
  telemetry_parking_stage("ENTRY", fixedFirstTarget);
  settle(150);
  // The old square-side target can differ from the real heading at marker 13.
  // Do not steer toward that old target during the 150 ms parking entry move.
  read_gyro_data();
  current_angle_gyro = gz;
  move_straight_on_gyro_no_flush(robot_speed, 150);
  // At startup, the branch with LEFT ToF < RIGHT ToF selects turn_direction=+1.
  // If we EXITED parking on that left-hand side, add a measured 250 mm before
  // the first parking turn. Hold our actual incoming straight-line heading.
  if (turn_direction == 1) {
    telemetry_parking_stage("LEFT EXIT +250MM", fixedFirstTarget);
    read_gyro_data();
    const double approachHeading = gz;
    tof_poll_one((uint8_t)FRONT_DIR);
    const int frontA = tof_zone_mm(FRONT_DIR, 2, 3);
    const int frontB = tof_zone_mm(FRONT_DIR, 3, 3);
    // Require the 250 mm move plus at least 160 mm of remaining clearance.
    if ((frontA > 0 && frontA < 410) || (frontB > 0 && frontB < 410)) {
      fail("front clearance too small for left-exit extra 250mm"); return;
    }
    Serial.println("FINAL PARK: +250 mm forward after LEFT-side exit");
    if (!encoderMove(25.0f, 1, approachHeading, max(49, robot_speed), 11000UL)) {
      fail("left-exit extra 250mm encoder move failed"); return;
    }
  }
  settle(180);
  // Start angle is measured for diagnostics; it must NOT define the target.
  // The fixed heading reference was established at departure and updated by
  // floor markers throughout three laps. We never reset the physical gyro.
  pid_error = 0;
  pid_last_error = 0;
  read_gyro_data();
  const double initialHeading = gz;
  const double firstTarget = fixedFirstTarget;
  Serial.print("FINAL PARK: measured entry yaw="); Serial.print(initialHeading, 2);
  Serial.print(" fixed first target="); Serial.print(firstTarget, 2);
  Serial.print(" start error="); Serial.println(firstTarget - initialHeading, 2);
  current_angle_gyro = firstTarget;
  if (!turnTo(firstTarget, "FIRST 90")) {
    fail("first 90 deg did not settle"); return;
  }
  // Hold the settled measured yaw during the encoder-based front alignment.
  read_gyro_data();
  current_angle_gyro = firstTarget;

  // GENTLE front-wall alignment only. Normal driving stays at 52 PWM.
  // Both forward and reverse use a gradual 38..44 PWM distance profile.
  // If the encoder shows no movement, increase PWM only enough to overcome
  // static friction (never above 46); otherwise stop rather than surge.
  telemetry_parking_stage("FRONT 320 GENTLE", firstTarget);
  static const float FRONT_SOFT_LIMIT_MM = 285.0f;
  static const float FRONT_DISTANCE_TOL_MM = 14.0f;
  static const int FRONT_PWM_FAR = 44;
  static const int FRONT_PWM_NEAR = 38;
  static const int FRONT_PWM_STALL_CAP = 46;
  float frontDistance = -1.0f;
  float previousFront = -1.0f;
  uint32_t frontFrame = 0;
  uint8_t frontPairs = 0;
  const uint32_t acquireStarted = millis();
  while (frontPairs < 1 &&
         (uint32_t)(millis() - acquireStarted) < 1700UL) {
    float mm = -1.0f;
    if (readFront(frontFrame, mm)) {
      if (previousFront > 0.0f && fabs(mm - previousFront) <= 75.0f) {
        frontDistance = 0.5f * (previousFront + mm);
        ++frontPairs;
      }
      previousFront = mm;
    }
    read_gyro_data();
    delay(4);
  }
  if (frontPairs == 0 || frontDistance <= 0.0f) {
    fail("no consistent front ToF frames for parking alignment"); return;
  }
  if (fabs(frontDistance - FRONT_TARGET_MM) > 1200.0f) {
    fail("front distance outside parking alignment range"); return;
  }

  auto rearSafe = [&](float travelMM) -> bool {
    // Check immediately before EACH backward movement, not just at entry.
    tof_poll_one((uint8_t)BACK_DIR);
    const int a = tof_zone_mm(BACK_DIR, 2, 3);
    const int b = tof_zone_mm(BACK_DIR, 3, 3);
    const float minClearance = travelMM + 140.0f;
    return a > minClearance && b > minClearance;
  };

  // A round-trip backing correction is used only when the front distance is
  // already right but the gyro is still off. This provides real Ackermann
  // movement for straightening instead of steering while standing still.
  bool yawBacking = false;
  float backingGoal = 0.0f;
  int yawBackCycles = 0;
  int changes = 0;
  int driveDir = 0;
  uint8_t withinFrames = 0;
  uint8_t badJumps = 0;
  float filteredFront = frontDistance;
  float verifiedFront = frontDistance;
  bool alignedFront = false;
  const uint32_t alignStart = millis();
  uint32_t lastGoodFront = alignStart;
  uint32_t frontLastProgress = alignStart;
  uint32_t lastSerial = alignStart;
  int32_t lastEncoderTicks;
  noInterrupts(); lastEncoderTicks = encoder_ticks; interrupts();
  float cumulativeWheelMM = 0.0f;
  const float travelBudgetMM = fabs(frontDistance - FRONT_TARGET_MM) + 470.0f;

  while ((uint32_t)(millis() - alignStart) < 37000UL) {
    read_gyro_data();
    if (!gyroAvailable) { fail("gyro unavailable during front alignment"); return; }
    int32_t ticksNow;
    noInterrupts(); ticksNow = encoder_ticks; interrupts();
    if (ticksNow != lastEncoderTicks) frontLastProgress = millis();
    cumulativeWheelMM += (float)abs(ticksNow - lastEncoderTicks) * FINAL_MM_PER_TICK;
    lastEncoderTicks = ticksNow;
    if (cumulativeWheelMM > travelBudgetMM) {
      fail("parking alignment exceeded safe travel budget"); return;
    }

    float mm = -1.0f;
    bool gotFresh = false;
    if (readFront(frontFrame, mm)) {
      // Reject a sudden optical outlier; never drive toward the wall on it.
      if (fabs(mm - filteredFront) > 115.0f && badJumps < 2) {
        ++badJumps;
      } else {
        badJumps = 0;
        gotFresh = true;
        lastGoodFront = millis();
        filteredFront = 0.73f * mm + 0.27f * filteredFront;
        telemetry_front_cells(tof_zone_mm(FRONT_DIR, 2, 3),
                              tof_zone_mm(FRONT_DIR, 3, 3));
      }
    }
    const uint32_t frontAgeMs = (uint32_t)(millis() - lastGoodFront);
    if (frontAgeMs > 460UL) {
      fail("front ToF stale; parking motor stopped"); return;
    }
    // Near the wall, pause immediately if even a couple sensor frames are
    // missing. Never coast toward the barrier on a stale optical distance.
    if (frontAgeMs > 130UL && filteredFront < 470.0f) {
      move(0);
      steer(0);
      parkingHoldSteer = 0.0f;
      frontLastProgress = millis();
      delay(5);
      continue;
    }

    const float yawError = (float)(firstTarget - gz);
    const float distanceError = filteredFront - FRONT_TARGET_MM;
    if (gotFresh) {
      if (!yawBacking && fabs(distanceError) <= FRONT_DISTANCE_TOL_MM &&
          fabs(yawError) <= PARK_YAW_TOL_DEG) {
        if (++withinFrames >= 2) {
          // Read two more stationary wall frames before proceeding to turn 2.
          move(0); steer(0);
          settle(100);
          float check1 = -1.0f, check2 = -1.0f;
          uint32_t checkFrame = frontFrame;
          uint8_t checks = 0;
          const uint32_t verifyStarted = millis();
          while (checks < 2 && (uint32_t)(millis() - verifyStarted) < 650UL) {
            float fresh = -1.0f;
            if (readFront(checkFrame, fresh)) {
              if (checks == 0) check1 = fresh;
              else check2 = fresh;
              ++checks;
            }
            read_gyro_data();
            delay(4);
          }
          if (checks != 2 || fabs(check1 - check2) > 50.0f) {
            fail("front wall measurement unstable at parking stop"); return;
          }
          verifiedFront = 0.5f * (check1 + check2);
          filteredFront = verifiedFront;
          frontFrame = checkFrame;
          if (fabs(verifiedFront - FRONT_TARGET_MM) <= 19.0f &&
              fabs(firstTarget - gz) <= PARK_YAW_TOL_DEG) {
            alignedFront = true;
            break;
          }
          // Wheel coast may have shifted distance after stopping. Continue
          // under slow ToF control rather than giving up at the first stop.
          withinFrames = 0;
          driveDir = 0;
          parkingHoldSteer = 0.0f;
        }
      } else {
        withinFrames = 0;
      }
    }

    // If centered on the wall but not facing its intended heading, create
    // room by slowly backing away, then drive forward straight to 320 mm.
    if (!yawBacking && fabs(distanceError) <= 17.0f &&
        fabs(yawError) > PARK_YAW_TOL_DEG) {
      if (++yawBackCycles > 2) {
        fail("parking yaw did not converge after two reverse/forward passes"); return;
      }
      backingGoal = filteredFront + 95.0f;
      if (!rearSafe(120.0f)) {
        fail("insufficient rear room for parking yaw correction"); return;
      }
      yawBacking = true;
      telemetry_parking_stage("FRONT YAW BACKUP", firstTarget);
    }
    if (yawBacking && filteredFront >= backingGoal - 12.0f) {
      yawBacking = false;
      telemetry_parking_stage("FRONT 320 RETURN", firstTarget);
    }

    const float goal = yawBacking ? backingGoal : FRONT_TARGET_MM;
    const float goalError = filteredFront - goal;
    // When using a backing goal, hold still briefly if sensor filtering is
    // inside its deadband; let the next fresh frame clear the transition.
    int desiredDir = (goalError > 10.0f) ? 1 :
                     (goalError < -10.0f ? -1 : 0);
    if (desiredDir == 0) {
      move(0);
      steer(0);
      parkingHoldSteer = 0.0f;
      frontLastProgress = millis();
      delay(5);
      continue;
    }

    // Never continue driving TOWARD the front wall with less than this range.
    // Allow moving backward to recover a measured overshoot.
    if (desiredDir > 0 && filteredFront < FRONT_SOFT_LIMIT_MM) {
      fail("parking front clearance limit reached"); return;
    }
    if (desiredDir > 0) {
      const int zone2 = tof_zone_mm(FRONT_DIR, 2, 3);
      const int zone3 = tof_zone_mm(FRONT_DIR, 3, 3);
      if ((zone2 > 60 && zone2 < FRONT_SOFT_LIMIT_MM) ||
          (zone3 > 60 && zone3 < FRONT_SOFT_LIMIT_MM)) {
        fail("front ToF zone indicates unsafe clearance"); return;
      }
    }
    if (driveDir != desiredDir) {
      move(0);
      steer(0);
      parkingHoldSteer = 0.0f;
      delay(75);   // H-bridge direction reversal protection.
      if (driveDir != 0 && ++changes > 5) {
        fail("too many parking direction reversals"); return;
      }
      if (desiredDir < 0) {
        // Only travel as far as needed plus stopping margin.
        const float reverseNeed = min(150.0f, fabs(goalError) + 40.0f);
        if (!rearSafe(reverseNeed)) {
          fail("rear wall too close for parking reversal"); return;
        }
      }
      driveDir = desiredDir;
      frontLastProgress = millis();
    }

    const float remainingMM = fabs(goalError);
    // Continuous distance-to-PWM ramp, including reverse adjustments.
    // At the target: 38 PWM. Farther than 300 mm: at most 44 PWM.
    const float fraction = constrain(remainingMM / 300.0f, 0.0f, 1.0f);
    int pwm = FRONT_PWM_NEAR + (int)((FRONT_PWM_FAR - FRONT_PWM_NEAR) * fraction + 0.5f);
    const uint32_t stoppedForMs = (uint32_t)(millis() - frontLastProgress);
    if (stoppedForMs > 1200UL) pwm = FRONT_PWM_STALL_CAP;
    else if (stoppedForMs > 600UL) pwm = min(FRONT_PWM_STALL_CAP, pwm + 4);
    if (stoppedForMs > 1900UL) {
      fail("encoder stalled while parking aligning"); return;
    }
    holdHeading(desiredDir * pwm, firstTarget);
    if ((uint32_t)(millis() - lastSerial) > 400UL) {
      lastSerial = millis();
      Serial.print("FINAL FRONT mm="); Serial.print(filteredFront, 1);
      Serial.print(" yawErr="); Serial.print(yawError, 2);
      Serial.print(" pwm="); Serial.println(desiredDir * pwm);
    }
    delay(5);
  }
  move(0);
  steer(0);
  if (!alignedFront) {
    fail("front 320mm / gyro alignment did not converge safely"); return;
  }
  telemetry_parking_stage("FRONT 320 VERIFIED", firstTarget);
  Serial.print("FINAL FRONT VERIFIED mm="); Serial.print(verifiedFront, 1);
  Serial.print(" gyro="); Serial.print(gz, 2);
  Serial.print(" target="); Serial.println(firstTarget, 2);

#if FINAL_PARK_STOP_AFTER_FRONT_320
  currentState = PARK;
  telemetry_parking_stage("STOP AT FRONT 320", firstTarget);
  telemetry_note_event("320mm front check complete; second turn disabled");
  return;
#endif

  // Heading and front wall distance were confirmed together above.
  // Do not drive another gyro-correction arc towards the front wall.
  settle(120);
  // The second parking turn also uses the same fixed angle grid.
  // Nominal headings: -990 -> -900, or +990 -> +900, as applicable.
  settle(180);
  pid_error = 0;
  pid_last_error = 0;
  read_gyro_data();
  const double secondStartHeading = gz;
  const double secondTarget = firstTarget + parkDir * TURN_DEG;
  Serial.print("FINAL PARK second turn entry="); Serial.print(secondStartHeading, 2);
  Serial.print(" fixed target="); Serial.println(secondTarget, 2);
  if (!turnTo(secondTarget, "SECOND 90")) {
    fail("second 90 deg did not settle"); return;
  }
  current_angle_gyro = secondTarget;
  settle(180);
  Serial.print("FINAL PARK straight heading error=");
  Serial.println(secondTarget - gz, 2);

  // The side sensor sees a FAR outer wall, then a closer parking barrier.
  // Detect two separate FAR -> NEAR transitions with a FAR gap between.
  // No absolute "wall is below 120mm" threshold is used.
  uint32_t sideLastMs = 0;
  uint32_t driveStart = millis();
  uint32_t lastProgress = driveStart;
  uint32_t lastFreshSideMs = driveStart;
  float lastCm = 0.0f;
  float firstWallAtCm = 0.0f;
  float farReference = 0.0f;
  float firstNearReference = 0.0f;
  float farAccum = 0.0f;
  float gapAccum = 0.0f;
  uint8_t phase = 0;  // 0 learn far, 1 first drop, 2 advance,
                      // 3 learn far again, 4 second drop
  uint8_t farFrames = 0;
  uint8_t nearFrames = 0;
  uint8_t gapFrames = 0;
  bool detectedSecond = false;
  float wallSearchSteer = 0.0f;
  noInterrupts(); encoder_ticks = 0; interrupts();

  telemetry_parking_stage("SIDE FAR -> NEAR 1/2", secondTarget);
  Serial.println(sideSensor == LEFT_DIR ?
                 "FINAL PARK side=LEFT R2C3" : "FINAL PARK side=RIGHT R2C3");
  while ((uint32_t)(millis() - driveStart) < 30000UL && read_cm() < 170.0f) {
    int sideMM = -1;
    const uint32_t oldMs = sideLastMs;
    const bool valid = tof_read_new_zone(sideSensor, sideLastMs, 2, 3, sideMM);
    const bool fresh = sideLastMs != oldMs;
    if (fresh) {
      telemetry_side_cell(valid ? sideMM : -1);
      lastFreshSideMs = millis();
      // Only actual ranging frames count. A missing return is not treated as
      // either a far wall or a near wall and cannot trigger a false edge.
      if (valid && sideMM > 45 && sideMM < 3500) {
        const float mm = (float)sideMM;
        if (phase == 0) {
          farAccum += mm;
          if (++farFrames >= 3) {
            farReference = farAccum / (float)farFrames;
            phase = 1;
            nearFrames = 0;
            Serial.print("FINAL PARK outer wall baseline=");
            Serial.println(farReference, 1);
          }
        } else if (phase == 1) {
          // Learn the larger outer-wall distance, then detect a repeatable
          // drop in multiple consecutive frames rather than a fixed cutoff.
          // Ignore isolated very-high ToF outliers when updating baseline.
          if (mm > farReference && mm < farReference * 1.45f)
            farReference = 0.85f * farReference + 0.15f * mm;
          const float dropRequired = max(SIDE_EDGE_MIN_DROP_MM,
                                         farReference * SIDE_EDGE_DROP_RATIO);
          const bool dropped = farReference - mm >= dropRequired &&
                               mm <= farReference * 0.77f;
          nearFrames = dropped ? (uint8_t)(nearFrames + 1) : 0;
          if (nearFrames >= 3) {
            firstNearReference = mm;
            firstWallAtCm = read_cm();
            phase = 2;
            nearFrames = 0;
            telemetry_parking_stage("SIDE FIRST WALL", secondTarget);
            Serial.print("FINAL PARK first far->near: ");
            Serial.print(farReference, 1); Serial.print(" -> ");
            Serial.println(firstNearReference, 1);
          }
        } else if (phase == 3) {
          // The first barrier has ended. Demand fresh VALID far readings
          // again before even considering the second barrier.
          const float riseRequired = max(SIDE_EDGE_MIN_RISE_MM,
                                         firstNearReference * 0.35f);
          const bool backToFar = mm - firstNearReference >= riseRequired &&
                                 mm >= firstNearReference * 1.35f;
          if (backToFar) {
            gapAccum += mm;
            ++gapFrames;
          } else {
            gapAccum = 0.0f;
            gapFrames = 0;
          }
          if (gapFrames >= 3) {
            farReference = gapAccum / (float)gapFrames;
            phase = 4;
            nearFrames = 0;
            telemetry_parking_stage("SIDE FAR -> NEAR 2/2", secondTarget);
            Serial.print("FINAL PARK gap far baseline=");
            Serial.println(farReference, 1);
          }
        } else if (phase == 4) {
          // Ignore isolated very-high ToF outliers when updating baseline.
          if (mm > farReference && mm < farReference * 1.45f)
            farReference = 0.85f * farReference + 0.15f * mm;
          const float dropRequired = max(SIDE_EDGE_MIN_DROP_MM,
                                         farReference * SIDE_EDGE_DROP_RATIO);
          const bool dropped = farReference - mm >= dropRequired &&
                               mm <= farReference * 0.77f;
          nearFrames = dropped ? (uint8_t)(nearFrames + 1) : 0;
          if (nearFrames >= 3) {
            detectedSecond = true;
            Serial.print("FINAL PARK SECOND far->near: ");
            Serial.print(farReference, 1); Serial.print(" -> ");
            Serial.println(mm, 1);
            break;
          }
        }
      } else {
        // Reject invalid frames rather than counting them as a wall edge.
        if (phase == 1 || phase == 4) nearFrames = 0;
        if (phase == 3) { gapFrames = 0; gapAccum = 0.0f; }
      }
    }
    if ((uint32_t)(millis() - lastFreshSideMs) > 650UL) {
      fail("side ToF stopped sending frames"); return;
    }
    // Only a gentle heading hold is needed after the accurate SECOND 90.
    // The old 0.18 gain and +/-0.90 limit made the steering swing left/right
    // whenever a tiny yaw error or gyro-rate fluctuation changed sign.
    read_gyro_data();
    const double sideYawError = secondTarget - gz;
    const double sideYawCmd = fabs(sideYawError) <= 1.2 ? 0.0 :
        constrain(sideYawError * 0.055 - gyroRateDps * 0.006, -0.30, 0.30);
    wallSearchSteer = 0.80f * wallSearchSteer + 0.20f * (float)sideYawCmd;
    steer(wallSearchSteer);
    move(PARK_DRIVE_PWM);
    if (phase == 2 && read_cm() - firstWallAtCm >= 5.0f) {
      phase = 3;
      gapFrames = 0;
      gapAccum = 0.0f;
      telemetry_parking_stage("SIDE GAP REQUIRED", secondTarget);
      Serial.println("FINAL PARK +5cm after first wall, now waiting for FAR readings");
    }
    const float cm = read_cm();
    if (cm > lastCm + 0.25f) {
      lastCm = cm;
      lastProgress = millis();
    }
    if ((uint32_t)(millis() - lastProgress) > 3000UL) {
      fail("encoder stalled during side wall search"); return;
    }
    delay(5);
  }
  settle(130);
  if (!detectedSecond) { fail("second side wall never confirmed"); return; }

  telemetry_parking_stage("FINAL 3 MOVES", secondTarget);
  Serial.println("FINAL PARK three original parking moves");
  if (!encoderMove(4.0f, 1, secondTarget, normalDir < 0 ? 30 : 29, 5000UL)) {
    fail("last 4cm move failed"); return;
  }
  move_until_angle_max_no_flush(-49, secondTarget + parkDir * 75.0);
  move_until_angle_max_no_flush(normalDir < 0 ? -42 : -45,
                                 secondTarget + parkDir * 2.0);
  move_straight_on_gyro_no_flush(robot_speed, 80);
  move(0);
  steer(0);
  currentState = PARK;
  // Never display current measured gyro as the target; that hides errors.
  telemetry_parking_stage("COMPLETE", secondTarget);
  Serial.println("FINAL PARK complete");
}

void final_execute_command(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  if (cmd.length() == 0 || finalAlignmentFault || currentState == PARK) return;
  if (cmd != "TCS_MARKER") telemetry_note_camera(cmd);
  if (cmd == "TCS_MARKER") {
    if (!tcs_is_expected_marker_detected()) return;
    if (currentState == AVOID_CUBE) {
      finalDeferredCorner = true;
      if (debug) Serial.println("FINAL: marker deferred during cube bypass");
      return;
    }
    if (finalPendingTofCorner) return;
    if (turn_count == max_turns - 1) {
      final_perform_line_turn();  // 13th marker always starts parking immediately.
      return;
    }
    if ((uint32_t)(millis() - lastTurnTime) <= (uint32_t)delayTurns) return;
    read_gyro_data();
    // AFTER_CUBE corners use 10 cm when a cube is recent; otherwise up to
    // 30 cm on the old heading, capped by two fresh front frames <=650 mm.
    if (currentState == AFTER_CUBE) {
      finalSkipCornerAdvance = false;
      final_perform_line_turn();
      return;
    }
    if (currentState == PID) {
      finalPendingTofCorner = true;
      finalPendingTofSince = millis();
      finalFrontCloseFrames = 0;
      finalFrontLastFrameMs = 0;
      finalTofHold = false;
      if (debug) Serial.println("FINAL: color confirmed; straight until R2C3/R3C3 <650mm");
    } else {
      final_perform_line_turn();
    }
    return;
  }

  if (cmd.startsWith("S")) {
    finalRapidCornerActive = false;
      finalLastCubeCameraMs = millis();
    if (currentState == AVOID_CUBE) {
      if (finalCubePhase == FC_DRIVE_OUT && read_cm() >= 9.0f &&
          (uint32_t)(millis() - finalCubePhaseMs) >= 130UL) {
        if ((uint32_t)(millis() - finalPassFollowMs) > 230UL)
          finalPassFollowCount = 0;
        if (finalPassFollowCount < 3) ++finalPassFollowCount;
        finalPassFollowMs = millis();
        finalPassFollowSteering = cmd.substring(1).toFloat();
        telemetry_note_event("watching next cube during bypass");
      }
      return;
    }
    if (currentState == AFTER_CUBE) {
      // After the 13 cm outward bypass, two new FOLLOW frames let the robot
      // abandon the blocked 19 cm recovery and track the next cube.
      if (finalCubePhase == FC_RECOVER_TURN ||
          finalCubePhase == FC_RECOVER_FORWARD) {
        if (millis() - finalCubePhaseMs < 110UL) return;
        if (millis() - finalCubeLastFollowMs > 220UL) finalNextFollowCount = 0;
        finalCubeLastFollowMs = millis();
        if (finalNextFollowCount < 3) ++finalNextFollowCount;
        if (finalNextFollowCount < 2) return;
        finalCubePhase = FC_IDLE;
        currentState = FOLLOW_CUBE;
        desiredSteering = cmd.substring(1).toFloat();
        follow_cube_angle = desiredSteering;
        last_follow_cube = millis();
        Serial.println("FINAL: second cube visible during recovery; following");
      }
      return;
    }
    if (finalPendingTofCorner) {
      finalPendingTofCorner = false;
      finalFrontCloseFrames = 0;
      finalSkipCornerAdvance = true;
      final_perform_line_turn();
      finalSkipCornerAdvance = false;
      finalTofHold = false;
    }
    desiredSteering = cmd.substring(1).toFloat();
    currentState = FOLLOW_CUBE;
    follow_cube_angle = desiredSteering;
    last_follow_cube = millis();
    if (debug) {
      Serial.print("FOLLOW_CUBE angle: ");
      Serial.println(follow_cube_angle);
    }
    return;
  }

  char c = cmd.charAt(0);
  if (cmd.indexOf("RED") != -1) c = 'R';
  else if (cmd.indexOf("GREEN") != -1) c = 'G';

  if (c == 'R' || c == 'G') {
    finalRapidCornerActive = false;
      if (currentState == AVOID_CUBE || currentState == AFTER_CUBE) {
      if (final_cube_new_close(c)) {
        Serial.println("FINAL: second close cube confirmed during recovery");
        final_begin_avoid(c);
      }
      return;
    }
    if (finalPendingTofCorner) {
      finalPendingTofCorner = false;
      finalFrontCloseFrames = 0;
      finalSkipCornerAdvance = true;
      final_perform_line_turn();
      finalSkipCornerAdvance = false;
      finalTofHold = false;
    }
    final_begin_avoid(c);
    if (debug) {
      Serial.print("AVOID_CUBE dir: ");
      Serial.println(cube_avoid_direction);
    }
    return;
  }

  if (debugcam) {
    Serial.print("Ignored cmd: ");
    Serial.println(cmd);
  }
}