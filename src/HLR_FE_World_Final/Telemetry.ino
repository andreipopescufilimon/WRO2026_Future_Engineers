// ============================================================================
// HyperLine Robotics | WRO 2026 Future Engineers | World Final
// Read-only diagnostics
// Local HTTP dashboard, live sensor/control values, bounded yaw history and parking/color CSV exports. No remote actuation.
// Source revision: 24 November 2026, rolling parking release.
// ============================================================================

// Set 0 for competition (Wi-Fi radio OFF); set 1 to enable the live AP.
// Network when enabled: HyperLine / HyperLine-FE, http://192.168.4.1
#define HLR_ENABLE_WIFI_TELEMETRY 1

// Read-only Wi-Fi telemetry for Open and Final. No commands can change the robot.
static WebServer telemetryServer(80);
static bool telemetryReady = false;
static uint32_t telemetryLastTickMs = 0;
static uint32_t telemetryLastRecordMs = 0;
static uint32_t telemetryLastClientMs = 0;
static uint32_t telemetryLastTofPollMs = 0;
static uint8_t telemetryNextTofIndex = 0;

int telemetryMotorPwm = 0;
static const char *telemetryParkingStage = "Not started";
static const char *telemetryFault = "";
static const char *telemetryEvent = "Boot";
static const char *telemetryLastCamera = "None";
static uint32_t telemetryLastCameraMs = 0;
static float telemetryCameraSteer = 0.0f;
static bool telemetryParkingStarted = false;
static double telemetryParkingTarget = 0.0;
static bool telemetryFrontTestStopped = false;
static uint32_t telemetryLastFrontMs = 0;
static int telemetryFrontLeft = -1, telemetryFrontRight = -1;
extern float imuAccelX, imuAccelY, imuAccelZ;
extern float imuPitchDeg, imuRollDeg, imuAccelMagnitude;
void imu_read_acceleration();
static int telemetryFrontR2C3 = -1, telemetryFrontR3C3 = -1;
static int telemetrySideR2C3 = -1;

// Ring buffer retains raw TCS34725 values for phone-based calibration after a run.
struct TelemetryColorSample {
  uint32_t ms;
  uint16_t clear, red, green, blue;
  uint8_t detected;
  uint8_t markerArmed;
};
static const uint16_t TELEMETRY_COLOR_LOG_CAP = 400;
static TelemetryColorSample telemetryColorLog[TELEMETRY_COLOR_LOG_CAP];
static uint16_t telemetryColorHead = 0, telemetryColorCount = 0;
static uint32_t telemetryColorLastLogMs = 0;
static uint32_t telemetryColorLastRawStamp = 0;

static void telemetry_color_record() {
  if (!tcsReady || tcsLastReadMicros == 0 ||
      (uint32_t)(millis() - telemetryColorLastLogMs) < 85UL ||
      tcsLastReadMicros == telemetryColorLastRawStamp) return;
  telemetryColorLastRawStamp = tcsLastReadMicros;
  telemetryColorLastLogMs = millis();
  TelemetryColorSample &sample = telemetryColorLog[telemetryColorHead];
  sample.ms = millis(); sample.clear = tcsC;
  sample.red = tcsR; sample.green = tcsG; sample.blue = tcsB;
  sample.detected = (uint8_t)tcsCurrentColor;
  sample.markerArmed = tcsMarkerArmed ? 1 : 0;
  telemetryColorHead = (telemetryColorHead + 1) % TELEMETRY_COLOR_LOG_CAP;
  if (telemetryColorCount < TELEMETRY_COLOR_LOG_CAP) ++telemetryColorCount;
}

static void telemetry_send_color_log() {
  if (telemetryMotorPwm != 0) {
    telemetryServer.send(409, "text/plain", "Stop the robot before downloading the color log.");
    return;
  }
  telemetryServer.sendHeader("Content-Disposition", "attachment; filename=color-sensor-log.csv");
  telemetryServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  telemetryServer.send(200, "text/csv", "");
  telemetryServer.sendContent("ms,clear,red,green,blue,red_permille,green_permille,blue_permille,color_id,marker_armed\n");
  const uint16_t start = (telemetryColorHead + TELEMETRY_COLOR_LOG_CAP - telemetryColorCount) % TELEMETRY_COLOR_LOG_CAP;
  for (uint16_t i = 0; i < telemetryColorCount; ++i) {
    const TelemetryColorSample &v = telemetryColorLog[(start + i) % TELEMETRY_COLOR_LOG_CAP];
    const uint32_t sum = (uint32_t)v.red + v.green + v.blue;
    String line = String(v.ms) + "," + String(v.clear) + "," + String(v.red) +
       "," + String(v.green) + "," + String(v.blue);
    line += "," + String(sum ? (uint32_t)v.red * 1000UL / sum : 0);
    line += "," + String(sum ? (uint32_t)v.green * 1000UL / sum : 0);
    line += "," + String(sum ? (uint32_t)v.blue * 1000UL / sum : 0);
    line += "," + String(v.detected) + "," + String(v.markerArmed) + "\n";
    telemetryServer.sendContent(line);
  }
  telemetryServer.sendContent("");
}

struct TelemetryHistorySample {
  uint32_t timeMs;
  float yaw, target, rate;
  int16_t pwm;
  uint8_t stage;
};
static const uint16_t TELEMETRY_HISTORY_CAP = 480;  // 48 seconds at 10 Hz
static TelemetryHistorySample telemetryHistory[TELEMETRY_HISTORY_CAP];
static uint16_t telemetryHistoryHead = 0;
static uint16_t telemetryHistoryCount = 0;

