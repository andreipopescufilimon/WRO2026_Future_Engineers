// ============================================================================
// HyperLine Robotics | WRO 2026 Future Engineers | World Final
// Obstacle Challenge
// Camera-aware cube phases, gyro/encoder lane recovery, floor-marker corners, parking exit and rolling three-zone parking.
// Source revision: 24 November 2026, rolling parking release.
// ============================================================================

// Helper APIs are defined in tof.ino.
extern double gyroRateDps;
extern bool gyroAvailable;
bool tof_read_new_pair(DistanceDir dir, uint32_t &lastSeenFrameMs, int &a, int &b);
bool tof_enable_fast_parking_side(DistanceDir dir);
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
  FC_IDLE,
  FC_TURN_OUT,
  FC_DRIVE_OUT,
  FC_RECOVER_TURN,
  FC_RECOVER_FORWARD
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

// After cubes, return to the nominal lane using the encoder and gyro path
// measured DURING following/avoidance. The side ToFs are not required for
// post-cube recovery. Normal corner lane centering still uses side ToFs.
// This is dead-reckoning, not an absolute position sensor: wheel slip makes
// the estimated lateral position less accurate. Bound time/travel and keep
// camera + floor-marker interrupts enabled.
#define FINAL_CENTER_AFTER_CUBE 1
#define FINAL_POST_CUBE_ODOMETRY_RETURN 1
static bool finalLaneCenterActive = false;
// Positive means estimated displacement to the robot's LEFT from its lane
// heading. Captured at first camera-follow/avoid until post-cube recovery.
static bool finalCubeOdomActive = false;
static int32_t finalCubeOdomLastTicks = 0;
static float finalCubeLateralMM = 0.0f;
static int32_t finalLaneOdomLastTicks = 0;
static float finalLaneEstimatedLateralMM = 0.0f;
static float finalLaneTravelMM = 0.0f;
static uint8_t finalLaneOdomSettledCount = 0;
static uint32_t finalLaneOdomLastUpdateMs = 0;
static uint32_t finalLaneOdomLastLogMs = 0;

void final_cube_odom_begin() {
  if (finalCubeOdomActive) return;
  finalCubeOdomActive = true;
  finalCubeLateralMM = 0.0f;
  noInterrupts(); finalCubeOdomLastTicks = encoder_ticks; interrupts();
}

void final_cube_odom_tick() {
  if (!finalCubeOdomActive || !gyroAvailable) return;
  int32_t ticks;
  noInterrupts(); ticks = encoder_ticks; interrupts();
  const int32_t delta = ticks - finalCubeOdomLastTicks;
  finalCubeOdomLastTicks = ticks;
  // An unexpected large jump indicates an encoder reset elsewhere. Do not
  // count it as physical travel; phase resets are re-based explicitly below.
  if (abs(delta) > 120) return;
  const float headingRad = (float)((gz - current_angle_gyro) * 0.017453292519943295);
  finalCubeLateralMM += (float)abs(delta) * FINAL_MM_PER_TICK * sinf(headingRad);
  finalCubeLateralMM = constrain(finalCubeLateralMM, -450.0f, 450.0f);
}
// Post-cube recovery filters single/noisy camera messages but immediately
// gives a CONFIRMED new cube priority. Floor-marker turns always override.
// Ordinary corner centering still allows camera follow/avoid as before.
static bool finalLaneIgnoreCubeCommands = false;
static uint32_t finalLaneCameraCandidateMs = 0;
static char finalLaneCameraCandidateType = 0;
static uint8_t finalLaneCameraCandidateCount = 0;
static float finalLaneCameraFollowAngle = 0.0f;
static uint32_t finalLaneArmMs = 0;
static int32_t finalLaneArmTicks = 0;
static float finalLaneLeftFilteredMM = -1.0f;
static float finalLaneRightFilteredMM = -1.0f;
static uint32_t finalLaneLeftFreshMs = 0;
static uint32_t finalLaneRightFreshMs = 0;
static uint32_t finalLaneLastLeftFrameMs = 0;
static uint32_t finalLaneLastRightFrameMs = 0;
static uint32_t finalLaneLeftFrame = 0, finalLaneRightFrame = 0;
static uint32_t finalLaneLastSampleMs = 0;
static uint8_t finalLaneValidPairs = 0, finalLaneCenteredPairs = 0;
static float finalLaneYawBiasDeg = 0.0f;
static float finalLaneLastWidthMM = 0.0f;
static float finalLanePreviousErrorMM = 0.0f;
static bool finalLaneHavePreviousError = false;

// Direction is selected from the ROBOT position after passing the cube:
// right-left > 0 means closer to LEFT, independent of cube color/location.
// Sensor noise often exceeds 1cm per cell; use a tighter corridor-center
// tolerance, but demand repeatable measurements instead of one lucky sample.
static const float FINAL_CENTER_TOLERANCE_MM = 24.0f;
static const uint32_t FINAL_CENTER_MAX_MS = 8000UL;
static const float FINAL_CENTER_MAX_FORWARD_MM = 1000.0f;
static const uint32_t FINAL_CENTER_MISSING_SENSOR_MS = 1050UL;
static float finalLaneFilteredErrorMM = 0.0f;
static bool finalLaneErrorInitialized = false;
static float finalLaneBestErrorMM = 10000.0f;
static uint32_t finalLaneLastProgressMs = 0;
static uint32_t finalLaneLastGoodPairMs = 0;
static uint32_t finalLaneProcessedLeftFrame = 0;
static uint32_t finalLaneProcessedRightFrame = 0;

void final_lane_center_reset() {
  finalLaneCenterActive = false;
  finalLaneIgnoreCubeCommands = false;
  finalLaneCameraCandidateMs = 0;
  finalLaneCameraCandidateType = 0;
  finalLaneCameraCandidateCount = 0;
  finalLaneCameraFollowAngle = 0.0f;
  finalLaneYawBiasDeg = 0.0f;
  finalLaneValidPairs = finalLaneCenteredPairs = 0;
  finalLaneHavePreviousError = false;
  finalLaneLastWidthMM = 0.0f;
  finalLaneLeftFilteredMM = finalLaneRightFilteredMM = -1.0f;
  finalLaneLeftFreshMs = finalLaneRightFreshMs = 0;
  finalLaneLastLeftFrameMs = finalLaneLastRightFrameMs = 0;
  finalLaneProcessedLeftFrame = finalLaneProcessedRightFrame = 0;
  finalLaneErrorInitialized = false;
  finalLaneFilteredErrorMM = 0.0f;
  finalLaneBestErrorMM = 10000.0f;
  finalLaneLastGoodPairMs = 0;
  finalLaneLastProgressMs = 0;
  finalLaneEstimatedLateralMM = 0.0f;
  finalLaneTravelMM = 0.0f;
  finalLaneOdomSettledCount = 0;
  finalLaneOdomLastUpdateMs = 0;
  finalLaneOdomLastLogMs = 0;
}

void final_lane_center_arm(bool afterCube = false) {
  const float recordedLateralMM = afterCube && finalCubeOdomActive ? finalCubeLateralMM : 0.0f;
  final_lane_center_reset();
#if FINAL_CENTER_AFTER_CUBE
  finalLaneCenterActive = true;
  finalLaneIgnoreCubeCommands = afterCube;
  finalLaneArmMs = millis();
  finalLaneLastProgressMs = finalLaneArmMs;
  noInterrupts();
  finalLaneArmTicks = encoder_ticks;
  finalLaneOdomLastTicks = encoder_ticks;
  interrupts();
  finalLaneEstimatedLateralMM = constrain(recordedLateralMM, -280.0f, 280.0f);
  finalCubeOdomActive = false;
  finalLaneLastSampleMs = 0;
  finalLaneLeftFrame = finalLaneRightFrame = 0;
  if (afterCube) {
    Serial.print("FINAL post-cube estimated lateral mm=");
    Serial.println(finalLaneEstimatedLateralMM, 1);
  }
  telemetry_note_event(afterCube ? "post-cube gyro+encoder lane return; camera/markers active" :
                                   "middle lane: side ToF correction");
#endif
}

void final_lane_center_tick() {
  if (!finalLaneCenterActive) return;
  if (currentState != PID || turn_count >= max_turns - 1) {
    final_lane_center_reset();
    return;
  }
  // Markers and confirmed corner turns always take priority over centering.
  if (finalPendingTofCorner || finalExecutingTurn) {
    final_lane_center_reset();
    return;
  }
  if (finalRapidCornerActive) return;

  const uint32_t now = millis();
  int32_t ticksNow;
  noInterrupts(); ticksNow = encoder_ticks; interrupts();
  const float traveledMM = (float)abs(ticksNow - finalLaneArmTicks) * FINAL_MM_PER_TICK;
  // Never suppress the camera indefinitely in a genuinely unobservable lane.
  if ((uint32_t)(now - finalLaneArmMs) > FINAL_CENTER_MAX_MS ||
      traveledMM > FINAL_CENTER_MAX_FORWARD_MM) {
    final_lane_center_reset();
    pid_last_error = current_angle_gyro - gz;
    telemetry_note_event("post-cube centering budget reached; camera enabled");
    return;
  }
  if (!finalLaneIgnoreCubeCommands &&
      ((finalLastCubeCameraMs && (uint32_t)(now - finalLastCubeCameraMs) < 650UL) ||
       (uint32_t)(now - last_cube_time) < 550UL)) {
    finalLaneYawBiasDeg *= 0.75f;
    return;
  }
#if FINAL_POST_CUBE_ODOMETRY_RETURN
  if (finalLaneIgnoreCubeCommands) {
    // No side ToF reads in post-cube lane return. Estimate lateral offset
    // using the actual path recorded since first detecting the last cube.
    // Steering takes the sign opposite to the estimated displacement and
    // decreases continuously toward zero as the centerline is approached.
    if ((uint32_t)(now - finalLaneOdomLastUpdateMs) < 25UL) return;
    finalLaneOdomLastUpdateMs = now;
    const int32_t delta = ticksNow - finalLaneOdomLastTicks;
    finalLaneOdomLastTicks = ticksNow;
    if (abs(delta) <= 120) {
      const float mm = (float)abs(delta) * FINAL_MM_PER_TICK;
      finalLaneTravelMM += mm;
      const float headingRad = (float)((gz - current_angle_gyro) * 0.017453292519943295);
      finalLaneEstimatedLateralMM += mm * sinf(headingRad);
    }
    const float estimatedError = finalLaneEstimatedLateralMM;
    const float desiredBias = constrain(-estimatedError * 0.075f, -12.0f, 12.0f);
    finalLaneYawBiasDeg += constrain(desiredBias - finalLaneYawBiasDeg, -0.75f, 0.75f);
    if (fabsf(estimatedError) < 25.0f && fabs(current_angle_gyro - gz) < 2.5 &&
        fabsf(finalLaneYawBiasDeg) < 2.0f) {
      if (++finalLaneOdomSettledCount >= 5) {
        final_lane_center_reset();
        pid_last_error = current_angle_gyro - gz;
        telemetry_note_event("post-cube estimated lane rejoined; camera active");
        return;
      }
    } else {
      finalLaneOdomSettledCount = 0;
    }
    if ((uint32_t)(now - finalLaneOdomLastLogMs) >= 320UL) {
      finalLaneOdomLastLogMs = now;
      Serial.print("FINAL ODOLANE lateral_est_mm=");
      Serial.print(estimatedError, 1);
      Serial.print(" yaw_bias_deg=");
      Serial.print(finalLaneYawBiasDeg, 1);
      Serial.print(" traveled_mm=");
      Serial.println(finalLaneTravelMM, 1);
    }
    return;
  }
#endif
  if ((uint32_t)(now - finalLaneLastSampleMs) < 45UL) return;
  finalLaneLastSampleMs = now;

  // Track SAME central-column cells on each side. Reject an inconsistent
  // two-cell pair instead of steering on an arbitrary short reflection.
  auto sampleSide = [&](DistanceDir dir, uint32_t &frameStamp,
                        float &filteredMM, uint32_t &lastGoodMs) {
    tof_poll_one((uint8_t)dir);
    const uint32_t stamp = tof_last_frame_ms(dir);
    if (!stamp || stamp == frameStamp || (uint32_t)(now - stamp) > 150UL) return;
    frameStamp = stamp;
    const int a = tof_zone_mm(dir, 2, 3);
    const int b = tof_zone_mm(dir, 3, 3);
    const bool va = (a >= 55 && a <= 1500);
    const bool vb = (b >= 55 && b <= 1500);
    if (!va && !vb) return;
    float measurement;
    if (va && vb && abs(a - b) <= 125) {
      measurement = 0.5f * (a + b);
    } else if (va && vb) {
      if (filteredMM < 0) return; // Cannot resolve a new contradictory pair.
      const float da = fabsf(a - filteredMM), db = fabsf(b - filteredMM);
      measurement = da < db ? a : b;
      if (fabsf(measurement - filteredMM) > 75.0f) return;
    } else {
      measurement = va ? a : b;
    }
    if (filteredMM > 0 && fabsf(measurement - filteredMM) > 135.0f) return;
    // Slightly less lag lets the recovery react before the robot passes center.
    filteredMM = filteredMM < 0 ? measurement : 0.58f * filteredMM + 0.42f * measurement;
    lastGoodMs = now;
  };
  sampleSide(LEFT_DIR, finalLaneLastLeftFrameMs,
             finalLaneLeftFilteredMM, finalLaneLeftFreshMs);
  sampleSide(RIGHT_DIR, finalLaneLastRightFrameMs,
             finalLaneRightFilteredMM, finalLaneRightFreshMs);

  const float left = finalLaneLeftFilteredMM;
  const float right = finalLaneRightFilteredMM;
  const float width = left + right;
  const bool plausible = left > 0 && right > 0 &&
      (uint32_t)(now - finalLaneLeftFreshMs) < 220UL &&
      (uint32_t)(now - finalLaneRightFreshMs) < 220UL &&
      width >= 340.0f && width <= 1800.0f &&
      (finalLaneLastWidthMM <= 0 || fabsf(width - finalLaneLastWidthMM) < 150.0f);
  // Do not count the same cached samples more than once. A real centered
  // decision requires independent *fresh* measurements from BOTH modules.
  const bool newPair = finalLaneLastLeftFrameMs != finalLaneProcessedLeftFrame &&
                       finalLaneLastRightFrameMs != finalLaneProcessedRightFrame;
  if (!plausible || !newPair) {
    finalLaneYawBiasDeg *= 0.90f; // Slowly unwind when data is missing.
    if ((uint32_t)(now - (finalLaneLastGoodPairMs ? finalLaneLastGoodPairMs : finalLaneArmMs)) >
        FINAL_CENTER_MISSING_SENSOR_MS) {
      final_lane_center_reset();
      pid_last_error = current_angle_gyro - gz;
      telemetry_note_event("center sensors unreliable; cube camera resumed");
    }
    return;
  }
  finalLaneProcessedLeftFrame = finalLaneLastLeftFrameMs;
  finalLaneProcessedRightFrame = finalLaneLastRightFrameMs;
  finalLaneLastGoodPairMs = now;
  finalLaneLastWidthMM = finalLaneLastWidthMM <= 0 ? width :
                          (0.80f * finalLaneLastWidthMM + 0.20f * width);
  if (finalLaneValidPairs < 8) ++finalLaneValidPairs;
  const float rawErrorMM = right - left;
  // Smooth the left-right difference separately from each wall measurement.
  if (!finalLaneErrorInitialized) {
    finalLaneFilteredErrorMM = rawErrorMM;
    finalLaneErrorInitialized = true;
  } else {
    finalLaneFilteredErrorMM = 0.60f * finalLaneFilteredErrorMM + 0.40f * rawErrorMM;
  }
  const float error = finalLaneFilteredErrorMM;
  const float errorDelta = finalLaneHavePreviousError ?
      constrain(error - finalLanePreviousErrorMM, -34.0f, 34.0f) : 0.0f;
  finalLanePreviousErrorMM = error;
  finalLaneHavePreviousError = true;
  if (finalLaneValidPairs < 3) return;

  const float magnitude = fabsf(error);
  if (magnitude + 9.0f < finalLaneBestErrorMM) {
    finalLaneBestErrorMM = magnitude;
    finalLaneLastProgressMs = now;
  }
  // If lateral correction stalls while good measurements continue, increase
  // the target-heading bias modestly. Never force a large blind swerve.
  const uint32_t noProgressMs = (uint32_t)(now - finalLaneLastProgressMs);
  // Do not suppress new camera detections for the full 8s if the measured
  // position genuinely fails to improve despite valid side measurements.
  if (magnitude > 55.0f && noProgressMs > 3100UL) {
    final_lane_center_reset();
    pid_last_error = current_angle_gyro - gz;
    telemetry_note_event("centering not converging; cube camera resumed");
    return;
  }
  const bool progressSlow = magnitude > 45.0f && noProgressMs > 1300UL;
  const float gain = progressSlow ? 0.055f : 0.046f;
  const float predictedError = error + 1.55f * errorDelta;
  const float requestedBias = magnitude <= FINAL_CENTER_TOLERANCE_MM ? 0.0f :
      constrain(predictedError * gain, -16.0f, 16.0f);
  finalLaneYawBiasDeg = 0.63f * finalLaneYawBiasDeg + 0.37f * requestedBias;

  // The yaw target must be regained as well as the physical lane center.
  // Otherwise the next straight can drift right back toward a wall.
  if (magnitude <= FINAL_CENTER_TOLERANCE_MM &&
      fabs(current_angle_gyro - gz) <= 2.0 &&
      fabsf(finalLaneYawBiasDeg) <= 1.8f) {
    if (++finalLaneCenteredPairs >= 4) {
      final_lane_center_reset();
      pid_last_error = current_angle_gyro - gz;
      telemetry_note_event("middle lane centered + heading restored");
    }
  } else {
    finalLaneCenteredPairs = 0;
  }
}