void telemetry_note_camera(const String &cmd) {
  if (cmd.startsWith("S")) {
    telemetryLastCamera = "FOLLOW S";
    telemetryCameraSteer = cmd.substring(1).toFloat();
  }
  else if (cmd == "R" || cmd == "RED") telemetryLastCamera = "RED close";
  else if (cmd == "G" || cmd == "GREEN") telemetryLastCamera = "GREEN close";
  else telemetryLastCamera = "Other";
  telemetryLastCameraMs = millis();
}
void telemetry_note_event(const char *msg) { telemetryEvent = msg; }
void telemetry_parking_stage(const char *name, double target) {
  telemetryParkingStarted = true;
  telemetryParkingStage = name;
  telemetryParkingTarget = target;
  telemetryFrontTestStopped = String(name) == "STOP AT FRONT 270";
  telemetryEvent = name;
}
void telemetry_parking_fail(const char *reason) {
  telemetryParkingStarted = true;
  telemetryParkingStage = "STOPPED / FAULT";
  telemetryFault = reason;
  telemetryEvent = reason;
}
void telemetry_front_cells(int a, int b) {
  telemetryFrontR2C3 = a;
  telemetryFrontR3C3 = b;
}
void telemetry_side_cell(int mm) { telemetrySideR2C3 = mm; }

// Offline parking diagnostics: updated from each new ToF frame while running.
// A phone can connect to the robot after it stops; no laptop is needed.
static uint8_t telemetryParkWalls[2] = {0, 0};
static uint8_t telemetryParkPhases[2] = {0, 0};
static uint8_t telemetryParkActive = 0; // 0=undecided, 1=LEFT, 2=RIGHT
static uint8_t telemetryParkSelected = 0; // 0=none, 1=left, 2=right
static int telemetryParkRows[2][4] = {{-1,-1,-1,-1},{-1,-1,-1,-1}};
static float telemetryParkTravelMM = 0.0f;
static const uint16_t PARK_SIDE_LOG_CAP = 512;
struct ParkingSideLogSample {
  uint32_t ms;
  int16_t travelMM;
  int16_t row[4];
  uint8_t side;
  uint8_t wallCount;
  uint8_t status[4];
  uint8_t phase;
  uint8_t nearMask;
};
static ParkingSideLogSample parkingSideLog[PARK_SIDE_LOG_CAP];
static uint16_t parkingLogHead = 0, parkingLogCount = 0;
static uint32_t parkingLastLoggedMs[2] = {0, 0};
static uint8_t parkingLastLoggedCount[2] = {0, 0};

void telemetry_side_parking_reset() {
  for (uint8_t i=0;i<2;i++) {
    telemetryParkWalls[i] = telemetryParkPhases[i] = 0;
    parkingLastLoggedMs[i] = 0;
    parkingLastLoggedCount[i] = 0;
    for (uint8_t r=0;r<4;r++) telemetryParkRows[i][r] = -1;
  }
  telemetryParkSelected = 0;
  telemetryParkActive = 0;
  telemetryParkTravelMM = 0;
  parkingLogHead = parkingLogCount = 0;
}

void telemetry_side_parking_active(uint8_t sensorIndex) {
  telemetryParkActive = sensorIndex == LEFT_DIR ? 1 : (sensorIndex == RIGHT_DIR ? 2 : 0);
}
void telemetry_side_parking_frame(uint8_t sensorIndex, uint8_t wallCount,
                                  uint8_t phase, uint8_t nearMask,
                                  float traveledMM, const int cells[4],
                                  const uint8_t statuses[4]) {
  if (sensorIndex != (uint8_t)LEFT_DIR && sensorIndex != (uint8_t)RIGHT_DIR) return;
  const uint8_t i = sensorIndex == (uint8_t)LEFT_DIR ? 0 : 1;
  telemetryParkWalls[i] = wallCount > 2 ? 2 : wallCount;
  telemetryParkPhases[i] = phase;
  telemetryParkTravelMM = traveledMM;
  for (uint8_t r=0;r<4;r++) telemetryParkRows[i][r] = cells[r];
  const uint32_t now = millis();
  const bool change = wallCount != parkingLastLoggedCount[i];
  if (!change && (uint32_t)(now - parkingLastLoggedMs[i]) < 25UL) return;
  parkingLastLoggedMs[i] = now;
  parkingLastLoggedCount[i] = wallCount;
  ParkingSideLogSample &v = parkingSideLog[parkingLogHead];
  v.ms = now;
  v.travelMM = (int16_t)traveledMM;
  for (uint8_t r=0;r<4;r++) {
    v.row[r] = (int16_t)cells[r];
    v.status[r] = statuses[r];
  }
  v.side = sensorIndex;
  v.wallCount = telemetryParkWalls[i];
  v.phase = phase;
  v.nearMask = nearMask;
  parkingLogHead = (parkingLogHead + 1) % PARK_SIDE_LOG_CAP;
  if (parkingLogCount < PARK_SIDE_LOG_CAP) ++parkingLogCount;
}
void telemetry_side_parking_finished(uint8_t sensorIndex) {
  telemetryParkSelected = sensorIndex == LEFT_DIR ? 1 : (sensorIndex == RIGHT_DIR ? 2 : 0);
}

static void telemetry_send_parking_log() {
  if (telemetryMotorPwm != 0) {
    telemetryServer.send(409, "text/plain", "Stop the robot before downloading its parking log.");
    return;
  }
  telemetryServer.sendHeader("Content-Disposition", "attachment; filename=parking-side-log.csv");
  telemetryServer.setContentLength(CONTENT_LENGTH_UNKNOWN);
  telemetryServer.send(200, "text/csv", "");
  telemetryServer.sendContent("ms,travel_mm,side,r1c3_mm,r2c3_mm,r3c3_mm,r4c3_mm,near_mask,phase,wall_count,s1c3,s2c3,s3c3,s4c3\n");
  const uint16_t start = (parkingLogHead + PARK_SIDE_LOG_CAP - parkingLogCount) % PARK_SIDE_LOG_CAP;
  for (uint16_t n=0;n<parkingLogCount;n++) {
    const ParkingSideLogSample &v = parkingSideLog[(start + n) % PARK_SIDE_LOG_CAP];
    String line;
    line.reserve(90);
    line += String(v.ms); line += ',';
    line += String(v.travelMM); line += ',';
    line += v.side == LEFT_DIR ? "LEFT" : "RIGHT";
    for (uint8_t r=0;r<4;r++) { line += ','; line += String(v.row[r]); }
    line += ','; line += String(v.nearMask);
    line += ','; line += String(v.phase);
    line += ','; line += String(v.wallCount);
    for (uint8_t r=0; r<4; ++r) {line += ','; line += String(v.status[r]);}
    line += '\n';
    telemetryServer.sendContent(line);
  }
  telemetryServer.sendContent("");
}


static const char *telemetry_state_name() {
  switch (currentState) {
    case PID: return "PID";
    case FOLLOW_CUBE: return "FOLLOW_CUBE";
    case AVOID_CUBE: return "AVOID_CUBE";
    case AFTER_CUBE: return "AFTER_CUBE";
    case PARK: return "PARK";
    default: return "UNKNOWN";
  }
}
static const char *telemetry_cube_name() {
  switch (finalCubePhase) {
    case FC_TURN_OUT: return "TURN_OUT";
    case FC_DRIVE_OUT: return "DRIVE_OUT";
    case FC_RECOVER_TURN: return "RECOVER_TURN";
    case FC_RECOVER_FORWARD: return "RECOVER_FORWARD";
    default: return "IDLE";
  }
}
static double telemetry_target_yaw() {
  if (telemetryParkingStarted && currentState == PARK) return telemetryParkingTarget;
  if ((currentState == AVOID_CUBE || currentState == AFTER_CUBE) &&
      finalCubePhase != FC_IDLE) return finalCubeTarget;
  return current_angle_gyro;
}
static uint8_t telemetry_stage_id() {
  if (telemetryParkingStarted) return 2;
  if (currentState == AVOID_CUBE || currentState == AFTER_CUBE) return 1;
  return 0;
}
static void telemetry_record_sample() {
  if (!gyroAvailable) return;
  const uint32_t now = millis();
  if ((uint32_t)(now - telemetryLastRecordMs) < 100UL) return;
  telemetryLastRecordMs = now;
  TelemetryHistorySample &s = telemetryHistory[telemetryHistoryHead];
  s.timeMs = now;
  s.yaw = (float)gz;
  s.target = (float)telemetry_target_yaw();
  s.rate = (float)gyroRateDps;
  s.pwm = (int16_t)telemetryMotorPwm;
  s.stage = telemetry_stage_id();
  telemetryHistoryHead = (telemetryHistoryHead + 1) % TELEMETRY_HISTORY_CAP;
  if (telemetryHistoryCount < TELEMETRY_HISTORY_CAP) ++telemetryHistoryCount;
}

// Each module reports two central vertical zones, online state, and age.
// The age allows a real distance to be distinguished from an old cache.
static void telemetry_append_tof(String &j, const char *key, DistanceDir dir) {
  const bool online = tof_sensor_online(dir);
  const uint32_t stamp = tof_last_frame_ms(dir);
  const uint32_t age = stamp ? (uint32_t)(millis() - stamp) : 0;
  j += ",\"tof_"; j += key; j += "\":{\"online\":";
  j += (online ? "true" : "false");
  j += ",\"age\":"; j += (stamp ? String(age) : String(-1));
  j += ",\"r2c3\":"; j += String(tof_telemetry_zone_mm(dir, 2, 3));
  j += ",\"r3c3\":"; j += String(tof_telemetry_zone_mm(dir, 3, 3));
  // Complete 4x4 matrix in row-major order, without changing any motor or
  // parking calculations. These snapshots may be stale: check age above.
  j += ",\"matrix\":[";
  for (uint8_t row=1; row<=4; ++row) {
    for (uint8_t col=1; col<=4; ++col) {
      if (row != 1 || col != 1) j += ',';
      j += String(tof_telemetry_zone_mm(dir, row, col));
    }
  }
  j += "]";
  j += ",\"status\":[";
  for (uint8_t row=1; row<=4; ++row) {
    for (uint8_t col=1; col<=4; ++col) {
      if (row != 1 || col != 1) j += ',';
      j += String(tof_zone_target_status(dir, row, col));
    }
  }
  j += "]";
  j += "}";
}