void final_cube_phase(uint8_t phase) {
  finalCubePhase = (FinalCubePhase)phase;
  finalCubePhaseMs = millis();
  finalCubeLastProgressMs = millis();
  finalCubeProgressCm = 0.0f;
  finalCubeBestYawError = fabs(finalCubeTarget - gz);
  finalCubeYawProgressMs = millis();
  pid_error = pid_last_error = 0;
  if (phase == FC_DRIVE_OUT || phase == FC_RECOVER_FORWARD) {
    noInterrupts();
    encoder_ticks = 0;
    finalCubeOdomLastTicks = 0;
    interrupts();
  }
  if (phase == FC_RECOVER_TURN) {
    // An S command on the second cube can now interrupt recovery.
    finalNextFollowCount = 0;
    finalNextCloseCount = 0;
  }
}

void final_begin_avoid(char color) {
  read_gyro_data();
  final_cube_odom_begin();
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
    Serial.print("FINAL CUBE new ");
    Serial.print(color);
    Serial.print(" target=");
    Serial.println(finalCubeTarget, 1);
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
    move(0);
    steer(0);
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
      move(0);
      steer(0);
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
    move(0);
    steer(0);
    finalAlignmentFault = true;
    currentState = PARK;
    Serial.println("FINAL CUBE STOP: wheel encoder not progressing");
    return;
  }

  if (finalCubePhase == FC_DRIVE_OUT && cm >= 13.0f) {
    // A second cube seen in at least two recent camera frames can take over
    // once the original outward bypass is safely completed.
    if (finalPassFollowCount >= 2 && finalPassFollowMs != 0 && (uint32_t)(millis() - finalPassFollowMs) <= 230UL) {
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
    final_lane_center_arm(true);
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
    finalCubePhase == FC_RECOVER_FORWARD && read_cm() >= 7.0f && (uint32_t)(millis() - last_cube_time) >= 650UL;
  const bool sameColorWithoutFollow = color == finalCubeColor && finalNextFollowCount < 2;
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
  robot_speed = 50;
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
  final_lane_center_reset();
}

// Cache side ToF frames before Start is pressed. Parking exit can then
// choose its direction immediately, without a blind 1.6s stationary wait.
// This is only a sensing cache: no motors/servo are commanded until Start.
struct FinalExitSideCache {
  DistanceDir direction;
  uint32_t lastFrameMs;
  uint32_t lastValidMs;
  float lastMM;
  uint8_t nearFrames;
  uint8_t validFrames;
};
static FinalExitSideCache finalExitLeft = { LEFT_DIR, 0, 0, -1.0f, 0, 0 };
static FinalExitSideCache finalExitRight = { RIGHT_DIR, 0, 0, -1.0f, 0, 0 };

void final_exit_prestart_sample() {
  auto update = [&](FinalExitSideCache &s) {
    if (!tof_sensor_online(s.direction)) return;
    tof_poll_one((uint8_t)s.direction);
    const uint32_t stamp = tof_last_frame_ms(s.direction);
    if (!stamp || stamp == s.lastFrameMs ||
        (uint32_t)(millis() - stamp) > 150UL) return;
    s.lastFrameMs = stamp;
    int closest = 32767;
    for (uint8_t r = 2; r <= 3; ++r) {
      for (uint8_t c = 2; c <= 3; ++c) {
        const int mm = tof_zone_mm(s.direction, r, c);
        if (mm >= 40 && mm < closest) closest = mm;
      }
    }
    if (closest == 32767) {
      s.nearFrames = 0;  // An invalid frame cannot confirm a nearby wall.
      return;
    }
    s.lastMM = (float)closest;
    s.lastValidMs = millis();
    if (s.validFrames < 255) ++s.validFrames;
    if (closest < 100) {
      if (s.nearFrames < 255) ++s.nearFrames;
    } else {
      s.nearFrames = 0;
    }
  };
  update(finalExitLeft);
  update(finalExitRight);
}

void final_exit_parking() {
  const uint32_t EXIT_MAX_STALE_MS = 350UL;
  const uint32_t EXIT_SHORT_RETRY_MS = 220UL; // only when pre-Start cache is inconclusive
  // The normal outcome is an immediate decision from three frames sampled
  // while idle. For a true no-return case, stop instead of guessing a turn.
  auto chooseExit = [&]() -> int {
    const uint32_t now = millis();
    const bool leftFresh = finalExitLeft.lastValidMs &&
      (uint32_t)(now - finalExitLeft.lastValidMs) <= EXIT_MAX_STALE_MS;
    const bool rightFresh = finalExitRight.lastValidMs &&
      (uint32_t)(now - finalExitRight.lastValidMs) <= EXIT_MAX_STALE_MS;
    const bool leftNear = leftFresh && finalExitLeft.nearFrames >= 3;
    const bool rightNear = rightFresh && finalExitRight.nearFrames >= 3;
    if (leftNear && !rightNear) return 1;   // close LEFT => exit RIGHT
    if (rightNear && !leftNear) return -1;  // close RIGHT => exit LEFT
    if (leftNear && rightNear) {
      if (fabsf(finalExitLeft.lastMM - finalExitRight.lastMM) >= 35.0f)
        return finalExitLeft.lastMM < finalExitRight.lastMM ? 1 : -1;
      return 0;
    }
    // Retain original two-side comparison when neither side is <100mm.
    if (leftFresh && rightFresh &&
        finalExitLeft.validFrames >= 3 && finalExitRight.validFrames >= 3)
      return finalExitLeft.lastMM < finalExitRight.lastMM ? 1 : -1;
    return 0;
  };
  final_exit_prestart_sample();
  int exitDirection = chooseExit();
  if (exitDirection == 0) {
    const uint32_t began = millis();
    while (exitDirection == 0 &&
           (uint32_t)(millis() - began) < EXIT_SHORT_RETRY_MS) {
      final_exit_prestart_sample();
      telemetry_tick();
      exitDirection = chooseExit();
      delay(4);
    }
  }
  Serial.print("EXIT immediate LEFT mm=");
  Serial.print(finalExitLeft.lastMM, 0);
  Serial.print(" nearFrames="); Serial.print(finalExitLeft.nearFrames);
  Serial.print(" RIGHT mm="); Serial.print(finalExitRight.lastMM, 0);
  Serial.print(" nearFrames="); Serial.println(finalExitRight.nearFrames);
  if (exitDirection == 0) {
    move(0);
    steer(0);
    finalAlignmentFault = true;
    currentState = PARK;
    telemetry_parking_fail("EXIT: no valid side decision at Start");
    Serial.println("FINAL EXIT STOP: neither side has a confirmed usable reading");
    return;
  }

  // The original exit path is deliberately kept identical in both branches.
  if (exitDirection == 1) {
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
  if (!gyroAvailable) {
    move(0);
    steer(0);
    return;
  }
  if (finalAlignmentFault) {
    move(0);
    return;
  }
  final_color_poll();
  read_gyro_data();
  if (currentState == FOLLOW_CUBE || currentState == AVOID_CUBE || currentState == AFTER_CUBE)
    final_cube_odom_tick();

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
      finalTofHold = (a <= 0 || b <= 0) && (uint32_t)(millis() - finalPendingTofSince) > 500UL;
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

  if (currentState == PID) final_lane_center_tick();
  else if (finalLaneCenterActive) final_lane_center_reset();

  switch (currentState) {
    case PID:
      {
        const double err = current_angle_gyro + finalLaneYawBiasDeg - gz;
        // Get the camera facing the next side earlier after a floor-marker turn.
        // Only normal PID corners use this; FOLLOW_CUBE / AVOID_CUBE retain
        // their own steering and priority. Front-wall marker gate is unchanged.
        if (finalRapidCornerActive) {
          const bool recent = (uint32_t)(millis() - finalRapidCornerStartMs) < 2600UL;
          if (recent && fabs(err) > 10.0) {
            // On a PID square-side change, use the actual mechanical lock.
            // Leave camera FOLLOW_CUBE / AVOID_CUBE steering untouched.
            steer(err > 0.0 ? 1.0 : -1.0);
            move(finalTofHold ? 0 : max(38, robot_speed - 7));
            pid_last_error = err;  // no derivative kick when regular PD resumes
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
        int cornerPwm = fabs(err) > 20.0 ? max(38, robot_speed - 7) : robot_speed;
        // Front wall establishes the repeatable turn position for no-cube
        // and recovered-after-cube PID runs. Decelerate from 900mm so a
        // 650mm corner is not overshot by one sensor frame.
        if (finalPendingTofCorner && !finalTofHold) {
          const int fa = tof_zone_mm(FRONT_DIR,2,3);
          const int fb = tof_zone_mm(FRONT_DIR,3,3);
          if (fa > 0 && fb > 0) {
            const int frontMM = (fa+fb)/2;
            if (frontMM < 900) cornerPwm = min(cornerPwm,
                                      constrain(34 + (frontMM-650)/8,34,cornerPwm));
          }
        }
        // Controlled centering needs successive valid side measurements.
        // Keep the wheels moving (avoid stalling at very low PWM), but do
        // not race through the recovery while camera detections are filtered.
        if (finalLaneCenterActive && finalLaneIgnoreCubeCommands)
          cornerPwm = min(cornerPwm, max(46, robot_speed - 4));
        move(finalTofHold ? 0 : cornerPwm);
        break;
      }

    case FOLLOW_CUBE:
      {
        if (millis() - last_follow_cube > FOLLOW_CUBE_DEAD_TIME) {
          currentState = PID;
          pid_last_error = current_angle_gyro - gz;
          last_cube_time = millis();
          final_lane_center_arm(true);  // Center, but keep new cube detection active.
          break;
        }
        steer(follow_cube_angle);
        move(robot_speed);
        break;
      }

    case AVOID_CUBE:
    case AFTER_CUBE:
      {
        final_cube_control_tick();
        break;
      }

    case PARK:
      {
        move(0);
        break;
      }
  }
  // A marker encountered while bypassing a cube is applied once the
  // bypass is complete, so it cannot alter the first cube's escape heading.
  if (finalDeferredCorner && currentState == AFTER_CUBE && finalCubePhase == FC_RECOVER_TURN && !finalExecutingTurn) {
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
    else {
      tcsLatchedColor = TCS_NONE;
      return;
    }
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
  noInterrupts();
  encoder_ticks = 0;
  interrupts();
  uint32_t lastFrontFrame = 0;
  uint32_t lastProgressMs = millis();
  const uint32_t startMs = lastProgressMs;
  float progressedCm = 0.0f;
  double prevError = 0.0;
  uint8_t nearFrames = 0;
  bool reachedWall = false;
  // After a cube, allow an extra short approach ONLY when the front sensor
  // reliably measures a wall still beyond the 650mm corner stand-off.
  // If the ToF is missing, retain the old 30cm movement ceiling.
  int travelCapCm = maxCm;
  if (useFrontTof) {
    for (uint8_t k=0; k<4; ++k) {
      int a=-1,b=-1;
      uint32_t previousFrame=lastFrontFrame;
      const bool fresh=tof_read_new_pair(FRONT_DIR,lastFrontFrame,a,b);
      if (fresh && a >= 650 && b >= 650 && a <= 1200 && b <= 1200) {
        const int frontMM = (a+b)/2;
        travelCapCm = max(maxCm, min(45, (frontMM-650)/10 + 4));
        break;
      }
      if (lastFrontFrame == previousFrame) delay(5);
    }
  }
  while (read_cm() < (float)travelCapCm) {
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
                       ? (uint8_t)(nearFrames + 1)
                       : 0;
        if (nearFrames >= 2) {
          reachedWall = true;
          break;
        }
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
    if ((uint32_t)(millis() - lastProgressMs) > 2500UL || (uint32_t)(millis() - startMs) > 30000UL || !gyroAvailable) {
      move(0);
      steer(0);
      finalAlignmentFault = true;
      Serial.println("FINAL: AFTER_CUBE advance: encoder/gyro timeout");
      return FINAL_ADVANCE_FAULT;
    }
    steer(steering);
    int approachPwm = robot_speed;
    if (useFrontTof) {
      const int a=tof_zone_mm(FRONT_DIR,2,3);
      const int b=tof_zone_mm(FRONT_DIR,3,3);
      if (a>0 && b>0) {
        const int frontMM=(a+b)/2;
        if (frontMM < 900) approachPwm = min(approachPwm,
                                 constrain(34+(frontMM-650)/8,34,approachPwm));
      }
    }
    move(approachPwm);
    delay(4);
  }
  move(0);
  steer(0);
  pid_error = 0;
  pid_last_error = 0;
  if (debug) {
    Serial.print("FINAL AFTER_CUBE advance cm=");
    Serial.print(read_cm(), 1);
    Serial.println(reachedWall ? " front <=650mm" : " bounded approach complete (front wall not confirmed)");
  }
  return FINAL_ADVANCE_COMPLETE;
}

void final_perform_line_turn() {
  if (finalExecutingTurn || turn_count >= max_turns || finalAlignmentFault) return;
  const bool parkingMarker = (turn_count == max_turns - 1);
  // Parking marker must obey the same cooldown as every other corner.
  // A spurious repeated color detection must not start parking early.
  if ((uint32_t)(millis() - lastTurnTime) <= (uint32_t)delayTurns) return;
  if (parkingMarker) {
    read_gyro_data();
    // On the 13th marker we must ALREADY be facing side 12. A duplicate
    // marker one side early leaves the gyro ~90 degrees from this heading.
    // Allow up to 50 deg of real-world heading error without accepting a
    // completely missing corner. Do not consume the marker if it is early.
    const double sideHeadingError = current_angle_gyro - gz;
    if (!gyroAvailable || !isfinite(sideHeadingError) || fabs(sideHeadingError) > 50.0) {
      Serial.print("FINAL: parking marker ignored: side heading error=");
      Serial.println(sideHeadingError, 2);
      return;
    }
  }

  finalExecutingTurn = true;
  final_lane_center_reset();
  finalCubeOdomActive = false;  // A corner changes the nominal lane heading.
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
      const bool cubeStillVisible = finalLastCubeCameraMs != 0 && (uint32_t)(millis() - finalLastCubeCameraMs) <= FINAL_CUBE_VISIBLE_RECENT_MS;
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
    } else if (currentState == FOLLOW_CUBE && fabs(oldSideHeading - gz) < 10.0) {
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
  // Apply exactly the same middle-lane recovery after no-cube and
  // after-cube turns. FOLLOW_CUBE keeps camera priority instead.
  if (currentState == PID) final_lane_center_arm();
  pid_error = 0;
  pid_last_error = 0;
  delayTurns = 1500;
  lastTurnTime = millis();
  lastLineDetectedTime = 0;
  finalExecutingTurn = false;
}

// Full parking enabled: continue after first turn and R2C3 front alignment.
#define FINAL_PARK_STOP_AFTER_FRONT_340 0

void final_park() {
  const int parkDir = turn_direction < 0 ? 1 : -1;
  // Both side ToFs are inspected during the wall search. A single hard-coded
  // side or zone can miss the actual parking bay after the return turns.
  const DistanceDir preferredSideSensor = parkDir > 0 ? LEFT_DIR : RIGHT_DIR;
  // The gyro is not zeroed after the parking exit. The 12 regular corners
  // establish the ABSOLUTE heading of the last straight: +1080 or -1080.
  // Hold this target for the whole entry, even if measured yaw has a small
  // residual. The first parking corner is the opposite +/-90 degree turn.
  const double TURN_DEG = 90.0;
  const double side12Heading = (double)turn_direction * 1080.0;
  const double fixedFirstTarget = side12Heading + parkDir * TURN_DEG;
  const double fixedSecondTarget = fixedFirstTarget + parkDir * TURN_DEG;
  // For +1: +1080 -> +990 -> +900. For -1: -1080 -> -990 -> -900.
  // ========================================================================
  // FINAL PARK: THREE TUNING ZONES (all dimensions are millimeters unless cm)
  // Zone 1: after lap 3 -> first 90 deg -> align to FRONT wall -> second 90.
  // Zone 2: locate side post 1 + post 2 -> advance by the chosen offset.
  // Zone 3: two reverse arcs + short straight finish (near end of function).
  // Edit values HERE. The state-machine and cube/lap navigation are unchanged.
  // ========================================================================

  // ZONE 1: approach and front-wall alignment
  const float PARK_Z1_MINUS_ENTRY_CM = 25.0f;      // -1 only: straight at turn-12 heading
  const float FRONT_TARGET_MM = 400.0f;           // Extra wall clearance for second 90deg turn and side search
  // FRONT ALIGNMENT ONLY: use FRONT R2C3 exclusively for distance control.
  // A 9-frame dominant-distance window rejects isolated 500->200mm returns.
  // No encoder-planned front steps: drive from trusted ToF distance directly.
  const float PARK_Z1_PLUS_TARGET_TOL_MM = 12.0f;
  const uint8_t PARK_Z1_PLUS_MAX_REVERSES = 4;
  const int PARK_Z1_FRONT_FAR_PWM = 35;
  const int PARK_Z1_FRONT_NEAR_PWM = 35;
  const int PARK_Z1_FRONT_BREAKAWAY_PWM = 35;
  // Both parking 90-degree turns share the requested 35 PWM cap.

  // ZONE 2: column-3 side post detection
  // Post dimensions: approximately 10mm thick, 220mm center-to-center.
  const int SIDE_EDGE_CHANGE_MM = turn_direction == 1 ? 50 : 35;
  // Physical pitch is 220mm. Log 6 shows approximately 286-300mm of
  // reported encoder travel between the two valid close pulses on +1.
  // Allow the observed encoder scale variation without accepting nearby
  // repeated echoes as a second post. -1 keeps its established V13 window.
  const float SIDE_POST_SEPARATION_MM = turn_direction == 1 ? 255.0f : 220.0f;
  const float SIDE_POST_SEPARATION_TOL_MM = turn_direction == 1 ? 80.0f : 145.0f;
  const float SIDE_AFTER_WALL2_MM = 45.0f;        // offset from second post leading edge
  const int PARK_DRIVE_PWM = 35;                   // side-search speed before first post
  const int PARK_AFTER_FIRST_WALL_PWM = 35;       // side-search speed after first post

  // ZONE 3: first reverse arc slightly deeper into the parking bay.
  // Increase/decrease this value in small steps after checking rear clearance.
  // The remaining two angle targets and all parking speeds are unchanged.
  const double PARK_Z3_REVERSE_ENTRY_DEG = 80.0;  // was 75.0

  const double PARK_YAW_TOL_DEG = 2.5;       // Preferred settled accuracy.
  const double PARK_YAW_CONTINUE_DEG = 5.0;  // Existing front-alignment yaw guard.
  // Both parking turns aim at the fixed gyro heading. A settled error of
  // Any settled error under 2deg is accepted rather than failing the run.
  const double PARK_TURN_PRECISE_TOL_DEG = 2.00;
  const double PARK_FIRST_ACCEPT_TOL_DEG = 2.00;
  // Parking entry, alignment, 90deg turns and post search are fixed at PWM 35.
  // The final three parking arcs use the separate global park_speed=32.
  // Low forward speed + near-full PARK servo lock = small turning radius.
  // PWM 35 is user-selected. If it stalls, stop rather than boosting above 35.
  const float PARK_SHORT_RADIUS_STEER = 0.94f;
  const int PARK_TURN_PWM = 35;
  const int PARK_TURN_NEAR_PWM = 35;
  const int PARK_TURN_FINE_PWM = 35;
  const int PARK_GYRO_TRIM_PWM = 35;  // Small increase for rolling gyro alignment
  // Both turns use one continuous tight-radius arc and a rolling handoff
  // when yaw error is under 2deg at low angular speed. If not, bounded
  // gyro correction and front/rear clearance checks still apply.
  const int PARK_FIRST_FAR_PWM = 35;
  const int PARK_FIRST_NEAR_PWM = 35;
  const int PARK_FIRST_FINE_PWM = 35;
  const int PARK_FIRST_RECOVER_PWM = 35;
  const float PARK_FIRST_RECOVER_MAX_MM = 180.0f;
  const double PARK_FIRST_RECOVER_LIMIT_DEG = 15.0;
  const double PARK_STOP_RATE_DPS = 1.2;
  bool finalYawCorrection = false;

  auto fail = [&](const char *reason) {
    move(0);
    steer(0);
    finalAlignmentFault = true;
    currentState = PARK;
    telemetry_parking_fail(reason);
    Serial.print("FINAL PARK STOP: ");
    Serial.print(reason);
    Serial.print(" gyro=");
    Serial.println(gz, 2);
  };

  // Parking heading hold uses the known-good forward servo sign.
  // The original robot forward steering convention is:
  // steer(+error) increases gz. Reuse it throughout parking, for both directions.
  // Unlike the former capped 0.20 steering request, this corrects an arrival
  // heading offset (e.g. 1100 -> 1080) during the measured entry distance.
  // Heading corrections are stronger ONLY during the straight front-wall
  // alignment following the first 90-degree parking turn.
  const float PARK_HEADING_HOLD_KP = 0.052f;
  const float PARK_FRONT_ALIGN_HEADING_KP = 0.095f;
  float parkingHoldSteer = 0.0f;
  uint32_t parkingHoldLastServoMs = 0;
  auto holdHeading = [&](int pwm, double heading, float headingKp) {
    read_gyro_data();
    const float error = (float)(heading - gz);
    const float driveSign = pwm < 0 ? -1.0f : 1.0f;
    const float desired = driveSign * constrain(error * headingKp, -0.78f, 0.78f);
    const uint32_t now = millis();
    if ((uint32_t)(now - parkingHoldLastServoMs) >= 20UL) {
      parkingHoldLastServoMs = now;
      parkingHoldSteer += constrain(desired - parkingHoldSteer, -0.085f, 0.085f);
    }
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

  // After either 90-degree turn, trim the stopped gyro heading in whichever
  // DRIVE direction has safe clearance. A reverse arc must reverse the servo
  // sign as well; otherwise it changes yaw in the WRONG direction.
  const double PARK_SETTLED_IDEAL_DEG = 0.75;
  const double PARK_SETTLED_ACCEPT_DEG = 2.00;
  const int PARK_CORRECTION_FRONT_REVERSE_MM = 380;  // Switch to reverse before approaching the 400mm front target.
  const int PARK_CORRECTION_REAR_MIN_MM = 220;
  const int PARK_CORRECTION_PWM = 38;  // Correction only; entry / alignment / turns remain PWM 35.

  // Read the nearest fresh central return. This is a CLEARANCE guard, not
  // the R2C3-only front distance controller used in the next parking stage.
  auto correctionClearanceMM = [&](DistanceDir dir) -> int {
    tof_poll_one((uint8_t)dir);
    const int r2 = tof_zone_mm(dir, 2, 3);
    const int r3 = tof_zone_mm(dir, 3, 3);
    if (r2 > 0 && r3 > 0) return min(r2, r3);
    if (r2 > 0) return r2;
    if (r3 > 0) return r3;
    return -1;
  };

  auto finishTurnHeading = [&](double target, const char *stage) -> bool {
    settle(190);
    if (!gyroAvailable || !isfinite(gz)) return false;
    double error = target - gz;
    Serial.print("FINAL PARK SETTLED "); Serial.print(stage);
    Serial.print(" target="); Serial.print(target, 3);
    Serial.print(" yaw="); Serial.print(gz, 3);
    Serial.print(" error="); Serial.println(error, 3);
    if (fabs(error) < PARK_SETTLED_ACCEPT_DEG) {
      // Don't waste space on a small residual. The straight alignment
      // controls the exact target heading with the existing higher KP.
      Serial.print("FINAL PARK ACCEPTED "); Serial.print(stage);
      Serial.print(" settled residual="); Serial.println(error, 3);
      return true;
    }
    if (fabs(error) > 9.0) return false;

    for (uint8_t attempt = 0; attempt < 2; ++attempt) {
      read_gyro_data();
      if (!gyroAvailable || !isfinite(gz)) return false;
      error = target - gz;
      if (fabs(error) < PARK_SETTLED_ACCEPT_DEG) return true;
      if (fabs(error) > 9.0) return false;

      // Use fresh front/rear ranging to select a safe correction direction.
      // If front is too close, reverse ONLY with verified rear clearance.
      int frontMM = correctionClearanceMM(FRONT_DIR);
      int rearMM = correctionClearanceMM(BACK_DIR);
      const uint32_t sampleStart = millis();
      // When the front is too close, wait briefly for a verified rear
      // reading instead of declaring the correction blocked immediately.
      while (frontMM < PARK_CORRECTION_FRONT_REVERSE_MM &&
             rearMM < PARK_CORRECTION_REAR_MIN_MM &&
             (uint32_t)(millis() - sampleStart) < 460UL) {
        delay(12);
        frontMM = correctionClearanceMM(FRONT_DIR);
        rearMM = correctionClearanceMM(BACK_DIR);
      }
      int driveDir = frontMM >= PARK_CORRECTION_FRONT_REVERSE_MM ? 1 :
                           rearMM >= PARK_CORRECTION_REAR_MIN_MM ? -1 : 0;
      Serial.print("FINAL PARK SETTLE DIRECTION "); Serial.print(stage);
      Serial.print(" front="); Serial.print(frontMM);
      Serial.print(" rear="); Serial.print(rearMM);
      Serial.print(" motion="); Serial.println(driveDir > 0 ? "FORWARD" :
                                                driveDir < 0 ? "BACKWARD" : "BLOCKED");
      if (!driveDir) {
        Serial.println("FINAL PARK correction: neither direction confirmed safe");
        break;
      }

      const int correctionDir = error > 0.0 ? 1 : -1;
      const double startingMagnitude = fabs(error);
      int32_t lastTicks = encoder_ticks;
      float correctionTravelMM = 0.0f;
      uint32_t lastEncoderProgress = millis();
      const uint32_t startedCorrection = millis();
      float fineSteering = 0.0f;
      double bestMagnitude = startingMagnitude;
      uint32_t lastYawProgress = startedCorrection;
      uint8_t unsafeFrames = 0;
      uint8_t directionSwitches = 0;
      uint32_t lastClearancePoll = 0;
      DistanceDir travelSensor = driveDir > 0 ? FRONT_DIR : BACK_DIR;
      int minClearance = driveDir > 0 ? PARK_CORRECTION_FRONT_REVERSE_MM :
                                       PARK_CORRECTION_REAR_MIN_MM;
      uint32_t lastClearanceFrame = tof_last_frame_ms(travelSensor);
      uint32_t lastClearanceGoodMs = startedCorrection;

      // One correction can switch direction while it is still running.
      // Do not waste a correction attempt on a front-wall clearance event.
      while ((uint32_t)(millis() - startedCorrection) < 3300UL) {
        read_gyro_data();
        if (!gyroAvailable || !isfinite(gz)) break;
        error = target - gz;
        const double remaining = fabs(error);
        const double rateToward = (double)correctionDir * gyroRateDps;
        const double lead = max(0.0, rateToward) * 0.08;
        if (error * correctionDir <= 0.0 ||
            remaining < 0.38 ||
            (remaining < 0.65 && rateToward > 1.4)) break;
        if (remaining > startingMagnitude + 2.5) break;
        if (remaining + 0.06 < bestMagnitude) {
          bestMagnitude = remaining;
          lastYawProgress = millis();
        }
        if ((uint32_t)(millis() - lastYawProgress) > 1600UL) break;

        const int32_t ticksNow = encoder_ticks;
        correctionTravelMM += (float)abs(ticksNow - lastTicks) * FINAL_MM_PER_TICK;
        if (ticksNow != lastTicks) lastEncoderProgress = millis();
        lastTicks = ticksNow;
        if (correctionTravelMM > 180.0f) break;

        // Monitor fresh ToF frames in the direction of travel.
        // Two low-clearance frames prevent reacting to one bad return.
        if ((uint32_t)(millis() - lastClearancePoll) >= 35UL) {
          lastClearancePoll = millis();
          const int mm = correctionClearanceMM(travelSensor);
          const uint32_t frameMs = tof_last_frame_ms(travelSensor);
          if (frameMs && frameMs != lastClearanceFrame) {
            lastClearanceFrame = frameMs;
            if (mm > 0) {
              lastClearanceGoodMs = millis();
              unsafeFrames = mm < minClearance ? (uint8_t)(unsafeFrames + 1) : 0;
            }
          }

          const bool tooClose = unsafeFrames >= 2;
          const bool sensorStale = (uint32_t)(millis() - lastClearanceGoodMs) > 280UL;
          if (tooClose || sensorStale) {
            move(0);
            steer_parking(0.0);
            Serial.print("FINAL PARK SETTLE CLEARANCE "); Serial.print(stage);
            Serial.print(driveDir > 0 ? " FRONT" : " REAR");
            Serial.println(tooClose ? " TOO CLOSE" : " SENSOR STALE");

            // Try the opposite direction within THIS correction pass.
            // Require a newly sampled, valid rear/front frame before moving.
            bool changedDirection = false;
            if (directionSwitches < 1) {
              const DistanceDir oppositeSensor = driveDir > 0 ? BACK_DIR : FRONT_DIR;
              const int oppositeMin = driveDir > 0 ? PARK_CORRECTION_REAR_MIN_MM :
                                                  PARK_CORRECTION_FRONT_REVERSE_MM;
              const uint32_t previousOppositeFrame = tof_last_frame_ms(oppositeSensor);
              const uint32_t checkStarted = millis();
              int oppositeMM = -1;
              while ((uint32_t)(millis() - checkStarted) < 360UL) {
                const int candidate = correctionClearanceMM(oppositeSensor);
                const uint32_t updated = tof_last_frame_ms(oppositeSensor);
                if (updated && updated != previousOppositeFrame) {
                  oppositeMM = candidate;
                  break;
                }
                read_gyro_data();
                delay(6);
              }
              if (oppositeMM >= oppositeMin) {
                driveDir = -driveDir;
                travelSensor = oppositeSensor;
                minClearance = oppositeMin;
                lastClearanceFrame = tof_last_frame_ms(travelSensor);
                lastClearanceGoodMs = millis();
                lastClearancePoll = millis();
                lastEncoderProgress = millis();
                lastYawProgress = millis();
                bestMagnitude = fabs(target - gz);
                unsafeFrames = 0;
                fineSteering = 0.0f;
                ++directionSwitches;
                changedDirection = true;
                Serial.print("FINAL PARK SETTLE SWITCH "); Serial.print(stage);
                Serial.print(" -> "); Serial.print(driveDir > 0 ? "FORWARD" : "REVERSE");
                Serial.print(" clearance="); Serial.println(oppositeMM);
              } else {
                Serial.print("FINAL PARK SETTLE: opposite direction not verified safe, mm=");
                Serial.println(oppositeMM);
              }
            }
            if (changedDirection) continue;
            break;  // Never drive toward a blocked or unverified space.
          }
        }

        // Slightly faster angular correction than the old 0.23..0.58 steer,
        // without changing the normal PWM 35 parking speeds.
        double requestedMag = constrain(0.32 + 0.068 * max(0.0, remaining - lead),
                                        0.32, 0.72);
        if (remaining < 1.1 && rateToward > 2.4)
          requestedMag = min(requestedMag, 0.20);
        // Reverse motion reverses the steering-to-yaw direction.
        const float requestedSteer = (float)(driveDir * correctionDir * requestedMag);
        fineSteering += constrain(requestedSteer - fineSteering, -0.085f, 0.085f);
        steer_parking(fineSteering);
        move(driveDir * PARK_CORRECTION_PWM); // +/-38 PWM, below the 40 cap.
        if ((uint32_t)(millis() - lastEncoderProgress) > 900UL) {
          Serial.println("FINAL PARK correction stopped: encoder not moving");
          break;
        }
        delay(5);
      }
      move(0);
      steer_parking(0.0);
      settle(180);
      error = target - gz;
      Serial.print("FINAL PARK CORRECTED "); Serial.print(stage);
      Serial.print(" try="); Serial.print(attempt + 1);
      Serial.print(" residual="); Serial.println(error, 3);
      if (gyroAvailable && isfinite(error) && fabs(error) <= PARK_SETTLED_IDEAL_DEG)
        return true;
      // If the front forced us to stop, the next attempt can choose reverse.
    }
    error = target - gz;
    if (gyroAvailable && isfinite(error) && fabs(error) < PARK_SETTLED_ACCEPT_DEG) {
      Serial.print("FINAL PARK acceptable residual, correct on straight: ");
      Serial.println(error, 3);
      return true;
    }
    return false;
  };

  const char *parkingTurnFailure = "parking turn did not settle";
  // Restore the older, working continuous-turn controller. It starts
  // motor movement while steering ramps from center rather than blocking
  // start-up on a new encoder/yaw plausibility gate.
  auto turnTo = [&](double target, const char *stage) -> bool {
    telemetry_parking_stage(stage, target);
    read_gyro_data();  // The wheels are already rolling at PWM 35.
    const double entry = gz;
    const int turnDir = target >= entry ? 1 : -1;
    const uint32_t started = millis();
    uint32_t lastLog = started;
    uint32_t lastImprovement = started;
    uint32_t lastFrontCheck = 0;
    uint32_t lastTurnServoMs = started;
    uint32_t lastEncoderMovingMs = started;
    int32_t lastTicks = encoder_ticks;
    float traveledMM = 0.0f;
    double bestRemaining = fabs(target - entry);
    float turnSteer = 0.0f;
    float turnFilteredRate = (float)gyroRateDps;

    const float maxTravelMM = finalYawCorrection ? 430.0f : 950.0f;
    const uint32_t maxTimeMs = finalYawCorrection ? 18000UL : 22000UL;
    // Start braking yaw by straightening the servo, but keep applying
    // correction until the target is REALLY reached. Final acceptance is
    // +/-2.5 deg after the wheels have stopped, not at the release point.
    const double fineStopDeg = 0.35;
    Serial.print("FINAL PARK TURN START ");
    Serial.print(stage);
    Serial.print(" yaw=");
    Serial.print(gz, 2);
    Serial.print(" target=");
    Serial.println(target, 2);

    while ((uint32_t)(millis() - started) < maxTimeMs) {
      read_gyro_data();
      if (!gyroAvailable) break;
      // Refresh the front sensor during the turn. This lets the subsequent
      // R2C3 approach decide whether it is safe to keep rolling immediately.
      if ((uint32_t)(millis() - lastFrontCheck) >= 45UL) {
        lastFrontCheck = millis();
        tof_poll_one((uint8_t)FRONT_DIR);
      }
      const double error = target - gz;
      const double remaining = fabs(error);
      if (!isfinite(remaining) || remaining > 230.0) break;

      const int32_t nowTicks = encoder_ticks;
      traveledMM += (float)abs(nowTicks - lastTicks) * FINAL_MM_PER_TICK;
      if (nowTicks != lastTicks) lastEncoderMovingMs = millis();
      lastTicks = nowTicks;
      if (traveledMM > maxTravelMM) {
        Serial.println("FINAL PARK TURN: maximum travel reached");
        break;
      }

      turnFilteredRate = 0.72f * turnFilteredRate + 0.28f * (float)constrain(gyroRateDps, -45.0, 45.0);
      const double rateToward = (double)turnDir * turnFilteredRate;
      const bool crossedTarget = error * turnDir <= 0.0;

      // NORMAL HANDOFF: reach heading while still rolling, then let the
      // following straight stage hold the exact gyro target. Do not park in
      // place for a gyro settle between corner / front / next corner.
      // Only accept a rolling handoff with a low yaw rate and <2 deg error.
      if ((remaining <= 1.15 && fabs(turnFilteredRate) <= 6.0) ||
          (crossedTarget && remaining < 1.75 && fabs(turnFilteredRate) <= 6.5)) {
        steer_parking(0.0);
        move(PARK_TURN_PWM);
        pid_error = 0;
        pid_last_error = 0;
        Serial.print("FINAL PARK ROLLING TURN COMPLETE ");
        Serial.print(stage);
        Serial.print(" yaw="); Serial.print(gz, 2);
        Serial.print(" residual="); Serial.println(target - gz, 2);
        return true;
      }

      // A missed target or excessive yaw rate still uses the existing
      // bounded correction and clearance checks, including safe reversing.
      if (crossedTarget ||
          (remaining <= fineStopDeg && fabs(turnFilteredRate) <= 2.6) ||
          (remaining <= 0.13 && fabs(turnFilteredRate) <= PARK_STOP_RATE_DPS)) {
        steer_parking(0.0);
        move(0);
        const uint32_t stopStarted = millis();
        while ((uint32_t)(millis() - stopStarted) < 180UL) {
          read_gyro_data();
          delay(4);
        }
        const double settledError = target - gz;
        Serial.print("FINAL PARK TURN STOP ");
        Serial.print(stage);
        Serial.print(" actual=");
        Serial.print(gz, 3);
        Serial.print(" error=");
        Serial.print(settledError, 3);
        Serial.print(" rate=");
        Serial.println(gyroRateDps, 2);
        // Always inspect the stationary heading. Precise correction is
        // performed only when a real 90-degree yaw change was observed.
        if (fabs(settledError) <= 9.0) {
          const bool aligned = finishTurnHeading(target, stage);
          if (aligned) { pid_error = 0; pid_last_error = 0; }
          return aligned;
        }
        return false;
      }

      if (remaining + 0.06 < bestRemaining) {
        bestRemaining = remaining;
        lastImprovement = millis();
      }
      if ((uint32_t)(millis() - lastImprovement) > 3800UL) {
        Serial.println("FINAL PARK TURN: gyro not approaching target");
        break;
      }

      if (finalYawCorrection) {
        const int r2 = tof_zone_mm(FRONT_DIR, 2, 3);
        const int r3 = tof_zone_mm(FRONT_DIR, 3, 3);
        if ((r2 > 0 && r2 < 180) || (r3 > 0 && r3 < 180)) {
          Serial.println("FINAL PARK TURN: not enough safe front clearance");
          break;
        }
      }

      // The yaw-rate lead anticipates how much further the robot will turn
      // while the servo is returning to center. It must not become a hard
      // release: if yaw slows too early, steering builds again continuously.
      const double headingLead = max(0.0, rateToward) * 0.23;
      const double controlError = max(0.0, remaining - headingLead);
      double steerMagnitude;
      if (controlError > 22.0) {
        steerMagnitude = 1.00;  // full parking servo lock for both main 90-degree arcs
      } else if (controlError > 7.0) {
        steerMagnitude = 0.23 + (controlError - 7.0) * (0.77 / 15.0);
      } else {
        // Gentle final approach. If still rotating quickly, center the
        // wheels; if it settles short, feed in a little SAME-DIRECTION steer.
        const double allowedRate = 0.65 + remaining * 1.40;
        if (rateToward > allowedRate) {
          steerMagnitude = 0.0;
        } else {
          steerMagnitude = constrain(controlError * 0.085 + 0.025,
                                     0.0, 0.27);
          if (remaining > fineStopDeg && fabs(rateToward) < 0.8)
            steerMagnitude = max(steerMagnitude, 0.14);
        }
      }

      const float requested = (float)(turnDir * steerMagnitude);
      const uint32_t nowMs = millis();
      if ((uint32_t)(nowMs - lastTurnServoMs) >= 20UL) {
        lastTurnServoMs = nowMs;
        // Release steering promptly when yaw is too fast, but build it
        // gently so the steering servo does not oscillate around the goal.
        const float maxStep = fabs(requested) < fabs(turnSteer) ? 0.10f : 0.055f;
        turnSteer += constrain(requested - turnSteer, -maxStep, maxStep);
      }
      steer_parking(turnSteer);
      // Hold 35 PWM throughout. If the wheels stall under steering load, abort.
      const uint32_t wheelIdleMs = (uint32_t)(millis() - lastEncoderMovingMs);
      const int pwm = PARK_TURN_PWM;  // Constant 35, including start and stall attempts
      if (wheelIdleMs > 1300UL && remaining > 7.0) {
        Serial.println("FINAL PARK TURN: motor/encoder stalled under steering load");
        break;
      }
      move(pwm);

      if ((uint32_t)(millis() - lastLog) > 350UL) {
        lastLog = millis();
        Serial.print("FINAL PARK TURN ");
        Serial.print(stage);
        Serial.print(" gyro=");
        Serial.print(gz, 2);
        Serial.print(" target=");
        Serial.print(target, 2);
        Serial.print(" error=");
        Serial.print(error, 2);
        Serial.print(" rate=");
        Serial.print(turnFilteredRate, 2);
        Serial.print(" steer=");
        Serial.print(turnSteer, 3);
        Serial.print(" pwm=");
        Serial.println(pwm);
      }
      delay(5);
    }

    settle(180);
    const double remainingAfterStop = target - gz;
    // Finish both corners by checking the stopped angle, never by quietly
    // accepting an error of several degrees.
    if (gyroAvailable && isfinite(remainingAfterStop) &&
        fabs(remainingAfterStop) <= 9.0 && finishTurnHeading(target, stage)) {
      pid_error = 0;
      pid_last_error = 0;
      return true;
    }
    Serial.print("FINAL PARK TURN FAILED ");
    Serial.print(stage);
    Serial.print(" gyro=");
    Serial.print(gz, 3);
    Serial.print(" target=");
    Serial.print(target, 3);
    Serial.print(" error=");
    Serial.println(remainingAfterStop, 3);
    return false;
  };

  // All final FRONT ALIGNMENT decisions use ONLY R2C3. The most common
  // distance cluster in nine NEW frames wins, not an average of good and
  // reflected values. For example [500,500,200,500] stays near 500mm.
  // Moving and stationary measurements follow the SAME anti-spike policy.
  bool frontR2OnlyActive = false;
  static const uint8_t FRONT_MODE_WINDOW = 9;
  static const float FRONT_MODE_CLUSTER_MM = 45.0f;
  static const float FRONT_MODE_NEW_READING_MM = 65.0f;
  static const float FRONT_MODE_MAX_STEP_MM = 85.0f;
  int frontModeHistory[FRONT_MODE_WINDOW] = {};
  uint8_t frontModeCount = 0;
  uint8_t frontModeNext = 0;
  float frontModeTrustedMM = -1.0f;
  bool frontModeOutlierHold = false;
  uint32_t frontModeOutlierSince = 0;

  auto readFront = [&](uint32_t &lastMs, float &distanceMM) -> bool {
    const uint32_t previous = lastMs;
    int r2 = -1, r3 = -1;
    if (frontR2OnlyActive) {
      // A fresh, valid R2C3 reading is the ONLY front alignment source.
      // R3C3 is not a fallback and does not participate in this filter.
      tof_read_new_zone(FRONT_DIR, lastMs, 2, 3, r2);
    } else {
      tof_read_new_pair(FRONT_DIR, lastMs, r2, r3);
    }
    if (lastMs == previous) return false;  // never reuse a cached frame
    const bool good2 = r2 > 45 && r2 <= 3500;
    const bool good3 = r3 > 45 && r3 <= 3500;
    if (!frontR2OnlyActive) {
      if (!good2 && !good3) return false;
      distanceMM = (float)(good2 && good3 ? min(r2, r3) :
                           (good2 ? r2 : r3));
      return true;
    }
    if (!good2) return false;

    frontModeHistory[frontModeNext] = r2;
    frontModeNext = (frontModeNext + 1) % FRONT_MODE_WINDOW;
    if (frontModeCount < FRONT_MODE_WINDOW) ++frontModeCount;
    if (frontModeCount < 3) return false;

    // Find the strongest group of similar measurements. No dynamic memory.
    uint8_t bestCount = 0;
    float bestMean = -1.0f;
    float bestDistanceFromTrusted = 1.0e9f;
    for (uint8_t i = 0; i < frontModeCount; ++i) {
      const int anchor = frontModeHistory[i];
      uint8_t matches = 0;
      float total = 0.0f;
      for (uint8_t j = 0; j < frontModeCount; ++j) {
        const int v = frontModeHistory[j];
        if (abs(v - anchor) <= FRONT_MODE_CLUSTER_MM) {
          ++matches;
          total += (float)v;
        }
      }
      if (matches < 3) continue;
      const float mean = total / (float)matches;
      const float oldDistance = frontModeTrustedMM > 0.0f ?
                                  fabs(mean - frontModeTrustedMM) : 0.0f;
      if (matches > bestCount ||
          (matches == bestCount && oldDistance < bestDistanceFromTrusted)) {
        bestCount = matches;
        bestMean = mean;
        bestDistanceFromTrusted = oldDistance;
      }
    }
    // While approaching a real wall, successive ToF distances should form
    // a smooth trend. Use the median of the last three if they move in the
    // SAME direction with small steps. Otherwise favor the nine-frame mode.
    // This avoids a 9-frame delay making the robot overshoot the wall.
    float chosenMM = bestMean;
    if (frontModeCount >= 3 && bestCount >= 3) {
      const int newest = frontModeHistory[(frontModeNext + FRONT_MODE_WINDOW - 1) % FRONT_MODE_WINDOW];
      const int middle = frontModeHistory[(frontModeNext + FRONT_MODE_WINDOW - 2) % FRONT_MODE_WINDOW];
      const int oldest = frontModeHistory[(frontModeNext + FRONT_MODE_WINDOW - 3) % FRONT_MODE_WINDOW];
      const bool smoothDown = oldest - middle >= 3 && middle - newest >= 3 &&
                              oldest - newest <= 65;
      const bool smoothUp = middle - oldest >= 3 && newest - middle >= 3 &&
                            newest - oldest <= 65;
      if ((smoothDown || smoothUp) && fabs((float)middle - bestMean) <= 70.0f)
        chosenMM = (float)middle;
    }
    // A one- or two-frame short reflection is never accepted as a wall.
    const bool clusterReady = bestCount >= 3;
    const bool incomingBelongs = clusterReady &&
      fabs((float)r2 - chosenMM) <= FRONT_MODE_NEW_READING_MM;
    const bool plausibleStep = frontModeTrustedMM < 0.0f ||
      (clusterReady && fabs(chosenMM - frontModeTrustedMM) <= FRONT_MODE_MAX_STEP_MM);
    if (!clusterReady || !incomingBelongs || !plausibleStep) {
      if (!frontModeOutlierHold) frontModeOutlierSince = millis();
      frontModeOutlierHold = true;
      return false;  // main front loop brakes until the real cluster returns
    }
    frontModeTrustedMM = chosenMM;
    frontModeOutlierHold = false;
    distanceMM = chosenMM;
    return true;
  };

  // A single front-zone frame can briefly jump from 530mm to below target,
  // causing V4 to choose the WRONG drive direction and then report overshoot.
  // While stopped, require at least 3 agreeing readings from up to 5 NEW
  // frames; outliers cannot set the forward/reverse direction.
  auto readStableFront = [&](uint32_t &lastMs, float &stableMM,
                             uint32_t timeoutMs) -> bool {
    float samples[5] = {};
    uint8_t collected = 0;
    const uint32_t started = millis();
    while (collected < 5 && (uint32_t)(millis() - started) < timeoutMs) {
      float mm = -1.0f;
      if (readFront(lastMs, mm)) {
        samples[collected++] = mm;
      }
      read_gyro_data();
      delay(4);
    }
    if (collected < 3) return false;
    for (uint8_t i = 1; i < collected; ++i) {
      const float v = samples[i];
      uint8_t j = i;
      while (j > 0 && samples[j - 1] > v) {
        samples[j] = samples[j - 1];
        --j;
      }
      samples[j] = v;
    }
    const float median = samples[collected / 2];
    uint8_t agreeing = 0;
    for (uint8_t i = 0; i < collected; ++i)
      if (fabs(samples[i] - median) <= 35.0f) ++agreeing;
    if (agreeing < 3) {
      Serial.println("FINAL FRONT unstable readings: waiting, no movement");
      return false;
    }
    stableMM = median;
    return true;
  };

  // ========================================================================
  // ZONE 1 / 3: approach, first turn, verified front distance, second turn
  // ========================================================================
  finalRapidCornerActive = false;
  telemetry_parking_stage("ENTRY", side12Heading);
  settle(150);
  // Drive each parking entry straight on the 12-corner ABSOLUTE target,
  // not on a new target captured from whatever yaw the chassis arrived at.
  // Keep the raw yaw only for diagnostics and for the turning controller.
  current_angle_gyro = side12Heading;
  read_gyro_data();
  const double liveEntryHeading = gz;
  Serial.print("FINAL PARK ABSOLUTE HEADINGS: entry=");
  Serial.print(side12Heading, 2);
  Serial.print(" first="); Serial.print(fixedFirstTarget, 2);
  Serial.print(" second="); Serial.print(fixedSecondTarget, 2);
  Serial.print(" measured entry="); Serial.println(liveEntryHeading, 2);
  // Do not gate this movement on front ToF readings. The old V2 code
  // returned early if the front sensor was unavailable or read <=420 mm,
  // leaving the robot parked before its first 90-degree turn.
  // STEP 1: measured forward entry on absolute heading +/-1080.
  // Use the same encoder and gyro logic for both parking directions.
  const float entryDistanceCM = turn_direction == -1 ? 25.0f : 15.0f;
  telemetry_parking_stage("ENTRY STRAIGHT 1080", side12Heading);
  noInterrupts(); encoder_ticks = 0; interrupts();
  parkingHoldSteer = 0.0f;
  parkingHoldLastServoMs = 0;
  const uint32_t entryStartMs = millis();
  uint32_t entryLastProgressMs = entryStartMs;
  float entryLastMM = 0.0f;
  uint32_t entryLastFrontFrameMs = tof_last_frame_ms(FRONT_DIR);
  uint8_t entryCloseFrames = 0;
  bool entryFault = false;
  bool entryBlocked = false;
  // This move is a finite distance, never an indefinite yaw-trimming loop.
  while (read_cm() < entryDistanceCM) {
    read_gyro_data();
    if (!gyroAvailable || !isfinite(gz)) { entryFault = true; break; }
    const float mm = read_cm() * 10.0f;
    if (mm > entryLastMM + 1.0f) {
      entryLastMM = mm;
      entryLastProgressMs = millis();
    }
    if ((uint32_t)(millis() - entryStartMs) > 11500UL ||
        (uint32_t)(millis() - entryLastProgressMs) > 2200UL) {
      entryFault = true; break;
    }
    // Stop only for a repeated very close front return, not a missing reading.
    tof_poll_one((uint8_t)FRONT_DIR);
    const uint32_t frameMs = tof_last_frame_ms(FRONT_DIR);
    if (frameMs && frameMs != entryLastFrontFrameMs) {
      entryLastFrontFrameMs = frameMs;
      const int r2 = tof_zone_mm(FRONT_DIR, 2, 3);
      const int r3 = tof_zone_mm(FRONT_DIR, 3, 3);
      if (r2 > 0 && r3 > 0 && r2 < 170 && r3 < 170)
        ++entryCloseFrames;
      else
        entryCloseFrames = 0;
      if (entryCloseFrames >= 2) { entryBlocked = true; break; }
    }
    const int pwm = 35;  // Fixed speed for the entire parking entry
    holdHeading(pwm, side12Heading, PARK_HEADING_HOLD_KP);
    delay(5);
  }
  // Transition directly into the first 90-degree turn at the same PWM.
  // Stop only if the entry safety checks failed.
  read_gyro_data();
  Serial.print("FINAL PARK ENTRY COMPLETE: target="); Serial.print(side12Heading, 2);
  Serial.print(" actual="); Serial.print(gz, 2);
  Serial.print(" traveledCM="); Serial.println(read_cm(), 2);
  if (entryFault || entryBlocked) {
    fail(entryBlocked ? "parking entry front obstacle" :
                         "parking entry gyro or encoder stalled");
    return;
  }

  // Keep the heading grid. +1: start at +1080 and turn to +990.
  // -1: start at -1080 and turn to -990. Do NOT recalculate the 90-degree
  // target as measuredYaw +/- 90: that changes the intended parking angle.
  pid_error = 0;
  pid_last_error = 0;
  read_gyro_data();
  const double initialHeading = gz;
  const double firstTarget = fixedFirstTarget;
  Serial.print("FINAL PARK FIRST TURN: straight target=");
  Serial.print(side12Heading, 2);
  Serial.print(" measured yaw=");
  Serial.print(initialHeading, 2);
  Serial.print(" absolute first target=");
  Serial.print(firstTarget, 2);
  Serial.print(" requested gyro delta=");
  Serial.println(firstTarget - initialHeading, 2);
  current_angle_gyro = firstTarget;
  if (!turnTo(firstTarget, "FIRST 90")) {
    fail(parkingTurnFailure);
    return;
  }
  // FRONT WALL ALIGNMENT: one sensor cell, one target, no 9-frame mode gate.
  // Read only fresh R2C3 (4x4 sensor row 2, column 3). A rolling median of
  // three valid frames eliminates isolated glitches without rejecting a
  // steady wall. Stay at PWM 35 in both directions and keep firstTarget yaw.
  // A trusted R2C3 target can hand off to the second turn while rolling.
  // Unexpected/suspicious measurements or reversal still stop for safety.
  frontR2OnlyActive = true;  // Restore normal side-search readFront after alignment.
  current_angle_gyro = firstTarget;
  telemetry_parking_stage("FRONT R2C3 ALIGN", firstTarget);
  const float FRONT_TOLERANCE_MM = 8.0f; // closest practical band with 4x4 ToF
  const uint32_t FRONT_ALIGN_TIMEOUT_MS = 45000UL;
  const uint32_t FRONT_INVALID_PAUSE_MS = 220UL;
  const uint32_t FRONT_INVALID_FAIL_MS = 3500UL;
  const int FRONT_ALIGN_PWM = 35;
  int frontHistory[3] = {0, 0, 0};
  uint8_t frontHistoryCount = 0;
  uint8_t frontHistoryIndex = 0;
  uint32_t r2LastFrame = tof_last_frame_ms(FRONT_DIR);
  uint32_t lastGoodR2Ms = millis();
  uint32_t lastAcceptedR2Ms = millis();
  uint32_t frontStartMs = millis();
  uint32_t frontLogMs = 0;
  uint32_t targetStopMs = 0;  // Used only by emergency reset diagnostics.
  uint32_t lastEncoderChangeMs = millis();
  int32_t previousAlignTicks = encoder_ticks;
  int activeFrontDirection = 0;
  int candidateDirection = 0;
  uint8_t candidateFrames = 0;
  uint8_t targetStableFrames = 0;
  uint8_t abruptChangeFrames = 0;
  uint8_t rawDangerFrames = 0;
  uint8_t rearCloseFrames = 0;
  uint32_t rearFrameMs = tof_last_frame_ms(BACK_DIR);
  bool alignedFront = false;
  float verifiedFront = -1.0f;
  float frontFiltered = -1.0f;
  float frontTrusted = -1.0f;
  int latestR2Raw = -1;
  uint8_t frontEmergencyRecoveries = 0;

  // Two fresh R2C3 readings under 110 mm are an emergency CLOSE event,
  // not a reason to abandon parking when there is verified space behind.
  // Back away on the FIRST-TURN heading, then restart the usual R2C3
  // forward/reverse alignment. Never reverse without fresh rear clearance.
  auto recoverFrontEmergency = [&]() -> bool {
    move(0);
    steer_parking(0.0);
    telemetry_parking_stage("FRONT TOO CLOSE - REVERSE", firstTarget);
    Serial.println("FINAL FRONT EMERGENCY: checking rear before reversing");
    const int REAR_STOP_MM = 220;
    const uint32_t REAR_FRESH_TIMEOUT_MS = 350UL;
    const float MAX_REVERSE_MM = 650.0f;
    const uint32_t MAX_REVERSE_MS = 18000UL;
    const uint32_t precheckStart = millis();
    uint32_t rearSeenMs = tof_last_frame_ms(BACK_DIR);
    uint8_t rearSafeFrames = 0;

    // Confirm two DIFFERENT fresh rear frames before applying reverse power.
    while ((uint32_t)(millis() - precheckStart) < 950UL) {
      int r2 = -1, r3 = -1;
      const uint32_t previous = rearSeenMs;
      const bool valid = tof_read_new_pair(BACK_DIR, rearSeenMs, r2, r3);
      if (rearSeenMs != previous) {
        if (!valid || min(r2, r3) <= REAR_STOP_MM) {
          Serial.println("FINAL FRONT EMERGENCY: rear is close or invalid");
          return false;
        }
        if (rearSafeFrames < 2) ++rearSafeFrames;
        if (rearSafeFrames >= 2) break;
      }
      delay(5);
    }
    if (rearSafeFrames < 2) {
      Serial.println("FINAL FRONT EMERGENCY: no two fresh safe rear frames");
      return false;
    }

    const uint32_t reverseStart = millis();
    uint32_t lastSafeRearMs = reverseStart;
    uint32_t lastValidFrontMs = reverseStart;
    uint32_t lastEncoderMovementMs = reverseStart;
    uint32_t frontSeenMs = r2LastFrame;
    int32_t startTicks = encoder_ticks;
    int32_t lastTicks = startTicks;
    uint8_t recoveredFrames = 0;
    int lastRecoveredFrontMM = -1;
    bool recovered = false;

    while ((uint32_t)(millis() - reverseStart) < MAX_REVERSE_MS) {
      read_gyro_data();
      if (!gyroAvailable || !isfinite(gz)) {
        Serial.println("FINAL FRONT EMERGENCY: lost gyro while reversing");
        break;
      }
      const int32_t currentTicks = encoder_ticks;
      const float reverseTravelMM = (float)abs(currentTicks - startTicks) * FINAL_MM_PER_TICK;
      if (reverseTravelMM >= MAX_REVERSE_MM) {
        Serial.println("FINAL FRONT EMERGENCY: reverse distance limit");
        break;
      }
      if (currentTicks != lastTicks) {
        lastTicks = currentTicks;
        lastEncoderMovementMs = millis();
      }
      if ((uint32_t)(millis() - lastEncoderMovementMs) > 1700UL) {
        Serial.println("FINAL FRONT EMERGENCY: reverse encoder not moving");
        break;
      }

      // Keep checking REAR while the robot moves backward.
      int rear2 = -1, rear3 = -1;
      const uint32_t beforeRear = rearSeenMs;
      const bool rearValid = tof_read_new_pair(BACK_DIR, rearSeenMs, rear2, rear3);
      if (rearSeenMs != beforeRear) {
        if (!rearValid || min(rear2, rear3) <= REAR_STOP_MM) {
          Serial.println("FINAL FRONT EMERGENCY: rear clearance limit reached");
          break;
        }
        lastSafeRearMs = millis();
      }
      if ((uint32_t)(millis() - lastSafeRearMs) > REAR_FRESH_TIMEOUT_MS) {
        Serial.println("FINAL FRONT EMERGENCY: stale rear ranging");
        break;
      }

      // Stop reversing when fresh R2C3 readings consistently return to the
      // desired approach band. Do not trust a single 500mm reflection spike.
      int frontMM = -1;
      if (tof_read_new_zone(FRONT_DIR, frontSeenMs, 2, 3, frontMM) && frontMM > 0) {
        lastValidFrontMs = millis();
        if (frontMM >= (int)(FRONT_TARGET_MM - 20.0f) &&
            frontMM <= (int)(FRONT_TARGET_MM + 180.0f) &&
            reverseTravelMM >= 70.0f &&
            (lastRecoveredFrontMM < 0 || abs(frontMM - lastRecoveredFrontMM) <= 65)) {
          if (recoveredFrames < 4) ++recoveredFrames;
        } else {
          recoveredFrames = 0;
        }
        lastRecoveredFrontMM = frontMM;
        if (recoveredFrames >= 4) {
          recovered = true;
          Serial.print("FINAL FRONT EMERGENCY: recovered R2C3=");
          Serial.print(frontMM);
          Serial.print(" reverse_mm=");
          Serial.println(reverseTravelMM, 1);
          break;
        }
      }
      if ((uint32_t)(millis() - lastValidFrontMs) > 600UL) {
        Serial.println("FINAL FRONT EMERGENCY: front ranging lost while reversing");
        break;
      }

      holdHeading(-FRONT_ALIGN_PWM, firstTarget, PARK_FRONT_ALIGN_HEADING_KP);
      delay(5);
    }

    move(0);
    steer_parking(0.0);
    settle(120);
    if (!recovered) return false;

    // Discard measurements from BEFORE the recovery. The normal R2C3
    // controller must verify the 400mm target while stopped from scratch.
    frontHistory[0] = frontHistory[1] = frontHistory[2] = 0;
    frontHistoryCount = 0;
    frontHistoryIndex = 0;
    frontFiltered = -1.0f;
    frontTrusted = -1.0f;
    verifiedFront = -1.0f;
    frontSeenMs = tof_last_frame_ms(FRONT_DIR);
    r2LastFrame = frontSeenMs;
    lastGoodR2Ms = millis();
    lastAcceptedR2Ms = millis();
    targetStopMs = 0;
    targetStableFrames = 0;
    abruptChangeFrames = 0;
    rawDangerFrames = 0;
    activeFrontDirection = 0;
    candidateDirection = 0;
    candidateFrames = 0;
    rearCloseFrames = 0;
    previousAlignTicks = encoder_ticks;
    lastEncoderChangeMs = millis();
    telemetry_parking_stage("FRONT R2C3 ALIGN", firstTarget);
    return true;
  };

  Serial.println("FINAL PARK: continuous turn-to-front-to-turn handoff enabled");
  Serial.print("FINAL FRONT R2C3: target mm=");
  Serial.print(FRONT_TARGET_MM, 1);
  Serial.println(" tolerance=8 mm speed=35 PWM");

  while ((uint32_t)(millis() - frontStartMs) < FRONT_ALIGN_TIMEOUT_MS) {
    read_gyro_data();
    if (!gyroAvailable || !isfinite(gz)) {
      fail("front alignment gyro unavailable");
      return;
    }

    int rawR2 = -1;
    const bool gotR2 = tof_read_new_zone(FRONT_DIR, r2LastFrame, 2, 3, rawR2);
    bool pauseForSuspicion = false;
    if (gotR2 && rawR2 > 0) {
      latestR2Raw = rawR2;
      lastGoodR2Ms = millis();
      // Once the fresh front data shows a comfortably distant wall, keep
      // driving forward during the initial 3-frame median warm-up.
      // If the distance is uncertain or close, fall back to the stop guard.
      if (frontHistoryCount < 3 && activeFrontDirection == 0 &&
          rawR2 > FRONT_TARGET_MM + 110.0f && rawR2 < 3500 &&
          (uint32_t)(millis() - lastGoodR2Ms) < 135UL)
        activeFrontDirection = 1;
      // Avoid an uncontrolled overshoot. Stop only if the raw sensor has
      // already gone past the LOWER side of the 8mm target band.
      if (activeFrontDirection > 0 && rawR2 < FRONT_TARGET_MM - FRONT_TOLERANCE_MM) {
        move(0);
        steer_parking(0.0);
        activeFrontDirection = 0;
        candidateDirection = 0;
        candidateFrames = 0;
        lastEncoderChangeMs = millis();
      }
      telemetry_front_cells(rawR2, tof_zone_mm(FRONT_DIR, 3, 3)); // second cell is telemetry only
      frontHistory[frontHistoryIndex] = rawR2;
      frontHistoryIndex = (frontHistoryIndex + 1) % 3;
      if (frontHistoryCount < 3) ++frontHistoryCount;
      if (rawR2 < 110) {
        if (rawDangerFrames < 255) ++rawDangerFrames;
        move(0);
        steer(0);
        pauseForSuspicion = true;
        if (rawDangerFrames >= 2) {
          if (frontEmergencyRecoveries >= 2) {
            fail("front R2C3 emergency persisted after two reverse recoveries");
            return;
          }
          ++frontEmergencyRecoveries;
          if (!recoverFrontEmergency()) {
            fail("front R2C3 emergency: reverse blocked or recovery incomplete");
            return;
          }
          // Ranging and filter history were reset by recovery. Read fresh
          // frames again before permitting ANY front movement or second turn.
          continue;
        }
      } else {
        rawDangerFrames = 0;
      }

      if (frontHistoryCount == 3) {
        const int a = frontHistory[0], b = frontHistory[1], c = frontHistory[2];
        const float median = (float)max(min(a,b), min(max(a,b),c));
        // The robot cannot physically move >120mm between consecutive valid
        // frames at 35 PWM. If the median jumps, STOP and confirm the new
        // distance with three frames, not with a 9-frame dominant cluster.
        if (frontTrusted > 0 && fabs(median - frontTrusted) > 120.0f) {
          if (abruptChangeFrames < 255) ++abruptChangeFrames;
          pauseForSuspicion = true;
          move(0);
          steer(0);
          if (abruptChangeFrames < 3) {
            activeFrontDirection = 0;
            candidateFrames = 0;
          } else {
            frontTrusted = median; // real repositioning now confirmed
            frontFiltered = median;
            lastAcceptedR2Ms = millis();
            abruptChangeFrames = 0;
          }
        } else {
          abruptChangeFrames = 0;
          frontTrusted = median;
          frontFiltered = median;
          lastAcceptedR2Ms = millis();
        }
      }

      // Speed/direction decisions are made ONCE per fresh frame, never from
      // cached samples inside the fast Arduino main loop.
      if (frontFiltered > 0 && !pauseForSuspicion) {
        const float distanceError = frontFiltered - FRONT_TARGET_MM;
        if (fabs(distanceError) <= FRONT_TOLERANCE_MM &&
            abs(rawR2 - (int)FRONT_TARGET_MM) <= (int)FRONT_TOLERANCE_MM + 5) {
          // Three fresh readings already built frontFiltered's median.
          // The newest raw cell agrees, so directly transition at speed 35.
          alignedFront = true;
          verifiedFront = frontFiltered;
          Serial.print("FINAL FRONT ROLLING TARGET R2C3=");
          Serial.println(verifiedFront, 1);
          break;
        } else {
          targetStopMs = 0;
          targetStableFrames = 0;
          const int wanted = distanceError > 0 ? 1 : -1;
          if (wanted != activeFrontDirection) {
            // A direction reversal is only allowed after TWO matching fresh
            // measurements. Brake during this check to avoid ping-pong.
            move(0);
            steer_parking(0.0);
            if (wanted != candidateDirection) {
              candidateDirection = wanted;
              candidateFrames = 1;
            } else if (candidateFrames < 255) {
              ++candidateFrames;
            }
            activeFrontDirection = 0;
            if (candidateFrames >= 2) {
              activeFrontDirection = wanted;
              candidateFrames = 0;
              candidateDirection = 0;
              lastEncoderChangeMs = millis();
              previousAlignTicks = encoder_ticks;
            }
          } else {
            candidateFrames = 0;
            candidateDirection = 0;
          }
        }
      }
    }

    const uint32_t sensorAge = (uint32_t)(millis() - lastGoodR2Ms);
    const bool safeRollingWarmup = frontHistoryCount < 3 &&
        !pauseForSuspicion && activeFrontDirection > 0 &&
        latestR2Raw > FRONT_TARGET_MM + 110.0f &&
        sensorAge <= 135UL;
    if ((sensorAge > FRONT_INVALID_PAUSE_MS || pauseForSuspicion ||
         frontHistoryCount < 3) && !safeRollingWarmup) {
      move(0);
      steer_parking(0.0);
      activeFrontDirection = 0;
      candidateFrames = 0;
      if (sensorAge > FRONT_INVALID_FAIL_MS) {
        Serial.print("FINAL R2C3 missing/status=");
        Serial.print(tof_zone_target_status(FRONT_DIR,2,3));
        Serial.print(" lastRaw=");
        Serial.println(latestR2Raw);
        fail("front R2C3 has no valid fresh frames for 3.5s");
        return;
      }
      lastEncoderChangeMs = millis();
      delay(5);
      continue;
    }

    if (activeFrontDirection < 0) {
      // Check rear clearance while reversing. Unknown status is not falsely
      // treated as a wall; two coherent close frames stop the movement.
      int back2 = -1, back3 = -1;
      uint32_t prior = rearFrameMs;
      const bool backValid = tof_read_new_pair(BACK_DIR, rearFrameMs, back2, back3);
      if (rearFrameMs != prior) {
        if (backValid && back2 < 150 && back3 < 150 &&
            abs(back2-back3) < 90) {
          if (++rearCloseFrames >= 2) {
            fail("rear obstacle confirmed during R2C3 reverse alignment");
            return;
          }
        } else {
          rearCloseFrames = 0;
        }
      }
    } else {
      rearCloseFrames = 0;
    }

    if (activeFrontDirection != 0 && !alignedFront) {
      holdHeading(activeFrontDirection * FRONT_ALIGN_PWM, firstTarget, PARK_FRONT_ALIGN_HEADING_KP);
      int32_t ticksNow = encoder_ticks;
      if (ticksNow != previousAlignTicks) {
        previousAlignTicks = ticksNow;
        lastEncoderChangeMs = millis();
      }
      if ((uint32_t)(millis() - lastEncoderChangeMs) > 2200UL) {
        fail("front alignment wheels stalled at PWM 35");
        return;
      }
    } else {
      move(0);
      steer_parking(0.0);
      lastEncoderChangeMs = millis();
    }

    if ((uint32_t)(millis() - frontLogMs) > 350UL) {
      frontLogMs = millis();
      Serial.print("FINAL FRONT R2C3 raw="); Serial.print(latestR2Raw);
      Serial.print(" median="); Serial.print(frontFiltered, 1);
      Serial.print(" goal="); Serial.print(FRONT_TARGET_MM, 1);
      Serial.print(" yawErr="); Serial.print(firstTarget - gz, 2);
      Serial.print(" motor="); Serial.println(activeFrontDirection * FRONT_ALIGN_PWM);
    }
    delay(5);
  }
  if (!alignedFront) {
    move(0);
    steer_parking(0.0);
    Serial.print("FINAL FRONT R2C3 timeout, last median=");
    Serial.println(frontFiltered, 1);
    fail("front R2C3 alignment timed out");
    return;
  }
  // A second parking turn begins as soon as fresh R2C3 confirms the target.
  // No deliberate stationary settle on a successful forward approach.
  // After FRONT VERIFIED the original V13 front-reading policy resumes.
  frontR2OnlyActive = false;
  telemetry_parking_stage("FRONT VERIFIED", firstTarget);
  Serial.print("FINAL FRONT VERIFIED mm=");
  Serial.print(verifiedFront, 1);
  Serial.print(" gyro=");
  Serial.print(gz, 2);
  Serial.print(" target=");
  Serial.println(firstTarget, 2);

#if FINAL_PARK_STOP_AFTER_FRONT_340
  currentState = PARK;
  telemetry_parking_stage("STOP AT FRONT TARGET", firstTarget);
  telemetry_note_event("front target check complete; second turn disabled");
  return;
#endif

  // Move directly from R2C3 target to second corner without an idle wait.
  // Turn controller still checks gyro, encoder progress and clearance.
  // The second parking turn uses +990 -> +900 or -990 -> -900.
  pid_error = 0;
  pid_last_error = 0;
  read_gyro_data();
  const double secondStartHeading = gz;
  const double secondTarget = fixedSecondTarget;
  Serial.print("FINAL PARK second turn entry=");
  Serial.print(secondStartHeading, 2);
  Serial.print(" fixed target=");
  Serial.println(secondTarget, 2);
  if (!turnTo(secondTarget, "SECOND 90")) {
    fail(parkingTurnFailure);
    return;
  }
  current_angle_gyro = secondTarget;
  // Keep the wheels moving into the two-post search after the second corner.
  steer_parking(0.0);
  read_gyro_data();
  Serial.print("FINAL PARK straight heading error=");
  Serial.println(secondTarget - gz, 2);

  // ========================================================================
  // ZONE 2 / 3: track the two thin side posts, then advance offset in mm
  // ========================================================================
  // Column 3, each physical side checked independently. First post is
  // confirmed on its brief close-to-open pulse; second is confirmed at
  // the leading edge ~220mm later, without waiting for that post to leave.
  // Track the four lines of COLUMN THREE (R1C3 .. R4C3) as the robot
  // translates past each thin post. The row pattern is a spatial signature:
  // one isolated short reading is weak evidence, while close samples seen
  // by multiple lines or over multiple frames are much more trustworthy.
  // Crucially the comparisons are per-row to avoid looking at a nearer
  // surface in an unrelated 4x4 matrix column.
  // V13: use the two reliably visible middle rows of COLUMN 3.
  // Outer rows can be unreliable on angled/mirrored sensors. A near return
  // must persist at two DIFFERENT encoder positions; static/invalid data
  // can never increment the physical wall count.
  struct ThinPostColumn3Detector {
    struct Row {
      int baselineMM;
      int lastOpenMM;
      uint8_t stableOpen;
    } row[2];  // indexes 0,1 => physical R2C3,R3C3
    bool inPulse;
    bool hasFirstPost;
    bool secondFound;
    uint8_t phase, postCount, nearMask;
    uint8_t pulseSpatialFrames;
    float pulseStartMM, pulseLastSpatialMM, pulseLastCloseMM;
    int32_t pulseStartTicks;
    int pulseNearestMM, pulseDeepestDropMM;
    int firstPostNearMM;
    bool strictPlusDirection;
    float firstAtMM, secondAtMM;
    int32_t secondStartTicks;
    int displayMM;
    int baselineMM;
    float lastOpenAtMM;

    void begin() {
      for (uint8_t i=0;i<2;++i) {
        row[i].baselineMM = -1;
        row[i].lastOpenMM = -1;
        row[i].stableOpen = 0;
      }
      inPulse = hasFirstPost = secondFound = false;
      phase = postCount = nearMask = pulseSpatialFrames = 0;
      pulseStartMM = pulseLastSpatialMM = pulseLastCloseMM = -1.0f;
      firstAtMM = secondAtMM = -1.0f;
      pulseStartTicks = secondStartTicks = 0;
      pulseNearestMM = 4000;
      pulseDeepestDropMM = 0;
      firstPostNearMM = -1;
      strictPlusDirection = false;
      displayMM = baselineMM = -1;
      lastOpenAtMM = -1.0f;
    }
    void newPulse(float atMM, int32_t ticks) {
      inPulse = true;
      pulseStartMM = pulseLastSpatialMM = pulseLastCloseMM = atMM;
      pulseStartTicks = ticks;
      pulseSpatialFrames = 1;
      pulseNearestMM = 4000;
      pulseDeepestDropMM = 0;
    }
    void clearPulse() {
      inPulse = false;
      pulseSpatialFrames = 0;
      pulseStartMM = pulseLastSpatialMM = pulseLastCloseMM = -1;
      pulseNearestMM = 4000;
      pulseDeepestDropMM = 0;
    }
    // +1: require two distinct post pulses at the actual 220 mm pitch.
    // Keep V13's broad proven window for -1 without changing its behavior.
    bool pairSpacing(float atMM, float expectedMM, float toleranceMM) {
      const float d = atMM - firstAtMM;
      if (strictPlusDirection)
        return d >= expectedMM - toleranceMM && d <= expectedMM + toleranceMM;
      return d >= 165.0f && d <= 365.0f;
    }
    bool feed(const int cells[4], float atMM, int32_t ticks,
              int edgeMM, float expectedMM, float toleranceMM) {
      nearMask = 0;
      displayMM = -1;
      if (secondFound) return true;
      for (uint8_t r=0; r<2; ++r) {
        Row &z = row[r];
        const int d = cells[r+1]; // R2C3, R3C3 ONLY
        if (d < 40 || d > 3500) continue;
        if (displayMM < 0 || d < displayMM) displayMM = d;
        if (z.baselineMM < 0) {
          z.baselineMM = z.lastOpenMM = d;
          z.stableOpen = 1;
          continue;
        }
        const int drop = z.baselineMM - d;
        // At least two stable open readings before treating a drop as a post.
        if (z.stableOpen >= 2 && drop >= edgeMM) {
          nearMask |= (uint8_t)(1U << (r+1));
          if (d < pulseNearestMM) pulseNearestMM = d;
          if (drop > pulseDeepestDropMM) pulseDeepestDropMM = drop;
          continue;
        }
        // Do not fold close echoes into the open-space reference.
        if (drop > 25) continue;
        if (z.lastOpenMM >= 0 && abs(z.lastOpenMM - d) <= 30) {
          if (z.stableOpen < 20) ++z.stableOpen;
          z.baselineMM = (3*z.baselineMM + d)/4;
        } else {
          // Relearn genuinely different stable scenery only outside a pulse.
          if (!inPulse) {
            z.stableOpen = 1;
            z.baselineMM = d;
          }
        }
        z.lastOpenMM = d;
      }
      int baselines = 0, n=0;
      for (uint8_t i=0;i<2;++i) if (row[i].baselineMM > 0) {
        baselines += row[i].baselineMM; ++n;
      }
      baselineMM = n ? baselines/n : -1;

      // A true second post needs fresh open-space readings between pulses.
      // Do not infer a gap from a missing (-1) cell or the previous wall's echo.
      if (strictPlusDirection && hasFirstPost && !inPulse && nearMask == 0) {
        for (uint8_t r=0; r<2; ++r) {
          const int d = cells[r+1];
          if (d >= 40 && d <= 3500 && row[r].stableOpen >= 2 &&
              abs(d - row[r].baselineMM) <= 30) {
            lastOpenAtMM = atMM;
            break;
          }
        }
      }
      if (nearMask != 0) {
        if (!inPulse) {
          // Never count a wall without a verified change of position.
          newPulse(atMM, ticks);
          // newPulse resets the pulse extrema; repopulate from this frame.
          for (uint8_t r=0;r<2;++r) if (nearMask & (1U<<(r+1))) {
            const int d = cells[r+1];
            if (d < pulseNearestMM) pulseNearestMM = d;
            const int drop = row[r].baselineMM - d;
            if (drop > pulseDeepestDropMM) pulseDeepestDropMM = drop;
          }
        } else {
          pulseLastCloseMM = atMM;
          if (atMM - pulseLastSpatialMM >= 2.0f) {
            pulseLastSpatialMM = atMM;
            if (pulseSpatialFrames < 255) ++pulseSpatialFrames;
          }
        }
        // A wide object is NOT one of the 10 mm posts.
        if (atMM - pulseStartMM > 105.0f) {
          clearPulse();
          // New geometry must be established, not the old low return.
          for (uint8_t i=0;i<2;++i) row[i].stableOpen = 0;
          return false;
        }
        // Recognize Wall 2 at its incoming close edge, after at least two
        // fresh samples at different encoder positions. Prevents the single
        // false reflection that caused premature parking in V9/V12.
        const bool closeMatchesFirst = !strictPlusDirection ||
          (firstPostNearMM >= 40 && pulseNearestMM >= 40 &&
           pulseNearestMM <= 195 &&
           abs(pulseNearestMM - firstPostNearMM) <= 60);
        const bool realGapObserved = !strictPlusDirection ||
          (lastOpenAtMM >= firstAtMM + 70.0f &&
           pulseStartMM - lastOpenAtMM <= 95.0f);
        if (hasFirstPost && pairSpacing(pulseStartMM, expectedMM, toleranceMM) &&
            closeMatchesFirst && realGapObserved &&
            pulseSpatialFrames >= 2 &&
            (pulseLastSpatialMM - pulseStartMM >= 5.0f ||
             (pulseLastSpatialMM - pulseStartMM >= 2.0f && pulseDeepestDropMM >= 75)) &&
            pulseDeepestDropMM >= edgeMM) {
          secondFound = true;
          postCount = phase = 2;
          secondAtMM = pulseStartMM;
          secondStartTicks = pulseStartTicks;
          return true;
        }
        return false;
      }
      if (!inPulse) return false;
      // For a real first wall, demand a falling AND rising edge, separated
      // by encoder movement. Never treat the first static measurement as it.
      const float width = pulseLastCloseMM - pulseStartMM;
      const int candidateNearMM = pulseNearestMM;
      const bool validPulse = pulseSpatialFrames >= 2 &&
                              width <= (strictPlusDirection ? 75.0f : 95.0f) &&
                              (width >= 5.0f || (width >= 2.0f && pulseDeepestDropMM >= 75)) &&
                              pulseDeepestDropMM >= edgeMM &&
                              (!strictPlusDirection ||
                               (pulseStartMM >= 55.0f &&
                                candidateNearMM >= 40 && candidateNearMM <= 195));
      const float newFirstAt = pulseStartMM;
      clearPulse();
      if (validPulse) {
        const float gap = newFirstAt - firstAtMM;
        const bool canReanchor = !hasFirstPost ||
          (strictPlusDirection ?
            (gap >= 75.0f && (gap < expectedMM - toleranceMM ||
                                gap > expectedMM + toleranceMM)) :
            (gap > 365.0f));
        if (canReanchor) {
          firstAtMM = newFirstAt;
          firstPostNearMM = candidateNearMM;
          lastOpenAtMM = -1.0f;
          hasFirstPost = true;
          postCount = phase = 1;
          if (strictPlusDirection) {
            Serial.print("FINAL SIDE +1 FIRST post candidate at encoder_mm=");
            Serial.print(newFirstAt, 1);
            Serial.print(" close_mm=");
            Serial.println(candidateNearMM);
          }
        }
      }
      return false;
    }
  };

  // Inspect both sides only to select the near wall; thereafter poll ONE.
  // Check both physical side modules. V6 succeeded while monitoring both;
  // V10/V11 watched only the preferred side, which may be the opposite side
  // of the actual thin markers depending on turn direction and mounting.
  struct SideWatch {
    DistanceDir sensor;
    ThinPostColumn3Detector detector;
    uint32_t lastFrameMs;
    uint32_t lastFreshMs;
    int cells[4];
  };
  SideWatch watches[2];
  watches[0].sensor = LEFT_DIR;
  watches[1].sensor = RIGHT_DIR;
  uint8_t onlineCount = 0;
  for (uint8_t i=0; i<2; ++i) {
    watches[i].detector.begin();
    watches[i].detector.strictPlusDirection = (turn_direction == 1);
    watches[i].lastFrameMs = 0;
    watches[i].lastFreshMs = millis();
    for (uint8_t r=0; r<4; ++r) watches[i].cells[r] = -1;
    if (tof_sensor_online(watches[i].sensor)) {
      ++onlineCount;
      Serial.print("FINAL PARK sensor online: ");
      Serial.println(watches[i].sensor == LEFT_DIR ? "LEFT" : "RIGHT");
    }
  }
  if (!onlineCount) { fail("both side ToF sensors offline"); return; }
  telemetry_side_parking_reset();
  const uint32_t driveStart = millis();
  uint32_t lastProgress = driveStart;
  uint32_t lastSideLog = driveStart;
  uint32_t sideFrontFrameMs = tof_last_frame_ms(FRONT_DIR);
  uint8_t sideFrontCloseFrames = 0;
  bool sideFrontPaused = false;
  uint32_t sideFrontPauseMs = 0;
  float lastCm = 0.0f;
  bool detectedSecond = false;
  int32_t secondWallTicks = 0;
  float wallSearchSteer = 0.0f;
  DistanceDir confirmedSide = preferredSideSensor;
  noInterrupts();
  encoder_ticks = 0;
  interrupts();
  telemetry_parking_stage("SIDE SEARCH LEFT/RIGHT", secondTarget);
  Serial.println("FINAL PARK SIDE V13: R2C3/R3C3 physical pulses, encoder movement + side selection");

  auto sampleSide = [&](SideWatch &s, float traveledMM) -> bool {
    if (!tof_sensor_online(s.sensor)) return false;
    tof_poll_one((uint8_t)s.sensor);
    const uint32_t frameMs = tof_last_frame_ms(s.sensor);
    if (!frameMs || frameMs == s.lastFrameMs ||
        (uint32_t)(millis() - frameMs) > 150UL) return false;
    s.lastFrameMs = frameMs;
    s.lastFreshMs = millis();
    for (uint8_t r = 0; r < 4; ++r)
      s.cells[r] = tof_zone_mm(s.sensor, r+1, 3);
    int32_t ticks;
    noInterrupts(); ticks = encoder_ticks; interrupts();
    const bool found = s.detector.feed(s.cells, traveledMM, ticks,
                                     SIDE_EDGE_CHANGE_MM,
                                     SIDE_POST_SEPARATION_MM,
                                     SIDE_POST_SEPARATION_TOL_MM);
    uint8_t statuses[4];
    for (uint8_t r=0; r<4; ++r)
      statuses[r] = (uint8_t)tof_zone_target_status(s.sensor, r+1, 3);
    telemetry_side_parking_frame((uint8_t)s.sensor,
                                 (uint8_t)(s.detector.secondFound ? 2 : (s.detector.hasFirstPost ? 1 : 0)),
                                 s.detector.phase,
                                 s.detector.nearMask, traveledMM, s.cells,
                                 statuses);
    if (found) {
      Serial.print("FINAL PARK SIDE WALL2 LEFT/RIGHT=");
      Serial.print(s.sensor == LEFT_DIR ? "LEFT" : "RIGHT");
      Serial.print(" spacing_mm=");
      Serial.println(s.detector.secondAtMM - s.detector.firstAtMM, 1);
    }
    return found;
  };

  // Returns 0=clear, 1=pause for confirmation, 2=confirmed close,
  // 3=stale while a potentially close wall remains unverified. A single
  // bad near zone pauses the motors, but never permanently aborts parking.
  // Close must be present in THREE distinct fresh frames before an abort.
  auto sideFrontSafety = [&]() -> uint8_t {
    float mm = -1.0f;
    if (readFront(sideFrontFrameMs, mm)) {
      if (mm < 150.0f) {
        if (sideFrontCloseFrames == 0) sideFrontPauseMs = millis();
        if (sideFrontCloseFrames < 3) ++sideFrontCloseFrames;
        sideFrontPaused = true;
        if (sideFrontCloseFrames >= 3) return 2;
      } else {
        // A fresh clear frame cancels a transient near return.
        if (sideFrontPaused)
          Serial.println("FINAL FRONT side-search transient close cleared");
        sideFrontCloseFrames = 0;
        sideFrontPaused = false;
        return 0;
      }
    }
    if (sideFrontPaused && (uint32_t)(millis() - sideFrontPauseMs) > 900UL)
      return 3;  // Do not resume moving blind toward a suspected obstacle.
    return sideFrontPaused ? 1 : 0;
  };

  // Pick the near sensor WHILE moving. Do not spend up to 900ms parked
  // at the second corner waiting for 2 frames from each side.
  // Feed both detectors during this short initial roll to avoid missing
  // an early thin post while choosing which side to sample at 60Hz.
  int32_t selectionSum[2] = {0,0};
  uint8_t selectionSamples[2] = {0,0};
  const uint32_t selectStart = millis();
  while ((uint32_t)(millis()-selectStart) < 150UL) {
    const float traveledMM = read_cm() * 10.0f;
    for (uint8_t i=0;i<2;++i) {
      if (!tof_sensor_online(watches[i].sensor)) continue;
      const uint32_t beforeFrame = watches[i].lastFrameMs;
      if (sampleSide(watches[i], traveledMM)) {
        detectedSecond = true;
        secondWallTicks = watches[i].detector.secondStartTicks;
        confirmedSide = watches[i].sensor;
        telemetry_side_parking_finished((uint8_t)confirmedSide);
        break;
      }
      if (watches[i].lastFrameMs == beforeFrame) continue;
      const int a = watches[i].cells[1];
      const int b = watches[i].cells[2];
      int v = -1;
      if (a >= 60 && b >= 60 && a <= 1500 && b <= 1500) v = (a+b)/2;
      else if (a >= 60 && a <= 1500) v = a;
      else if (b >= 60 && b <= 1500) v = b;
      if (v > 0 && selectionSamples[i] < 8) {
        selectionSum[i] += v;
        ++selectionSamples[i];
      }
    }
    if (detectedSecond) break;
    const uint8_t guard = sideFrontSafety();
    if (guard >= 2) {
      fail(guard == 2 ? "confirmed front obstacle during side startup" :
                         "front ToF stale during side startup");
      return;
    }
    if (guard == 1) {
      move(0);
      steer(0);
      delay(4);
      continue;
    }
    holdHeading(PARK_DRIVE_PWM, secondTarget, PARK_HEADING_HOLD_KP);
    const float cm = read_cm();
    if (cm > lastCm + 0.25f) { lastCm = cm; lastProgress = millis(); }
    delay(3);
  }
  int selectedIndex = preferredSideSensor == LEFT_DIR ? 0 : 1;
  const int lm = selectionSamples[0] ? selectionSum[0]/selectionSamples[0] : 10000;
  const int rm = selectionSamples[1] ? selectionSum[1]/selectionSamples[1] : 10000;
  if (!tof_sensor_online(watches[selectedIndex].sensor)) selectedIndex = 1-selectedIndex;
  if (lm >= 75 && lm <= 450 && (rm > 450 || lm + 90 < rm)) selectedIndex = 0;
  else if (rm >= 75 && rm <= 450 && (lm > 450 || rm + 90 < lm)) selectedIndex = 1;
  else if (lm == 10000 && rm != 10000) selectedIndex = 1;
  else if (rm == 10000 && lm != 10000) selectedIndex = 0;
  const DistanceDir scanningSide = watches[selectedIndex].sensor;
  telemetry_side_parking_active((uint8_t)scanningSide);
  const bool fastScan = tof_enable_fast_parking_side(scanningSide);
  watches[selectedIndex].lastFrameMs = 0;
  watches[selectedIndex].lastFreshMs = millis(); // First 60 Hz frame has time to arrive.
  Serial.print("FINAL PARK SCAN SIDE "); Serial.print(scanningSide == LEFT_DIR ? "LEFT" : "RIGHT");
  Serial.print(" selection L/R="); Serial.print(lm); Serial.print('/'); Serial.print(rm);
  Serial.print(" frequency="); Serial.println(fastScan ? "60Hz" : "30Hz");
  telemetry_parking_stage("SIDE COLUMN3 MOVEMENT CHECK", secondTarget);

  // A pair from ONE sensor is required; a candidate must survive movement.
  // Once side selection is done, sample ONLY that side for best ToF cadence.
  while (!detectedSecond && (uint32_t)(millis() - driveStart) < 30000UL && read_cm() < 170.0f) {
    const float traveledMM = read_cm() * 10.0f;
    bool found = false;
    if (sampleSide(watches[selectedIndex], traveledMM)) {
      move(0);
      steer_parking(0.0);
      secondWallTicks = watches[selectedIndex].detector.secondStartTicks;
      confirmedSide = scanningSide;
      detectedSecond = true;
      telemetry_side_parking_finished((uint8_t)confirmedSide);
      telemetry_parking_stage("WALL 2 CONFIRMED", secondTarget);
      found = true;
    }
    if (found) break;
    const uint32_t now = millis();
    bool anyFresh = false;
    uint8_t bestCount = 0;
    int bestDisplay = -1;
    SideWatch &selectedWatch = watches[selectedIndex];
    anyFresh = (uint32_t)(now - selectedWatch.lastFreshMs) < 650UL;
    bestCount = selectedWatch.detector.hasFirstPost ? 1 : 0;
    bestDisplay = selectedWatch.detector.displayMM;
    if (!anyFresh) { fail("selected side ToF has stale frames"); return; }
    telemetry_side_cell(bestDisplay);
    if ((uint32_t)(now - lastSideLog) >= 500UL) {
      lastSideLog = now;
      Serial.print("FINAL PARK SIDE ACTIVE ");
      Serial.print(scanningSide == LEFT_DIR ? "LEFT " : "RIGHT ");
      Serial.print("FINAL PARK SIDE counts LEFT=");
      Serial.print((int)(watches[0].detector.secondFound ? 2 : (watches[0].detector.hasFirstPost ? 1 : 0)));
      Serial.print(" RIGHT=");
      Serial.print((int)(watches[1].detector.secondFound ? 2 : (watches[1].detector.hasFirstPost ? 1 : 0)));
      Serial.print(" traveled_cm="); Serial.println(traveledMM / 10.0f,1);
    }
    const uint8_t sideGuard = sideFrontSafety();
    if (sideGuard >= 2) {
      move(0); steer(0);
      fail(sideGuard == 2 ? "confirmed front wall <150mm during side search" :
                             "front ToF stale during side-search safety check");
      return;
    }
    if (sideGuard == 1) {
      move(0); steer(0); delay(5); continue;
    }
    read_gyro_data();
    const double sideYawError = secondTarget - gz;
    const double sideYawCmd = fabs(sideYawError) <= 1.2 ? 0.0 :
      constrain(sideYawError * 0.062, -0.42, 0.42);
    wallSearchSteer = 0.80f * wallSearchSteer + 0.20f * (float)sideYawCmd;
    steer(wallSearchSteer);
    move(bestCount >= 1 ? PARK_AFTER_FIRST_WALL_PWM : PARK_DRIVE_PWM);
    const float cm = read_cm();
    if (cm > lastCm + 0.25f) { lastCm = cm; lastProgress = millis(); }
    if ((uint32_t)(millis() - lastProgress) > 3000UL) {
      fail("encoder stalled during side wall search"); return;
    }
    delay(5);
  }
  move(0);
  if (!detectedSecond) {
    Serial.print("FINAL PARK side pattern incomplete; LEFT posts=");
    Serial.print((int)watches[0].detector.postCount);
    Serial.print(" RIGHT posts=");
    Serial.print((int)watches[1].detector.postCount);
    Serial.print(" cm=");
    Serial.println(read_cm(), 1);
    fail("side wall pulses not confirmed (see WiFi log + sensor statuses)");
    return;
  }

  // Advance the configured offset from the FIRST close-frame encoder tick of post 2.
  // Confirmation travel is included in SIDE_AFTER_WALL2_MM.
  telemetry_parking_stage("WALL 2 OFFSET", secondTarget);
  Serial.println("FINAL PARK: side post2 confirmed; moving to configured offset from first edge");
  const uint32_t offsetStarted = millis();
  uint32_t offsetProgressAt = offsetStarted;
  float lastOffsetMM = 0.0f;
  bool offsetReached = false;
  while ((uint32_t)(millis() - offsetStarted) < 5000UL) {
    int32_t nowTicks;
    noInterrupts();
    nowTicks = encoder_ticks;
    interrupts();
    const float advanceMM = (float)abs(nowTicks - secondWallTicks) * FINAL_MM_PER_TICK;
    if (advanceMM >= SIDE_AFTER_WALL2_MM) {
      offsetReached = true;
      break;
    }
    if (advanceMM > lastOffsetMM + 1.0f) {
      lastOffsetMM = advanceMM;
      offsetProgressAt = millis();
    }
    if ((uint32_t)(millis() - offsetProgressAt) > 1800UL) break;
    read_gyro_data();
    if (!gyroAvailable || !isfinite(gz) || fabs(secondTarget - gz) > 12.0) break;
    // Use the same fresh-frame front safeguard during the extra offset.
    const uint8_t offsetGuard = sideFrontSafety();
    if (offsetGuard >= 2) {
      move(0);
      steer(0);
      fail(offsetGuard == 2 ? "confirmed front wall <150mm during post2 advance" :
                               "front ToF stale during post2 advance");
      return;
    }
    if (offsetGuard == 1) {
      move(0);
      steer(0);
      offsetProgressAt = millis(); // Avoid counting a deliberate pause as encoder stall.
      delay(5);
      continue;
    }
    holdHeading(PARK_AFTER_FIRST_WALL_PWM, secondTarget, PARK_HEADING_HOLD_KP);
    delay(5);
  }
  move(0);
  steer_parking(0.0);
  settle(110);
  if (!offsetReached) {
    fail("wall 2 encoder offset incomplete or unsafe");
    return;
  }
  Serial.println("FINAL PARK: configured post2 offset reached; starting parallel parking arcs");
  // ========================================================================
  // ZONE 3 / 3: exactly three angle-driven parallel-parking movements.
  // current_angle_gyro holds the 2nd 90deg target. The opposite sign
  // selects the mirrored parking slot. No timed finishing drive.
  // ========================================================================
  telemetry_parking_stage("FINAL 3 MOVES", current_angle_gyro);
  pid_error = 0;
  pid_last_error = 0;
  if (turn_direction == 1) {
    move_until_angle_max(-park_speed, (current_angle_gyro) - PARK_Z3_REVERSE_ENTRY_DEG);
    move_until_angle_max(-park_speed, (current_angle_gyro) - 5);
    move_until_angle_max( park_speed, (current_angle_gyro) + 5);
  } else {
    move_until_angle_max(-park_speed, (current_angle_gyro) + PARK_Z3_REVERSE_ENTRY_DEG);
    move_until_angle_max(-park_speed, (current_angle_gyro) + 5);
    move_until_angle_max( park_speed, (current_angle_gyro) - 5);
  }
  move(0);
  steer_parking(0.0);
  currentState = PARK;
  telemetry_parking_stage("COMPLETE", current_angle_gyro);
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
    // The detected floor marker/corner overrides post-cube centering.
    if (finalLaneCenterActive) final_lane_center_reset();
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

  // Center the robot while STILL processing the camera. During the first
  // 150ms after a bypass an old cube may still be at the image edge, so wait
  // briefly, then require repeated fresh commands for a NEW cube.
  // A real next cube gets priority over centering; floor markers above
  // already have priority without any debounce.
  if (finalLaneCenterActive && finalLaneIgnoreCubeCommands && currentState == PID) {
    const uint32_t now = millis();
    if ((uint32_t)(now - finalLaneArmMs) < 150UL) return;
    char kind = 0;
    float followAngle = 0.0f;
    if (cmd.startsWith("S")) {
      kind = 'S';
      followAngle = cmd.substring(1).toFloat();
      if (!isfinite(followAngle)) return;
    } else if (cmd.indexOf("RED") != -1 || cmd.charAt(0) == 'R') {
      kind = 'R';
    } else if (cmd.indexOf("GREEN") != -1 || cmd.charAt(0) == 'G') {
      kind = 'G';
    } else {
      return;
    }
    if (kind != finalLaneCameraCandidateType ||
        (uint32_t)(now - finalLaneCameraCandidateMs) > 320UL) {
      finalLaneCameraCandidateType = kind;
      finalLaneCameraCandidateCount = 0;
    }
    finalLaneCameraCandidateMs = now;
    if (finalLaneCameraCandidateCount < 4) ++finalLaneCameraCandidateCount;
    if (kind == 'S') finalLaneCameraFollowAngle = followAngle;
    // Same-color near readings could be the cube we just passed. Require
    // one extra confirmation; other close cubes still take priority quickly.
    const uint8_t needed = (kind != 'S' && kind == finalCubeColor) ? 3 : 2;
    if (finalLaneCameraCandidateCount < needed) return;

    const float confirmedFollowAngle = finalLaneCameraFollowAngle;
    const float carryOffset = finalLaneEstimatedLateralMM;
    final_lane_center_reset();
    final_cube_odom_begin();
    finalCubeLateralMM = carryOffset;
    finalRapidCornerActive = false;
    finalLastCubeCameraMs = now;
    if (kind == 'S') {
      currentState = FOLLOW_CUBE;
      follow_cube_angle = confirmedFollowAngle;
      desiredSteering = follow_cube_angle;
      last_follow_cube = now;
      last_cube_time = now;
      pid_error = pid_last_error = 0;
      telemetry_note_event("new cube FOLLOW confirmed during middle-lane recovery");
    } else {
      final_begin_avoid(kind);
      telemetry_note_event("new cube CLOSE confirmed during middle-lane recovery");
    }
    return;
  }

  if (cmd.startsWith("S")) {
    final_lane_center_reset();
    finalRapidCornerActive = false;
    finalLastCubeCameraMs = millis();
    if (currentState == AVOID_CUBE) {
      if (finalCubePhase == FC_DRIVE_OUT && read_cm() >= 9.0f && (uint32_t)(millis() - finalCubePhaseMs) >= 130UL) {
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
      if (finalCubePhase == FC_RECOVER_TURN || finalCubePhase == FC_RECOVER_FORWARD) {
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
    if (currentState != FOLLOW_CUBE) final_cube_odom_begin();
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
    final_lane_center_reset();
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