static String telemetry_json() {
  const double target = telemetry_target_yaw();
  String j;
  j.reserve(2300);
  j += "{\"ms\":"; j += String(millis());
  j += ",\"gyro\":"; j += String(gz, 3);
  j += ",\"target\":"; j += String(target, 3);
  j += ",\"error\":"; j += String(target - gz, 3);
  j += ",\"rate\":"; j += String(gyroRateDps, 3);
  j += ",\"bias\":"; j += String(drifts_z, 5);
  j += ",\"accel_x\":"; j += String(imuAccelX, 3);
  j += ",\"accel_y\":"; j += String(imuAccelY, 3);
  j += ",\"accel_z\":"; j += String(imuAccelZ, 3);
  j += ",\"accel_norm\":"; j += String(imuAccelMagnitude, 3);
  j += ",\"roll\":"; j += String(imuRollDeg, 2);
  j += ",\"pitch\":"; j += String(imuPitchDeg, 2);
  j += ",\"pwm\":"; j += String(telemetryMotorPwm);
  j += ",\"ticks\":"; j += String((long)encoder_ticks);
  j += ",\"turns\":"; j += String(turn_count);
  j += ",\"gyro_ok\":"; j += (gyroAvailable ? "true" : "false");
  j += ",\"state\":\""; j += telemetry_state_name(); j += "\"";
  j += ",\"cube_phase\":\""; j += telemetry_cube_name(); j += "\"";
  j += ",\"park_stage\":\""; j += telemetryParkingStage; j += "\"";
  j += ",\"last_camera\":\""; j += telemetryLastCamera; j += "\"";
  j += ",\"camera_age\":";
  j += String(telemetryLastCameraMs ? (uint32_t)(millis() - telemetryLastCameraMs) : 0UL);
  j += ",\"front_left\":"; j += String(telemetryFrontLeft);
  j += ",\"front_right\":"; j += String(telemetryFrontRight);
  j += ",\"front_a\":"; j += String(telemetryFrontR2C3);
  j += ",\"front_b\":"; j += String(telemetryFrontR3C3);
  j += ",\"side\":"; j += String(telemetrySideR2C3);
  j += ",\"parking_walls_left\":"; j += String((int)telemetryParkWalls[0]);
  j += ",\"parking_walls_right\":"; j += String((int)telemetryParkWalls[1]);
  j += ",\"parking_phase_left\":"; j += String((int)telemetryParkPhases[0]);
  j += ",\"parking_phase_right\":"; j += String((int)telemetryParkPhases[1]);
  j += ",\"parking_selected_side\":"; j += String((int)telemetryParkSelected);
  j += ",\"parking_active_side\":"; j += String((int)telemetryParkActive);
  j += ",\"parking_scan_mm\":"; j += String(telemetryParkTravelMM, 1);
  for (uint8_t side=0; side<2; ++side) {
    j += side ? ",\"parking_rows_right\":[" : ",\"parking_rows_left\":[";
    for (uint8_t r=0;r<4;r++) {
      if (r) j += ',';
      j += String(telemetryParkRows[side][r]);
    }
    j += ']';
  }
  telemetry_append_tof(j, "front", FRONT_DIR);
  telemetry_append_tof(j, "left", LEFT_DIR);
  telemetry_append_tof(j, "right", RIGHT_DIR);
  telemetry_append_tof(j, "rear", BACK_DIR);
  j += ",\"event\":\""; j += telemetryEvent; j += "\"";
  j += ",\"fault\":\""; j += telemetryFault; j += "\"";
  j += ",\"camera_steer\":"; j += String(telemetryCameraSteer, 3);
  j += ",\"follow_seen\":"; j += String((int)finalPassFollowCount);
  // TCS34725 raw samples & the same normalized ratios used by the classifier.
  const uint32_t tcsSum = (uint32_t)tcsR + tcsG + tcsB;
  j += ",\"tcs_ready\":"; j += (tcsReady ? "true" : "false");
  j += ",\"tcs_age_ms\":"; j += (tcsLastReadMicros ? String((uint32_t)(micros() - tcsLastReadMicros) / 1000UL) : String(-1));
  j += ",\"tcs_clear\":"; j += String(tcsC);
  j += ",\"tcs_red\":"; j += String(tcsR);
  j += ",\"tcs_green\":"; j += String(tcsG);
  j += ",\"tcs_blue\":"; j += String(tcsB);
  j += ",\"tcs_r_permille\":"; j += String(tcsSum ? (uint32_t)tcsR * 1000UL / tcsSum : 0UL);
  j += ",\"tcs_g_permille\":"; j += String(tcsSum ? (uint32_t)tcsG * 1000UL / tcsSum : 0UL);
  j += ",\"tcs_b_permille\":"; j += String(tcsSum ? (uint32_t)tcsB * 1000UL / tcsSum : 0UL);
  j += ",\"tcs_detected\":\""; j += tcs_color_name(tcsCurrentColor); j += "\"";
  j += ",\"tcs_latched\":\""; j += tcs_color_name(tcsLatchedColor); j += "\"";
  j += ",\"tcs_armed\":"; j += (tcsMarkerArmed ? "true" : "false");
  j += ",\"lane_centering\":"; j += (finalLaneCenterActive ? "true" : "false");
  j += ",\"lane_camera_suppressed\":"; j += (finalLaneIgnoreCubeCommands ? "true" : "false");
  j += ",\"lane_left_mm\":"; j += String(finalLaneLeftFilteredMM, 1);
  j += ",\"lane_right_mm\":"; j += String(finalLaneRightFilteredMM, 1);
  j += ",\"lane_yaw_bias\":"; j += String(finalLaneYawBiasDeg, 2);
  j += ",\"lane_odometry\":"; j += (finalLaneCenterActive && finalLaneIgnoreCubeCommands ? "true" : "false");
  j += ",\"lane_estimated_offset_mm\":"; j += String(finalLaneEstimatedLateralMM, 1);
  j += ",\"lane_odometry_travel_mm\":"; j += String(finalLaneTravelMM, 1);
  j += "}";
  return j;
}
static void telemetry_send_history() {
  // A large download in the middle of a gyro turn can delay angle integration.
  // The browser keeps building live history while the wheels are moving.
  if (telemetryMotorPwm != 0) {
    telemetryServer.send(200, "application/json", "[]");
    return;
  }
  String out;
  out.reserve(50000);
  out += "[";
  const uint16_t start = (telemetryHistoryHead + TELEMETRY_HISTORY_CAP - telemetryHistoryCount)
                           % TELEMETRY_HISTORY_CAP;
  for (uint16_t i = 0; i < telemetryHistoryCount; ++i) {
    const TelemetryHistorySample &s = telemetryHistory[(start + i) % TELEMETRY_HISTORY_CAP];
    if (i) out += ",";
    out += "["; out += String(s.timeMs);
    out += ","; out += String(s.yaw, 2);
    out += ","; out += String(s.target, 2);
    out += ","; out += String(s.rate, 2);
    out += ","; out += String((int)s.pwm);
    out += ","; out += String((int)s.stage);
    out += "]";
  }
  out += "]";
  telemetryServer.send(200, "application/json", out);
}

static const char TELEMETRY_HTML[] PROGMEM = R"HLRHTML(
<!doctype html><html lang="en"><head><meta name="viewport" content="width=device-width,initial-scale=1">
<meta charset="utf-8"><title>HyperLine | Live telemetry</title>
<style>
:root{color-scheme:dark;font-family:system-ui,-apple-system,Segoe UI,Arial,sans-serif;background:#091522;color:#eff6ff}
*{box-sizing:border-box}body{max-width:1050px;margin:auto;padding:18px}h1{font-size:22px;margin:3px 0 5px}
.sub{font-size:13px;color:#93aac2;margin:0 0 20px}.grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:10px}
.card{background:#152638;border:1px solid #294357;border-radius:12px;padding:13px;min-width:0}
.label{color:#9fb5cb;font-size:12px}.value{font-weight:750;font-size:25px;margin-top:4px;overflow-wrap:anywhere}
.wide{grid-column:1/-1}.good{color:#77eac0}.warn{color:#ffb274}.bad{color:#ff8690}
canvas{width:100%;height:245px;display:block}.key{font-size:12px;color:#acbfd4}
.key span{margin-right:16px} .key .a{color:#6bdfff}.key .b{color:#ffbc73}
.status{display:flex;justify-content:space-between;gap:10px;align-items:center;margin-bottom:15px}
.small{font-size:13px;color:#c3d5e4;overflow-wrap:anywhere}.footer{margin-top:12px;color:#879cb0;font-size:12px}
.matrix-grid{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:5px;margin-top:9px}
.matrix-cell{border:1px solid #34506a;border-radius:6px;min-height:43px;padding:5px 2px;text-align:center;background:#20364b;font-size:13px;font-weight:650}
.matrix-cell span{display:block;font-size:10px;font-weight:400;color:#b9cce0}
.matrix-cell.invalid{background:#392d38;color:#ffadad}
.matrix-cell.merged{background:#544229;color:#ffe7b0}
.matrix-cell.selected{outline:2px solid #76d8fb;outline-offset:-2px}
.matrix-grid.stale .matrix-cell{opacity:.50}
.matrix-meta{margin:5px 0;color:#adc3d7;font-size:12px}
@media(max-width:650px){.grid{grid-template-columns:repeat(2,minmax(0,1fr))}.value{font-size:21px}body{padding:12px}h1{font-size:19px}}
</style></head><body>
<div class="status"><div><h1>HyperLine Robotics</h1><p class="sub">WRO 2026 | Open / Final | IMU + 4 ToF</p></div><div id="connected" class="small">Connecting...</div></div>
<div class="grid">
  <div class="card"><div class="label">Gyro yaw</div><div id="yaw" class="value">--</div></div>
  <div class="card"><div class="label">Target heading</div><div id="target" class="value">--</div></div>
  <div class="card"><div class="label">Heading error</div><div id="error" class="value">--</div></div>
  <div class="card"><div class="label">Angular rate</div><div id="rate" class="value">--</div></div>
  <div class="card"><div class="label">Gyro bias</div><div id="bias" class="value">--</div></div>
  <div class="card"><div class="label">Accel X (m/s²)</div><div id="accel_x" class="value">--</div></div>
  <div class="card"><div class="label">Accel Y (m/s²)</div><div id="accel_y" class="value">--</div></div>
  <div class="card"><div class="label">Accel Z (m/s²)</div><div id="accel_z" class="value">--</div></div>
  <div class="card"><div class="label">Acceleration magnitude</div><div id="accel_norm" class="value">--</div></div>
  <div class="card"><div class="label">IMU roll (tilt)</div><div id="roll" class="value">--</div></div>
  <div class="card"><div class="label">IMU pitch (tilt)</div><div id="pitch" class="value">--</div></div>
  <div class="card"><div class="label">Motor PWM</div><div id="pwm" class="value">--</div></div>
  <div class="card wide"><div class="label">Yaw history, last 48 seconds (also saved on the robot)</div>
    <canvas id="chart"></canvas><div class="key"><span class="a">● Actual gyro</span><span class="b">● Target heading</span></div></div>
  <div class="card"><div class="label">Robot state</div><div id="state" class="value">--</div></div>
  <div class="card"><div class="label">Cube phase</div><div id="cube_phase" class="value">--</div></div>
  <div class="card"><div class="label">Parking stage</div><div id="park_stage" class="value">--</div></div>
  <div class="card"><div class="label">Side walls detected</div><div id="park_wall_count" class="value">0 / 2</div><div id="park_wall_status" class="small">LEFT 0 | RIGHT 0</div></div>
  <div class="card wide"><div class="label">Column 3 during parking (mm). -- is an invalid target, not a negative distance. CSV includes ST target status codes.</div><div id="park_columns" class="small">LEFT: -- | RIGHT: --</div><div class="small"><a href="/parking-log.csv" style="color:#77eac0">Download recorded side ToF data after the run (CSV)</a></div></div>
  <div class="card wide"><div class="label">TCS34725 color sensor | A6/A7 I²C</div>
    <div class="grid" style="margin-top:9px">
      <div><div class="label">Detected / latched</div><div id="tcs_detected" class="value">--</div><div id="tcs_latched" class="small">--</div></div>
      <div><div class="label">Clear / R / G / B (raw)</div><div id="tcs_raw" class="small">--</div></div>
      <div><div class="label">Normalized R / G / B</div><div id="tcs_ratios" class="small">--</div></div>
    </div>
    <div id="tcs_status" class="small" style="margin-top:9px">--</div>
    <div class="small">Classification uses normalized RGB ×1000. Check ratios on WHITE / BLUE / RED floor markers, not just color names. <a href="/color-log.csv" style="color:#77eac0">Download recorded color readings (CSV)</a></div>
  </div>
  <div class="card"><div class="label">Post-cube middle lane</div><div id="lane_status" class="value">--</div><div id="lane_dists" class="small">--</div></div>
  <div class="card"><div class="label">FRONT ToF | 0x30</div><div id="tof_front" class="value">--</div><div id="tof_front_status" class="small">--</div><div class="matrix-meta">4x4 mm | blue: R2C3/R3C3 | s5: valid, s9: merged pulse</div><div id="tof_front_matrix" class="matrix-grid"></div></div>
  <div class="card"><div class="label">LEFT ToF | 0x31</div><div id="tof_left" class="value">--</div><div id="tof_left_status" class="small">--</div><div class="matrix-meta">4x4 mm | blue: R2C3/R3C3 | s5: valid, s9: merged pulse</div><div id="tof_left_matrix" class="matrix-grid"></div></div>
  <div class="card"><div class="label">RIGHT ToF | 0x32</div><div id="tof_right" class="value">--</div><div id="tof_right_status" class="small">--</div><div class="matrix-meta">4x4 mm | blue: R2C3/R3C3 | s5: valid, s9: merged pulse</div><div id="tof_right_matrix" class="matrix-grid"></div></div>
  <div class="card"><div class="label">REAR ToF | 0x33</div><div id="tof_rear" class="value">--</div><div id="tof_rear_status" class="small">--</div><div class="matrix-meta">4x4 mm | blue: R2C3/R3C3 | s5: valid, s9: merged pulse</div><div id="tof_rear_matrix" class="matrix-grid"></div></div>
  <div class="card"><div class="label">Front R2 C3</div><div id="front_a" class="value">--</div></div>
  <div class="card"><div class="label">Front R3 C3</div><div id="front_b" class="value">--</div></div>
  <div class="card"><div class="label">Front LEFT zone (C2)</div><div id="front_left" class="value">--</div></div>
  <div class="card"><div class="label">Front RIGHT zone (C4)</div><div id="front_right" class="value">--</div></div>
  <div class="card"><div class="label">Side center median (parking)</div><div id="side" class="value">--</div></div>
  <div class="card"><div class="label">Last camera command</div><div id="last_camera" class="value">--</div><div id="camera_steer" class="small">--</div></div>
  <div class="card"><div class="label">Camera frame age</div><div id="camera_age" class="value">--</div></div>
  <div class="card"><div class="label">Turns / encoder</div><div id="turns" class="value">--</div><div id="ticks" class="small"></div></div>
  <div class="card wide"><div class="label">Last event / stop reason</div><div id="event" class="small">--</div><div id="fault" class="small bad"></div></div>
</div>
<div class="footer">Read-only dashboard. No motor control is exposed over Wi-Fi. SSID: HyperLine • 192.168.4.1</div>
<script>
const el=id=>document.getElementById(id), chart=el('chart');
let history=[],latest=0;
function graph(){
 const ctx=chart.getContext('2d'),dpr=devicePixelRatio||1,w=chart.clientWidth,h=chart.clientHeight;
 chart.width=Math.floor(w*dpr);chart.height=Math.floor(h*dpr);ctx.setTransform(dpr,0,0,dpr,0,0);
 ctx.clearRect(0,0,w,h);const pad={l:44,r:10,t:16,b:23};const cw=w-pad.l-pad.r,ch=h-pad.t-pad.b;
 if(history.length<2)return;
 const values=history.flatMap(v=>[v[1],v[2]]).filter(Number.isFinite);if(!values.length)return;
 let lo=Math.min(...values),hi=Math.max(...values);const margin=Math.max((hi-lo)*.12,7);lo-=margin;hi+=margin;
 const x0=history[0][0],x1=history[history.length-1][0];
 const xp=t=>pad.l+(t-x0)/Math.max(1,x1-x0)*cw,yp=v=>pad.t+(hi-v)/(hi-lo)*ch;
 ctx.lineWidth=1;ctx.strokeStyle='#355064';ctx.fillStyle='#9aadc4';ctx.font='11px system-ui';
 for(let i=0;i<=4;i++){const v=lo+(hi-lo)*i/4,y=yp(v);ctx.beginPath();ctx.moveTo(pad.l,y);ctx.lineTo(w-pad.r,y);ctx.stroke();ctx.fillText(v.toFixed(0)+'°',3,y+4)}
 [[1,'#6bdfff'],[2,'#ffbc73']].forEach(([idx,color])=>{
 ctx.lineWidth=2;ctx.strokeStyle=color;ctx.beginPath();history.forEach((s,i)=>{let x=xp(s[0]),y=yp(s[idx]);i?ctx.lineTo(x,y):ctx.moveTo(x,y)});ctx.stroke();
 });
 ctx.fillStyle='#90a9be';ctx.fillText((x1-x0)/1000+' sec',pad.l,h-5)
}
async function loadHistory(){try{const r=await fetch('/history',{cache:'no-store'});if(r.ok){history=await r.json();graph()}}catch(e){}}
function set(k,v){el(k).textContent=v}
function drawMatrix(name, data) {
 const grid=el('tof_'+name+'_matrix');
 if(!grid) return;
 const valid=Array.isArray(data?.matrix)&&data.matrix.length===16;
 const age=Number(data?.age??-1);
 grid.classList.toggle('stale',age>450||age<0);
 // The display never invents values: an invalid cell is --, with its raw
 // ST status in the small caption and hover title.
 grid.innerHTML='';
 for(let i=0;i<16;i++){
   const v=valid?Number(data.matrix[i]):-1;
   const st=Array.isArray(data?.status)?Number(data.status[i]):0;
   const cell=document.createElement('div');
   cell.className='matrix-cell'+(v<=0?' invalid':st===9?' merged':'')+((i===6||i===10)?' selected':'');
   cell.textContent=v>0?String(v):'--';
   const sub=document.createElement('span');sub.textContent='R'+(Math.floor(i/4)+1)+'C'+(i%4+1)+'  s'+st;
   cell.appendChild(sub);
   cell.title='Row '+(Math.floor(i/4)+1)+' Col '+(i%4+1)+' | ST target status '+st;
   grid.appendChild(cell);
 }
}
function drawTof(name, data) {
 drawMatrix(name,data);
 const mm=v=>v>0?v+' mm':'--';
 if (!data || !data.online) {set('tof_'+name,'OFFLINE');set('tof_'+name+'_status','Sensor not initialized');return;}
 set('tof_'+name,'R2C3 '+mm(data.r2c3));
 const stale=data.age<0?'NO FRAMES':data.age>450?'STALE':'LIVE';
 set('tof_'+name+'_status','R3C3 '+mm(data.r3c3)+' | '+stale+' | '+(data.age<0?'--':data.age+' ms old'));
}

async function update(){try{
 const response=await fetch('/api',{cache:'no-store'});if(!response.ok)throw Error(response.status);
 const d=await response.json();set('connected','Live • '+Math.round(d.ms/1000)+'s');el('connected').className='small good';
 set('yaw',Number(d.gyro).toFixed(2)+'°');
 for(const k of ['target','error','rate','bias'])set(k,Number(d[k]).toFixed(k==='bias'?4:2)+(k==='rate'||k==='bias'?' °/s':'°'));
 for(const k of ['accel_x','accel_y','accel_z','accel_norm'])set(k,Number(d[k]).toFixed(3));
 for(const k of ['roll','pitch'])set(k,Number(d[k]).toFixed(2)+'°');
 set('pwm',d.pwm);set('state',d.state);set('cube_phase',d.cube_phase);set('park_stage',d.park_stage);
 const leftCount=d.parking_walls_left||0,rightCount=d.parking_walls_right||0;
 set('park_wall_count',Math.max(leftCount,rightCount)+' / 2');
 set('park_wall_status','Tracking '+(['none','LEFT','RIGHT'][d.parking_active_side||0])+' | LEFT '+leftCount+' | RIGHT '+rightCount+' | confirmed '+(['none','LEFT','RIGHT'][d.parking_selected_side||0]));
 const fmt=a=>Array.isArray(a)?a.map(v=>v>0?v:'--').join(' / '):'--';
 set('park_columns','LEFT: '+fmt(d.parking_rows_left)+' mm | RIGHT: '+fmt(d.parking_rows_right)+' mm | travel '+((d.parking_scan_mm||0)/10).toFixed(1)+' cm');
 for(const name of ['front','left','right','rear']) drawTof(name,d['tof_'+name]);
 set('front_a',d.front_a>0?d.front_a+' mm':'--');set('front_b',d.front_b>0?d.front_b+' mm':'--');set('front_left',d.front_left>0?d.front_left+' mm':'--');set('front_right',d.front_right>0?d.front_right+' mm':'--');set('side',d.side>0?d.side+' mm':'--');
 set('tcs_detected',d.tcs_detected||'--');
 set('tcs_latched','Latched: '+(d.tcs_latched||'--'));
 set('tcs_raw','C='+d.tcs_clear+' | R='+d.tcs_red+' | G='+d.tcs_green+' | B='+d.tcs_blue);
 set('tcs_ratios','R '+d.tcs_r_permille+' / G '+d.tcs_g_permille+' / B '+d.tcs_b_permille+' (per 1000)');
 set('tcs_status','Marker '+(d.tcs_armed?'ARMED':'NOT ARMED')+' | sample '+(d.tcs_age_ms<0?'missing':d.tcs_age_ms+' ms old')+' | '+(d.tcs_ready?'sensor ready':'sensor not ready'));
 set('lane_status',d.lane_centering?(d.lane_odometry?'GYRO + ENCODER RETURN':'SIDE ToF CENTERING'):'NORMAL');
 set('lane_dists',d.lane_odometry?('Estimated sideways: '+Number(d.lane_estimated_offset_mm).toFixed(0)+' mm | return travel '+Number(d.lane_odometry_travel_mm).toFixed(0)+' mm | yaw trim '+d.lane_yaw_bias.toFixed(1)+'°'):('LEFT '+(d.lane_left_mm>0?d.lane_left_mm.toFixed(0)+' mm':'--')+' | RIGHT '+(d.lane_right_mm>0?d.lane_right_mm.toFixed(0)+' mm':'--')+' | yaw trim '+d.lane_yaw_bias.toFixed(1)+'°'));
 set('last_camera',d.last_camera);set('camera_steer','S steering '+d.camera_steer.toFixed(3)+' | bypass frames '+d.follow_seen);set('camera_age',d.camera_age+' ms');set('turns',d.turns);set('ticks','Encoder: '+d.ticks+' ticks');
 set('event',d.event);set('fault',d.fault);el('error').className='value '+(Math.abs(d.error)>8?'warn':'good');
 if(d.ms>latest){history.push([d.ms,d.gyro,d.target,d.rate,d.pwm,0]);latest=d.ms;const cutoff=d.ms-48000;while(history.length&&history[0][0]<cutoff)history.shift();graph()}
 }catch(e){set('connected','Disconnected • '+String(e.message||e));el('connected').className='small bad'}
}
let polling=false;
async function poll(){if(!polling){polling=true;try{await update()}finally{polling=false}}setTimeout(poll,350)}
loadHistory().then(poll);
</script></body></html>
)HLRHTML";

void telemetry_setup() {
#if !HLR_ENABLE_WIFI_TELEMETRY
  WiFi.mode(WIFI_OFF);
  telemetryReady = false;
  Serial.println("TELEMETRY: Wi-Fi radio OFF (enable at top of Telemetry.ino)");
  return;
#else
  // The AP is available in BOTH Open and Final.
  WiFi.mode(WIFI_AP);
  bool apReady = WiFi.softAP("HyperLine", "HyperLine-FE");
  if (!apReady) {
    delay(200);
    apReady = WiFi.softAP("HyperLine", "HyperLine-FE");
  }
  if (!apReady) {
    Serial.println("TELEMETRY: Wi-Fi AP failed after retry (robot can still run)");
    return;
  }
  telemetryServer.on("/", HTTP_GET, []() {
    telemetryServer.send_P(200, "text/html; charset=utf-8", TELEMETRY_HTML);
  });
  telemetryServer.on("/api", HTTP_GET, []() {
    telemetryLastClientMs = millis();
    telemetryServer.send(200, "application/json", telemetry_json());
  });
  telemetryServer.on("/history", HTTP_GET, []() {
    telemetry_send_history();
  });
  telemetryServer.on("/parking-log.csv", HTTP_GET, []() {
    telemetry_send_parking_log();
  });
  telemetryServer.on("/color-log.csv", HTTP_GET, []() {
    telemetry_send_color_log();
  });
  telemetryServer.begin();
  telemetryReady = true;
  Serial.print("TELEMETRY: HyperLine / HyperLine-FE at http://");
  Serial.println(WiFi.softAPIP());
#endif
}

void telemetry_tick() {
#if !HLR_ENABLE_WIFI_TELEMETRY
  return;
#endif
  if (!telemetryReady) return;
  imu_read_acceleration();
  if (telemetryFrontTestStopped && millis() - telemetryLastFrontMs >= 120UL) {
    telemetryLastFrontMs = millis();
    tof_poll_front_only();
    telemetry_front_cells(tof_zone_mm(FRONT_DIR, 2, 3),
                          tof_zone_mm(FRONT_DIR, 3, 3));
    int l2=tof_zone_mm(FRONT_DIR,2,2),l3=tof_zone_mm(FRONT_DIR,3,2);
    int r2=tof_zone_mm(FRONT_DIR,2,4),r3=tof_zone_mm(FRONT_DIR,3,4);
    telemetryFrontLeft=(l2>0 && l3>0) ? (l2+l3)/2 : -1;
    telemetryFrontRight=(r2>0 && r3>0) ? (r2+r3)/2 : -1;
  }
  telemetry_record_sample();
  telemetry_color_record();
  const uint32_t now = millis();
  // Poll one sensor per cycle, and only while a dashboard is connected.
  // Unlike a four-sensor bulk read, this avoids long blocking bursts during
  // fast cube and gyro steering. Cached frames also update from mission reads.
  if (telemetryLastClientMs && (uint32_t)(now - telemetryLastClientMs) < 5000UL &&
      (uint32_t)(now - telemetryLastTofPollMs) >= 50UL) {
    telemetryLastTofPollMs = now;
    tof_poll_one(telemetryNextTofIndex);
    telemetryNextTofIndex = (telemetryNextTofIndex + 1) % 4;
  }
  if ((uint32_t)(now - telemetryLastTickMs) <
      (telemetryMotorPwm != 0 ? 80UL : 55UL)) return;
  telemetryLastTickMs = now;
  static bool busy = false;
  if (busy) return;
  busy = true;
  telemetryServer.handleClient();
  busy = false;
}
