// ============================================================================
// HYPERLINE ROBOTICS | WRO 2026 FUTURE ENGINEERS
// VL53L5CX | addressing, 4x4 ranging and measurement validity
// ============================================================================

// HyperLine Robotics: four VL53L5CXV0GC/1 sensors
// SparkFun VL53L5CX Arduino Library required.
// The addresses below are 7-bit I2C addresses, set on every boot.
// Front: LPn A3, 0x30 | Left: LPn A2, 0x31
// Right: LPn A1, 0x32 | Back: LPn A0, 0x33
//
// 4x4 at 30 Hz: faster updates than 8x8 (15 Hz maximum).
// Column and row numbers below are 1-based, as in the request.

#define TOF_FRAME_SIDE 4
#define TOF_RANGING_HZ 30
#define TOF_FRESH_MS 150UL
#define TOF_TURN_MM 700
#define TOF_SLOW_MM 1000  // Open: decelerate below 1000 mm, turn below 700 mm

SparkFun_VL53L5CX tofFront, tofLeft, tofRight, tofBack;

struct ToFUnit {
  SparkFun_VL53L5CX *sensor;
  uint8_t lpn;
  uint8_t address;
  bool online;
  uint16_t mm[16];
  uint32_t lastFrameMs;
  uint8_t status[16]; // Unfiltered ST target status for invalid-cell diagnosis.
};

ToFUnit tofUnits[4] = {
  { &tofFront, TOF_LPN_FRONT, 0x30, false, { 0 }, 0 },
  { &tofLeft, TOF_LPN_LEFT, 0x31, false, { 0 }, 0 },
  { &tofRight, TOF_LPN_RIGHT, 0x32, false, { 0 }, 0 },
  { &tofBack, TOF_LPN_BACK, 0x33, false, { 0 }, 0 }
};

bool tofTurnArmed = true;
bool tofTurnPending = false;

// Hold all LPn lines low before enabling sensors one at a time to prevent
// default-address collisions.
void tof_prepare_pins() {
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(tofUnits[i].lpn, LOW);
    pinMode(tofUnits[i].lpn, OUTPUT);
  }
}

// Initialize the shared I2C bus configuration and assign each module its own
// address before starting 4x4 ranging.
void tof_setup() {
  // gyro_setup() already calls Wire.begin(). The TCS uses another bus.
  Wire.setClock(400000);  // Compatible with the BMI088 on this shared bus.
  delay(15);

  for (uint8_t i = 0; i < 4; i++) {
    ToFUnit &u = tofUnits[i];
    digitalWrite(u.lpn, HIGH);
    delay(10);
    // Every newly enabled sensor starts at the default address 0x29.
    if (!u.sensor->begin(0x29, Wire)) {
      Serial.print("ToF start failed: ");
      Serial.println(i);
      digitalWrite(u.lpn, LOW);
      continue;
    }
    if (!u.sensor->setAddress(u.address)) {
      Serial.print("ToF address failed: ");
      Serial.println(i);
      digitalWrite(u.lpn, LOW);
      continue;
    }
    if (!u.sensor->setResolution(TOF_FRAME_SIDE * TOF_FRAME_SIDE) || !u.sensor->setRangingFrequency(TOF_RANGING_HZ) || !u.sensor->startRanging()) {
      Serial.print("ToF ranging failed: ");
      Serial.println(i);
      digitalWrite(u.lpn, LOW);
      continue;
    }
    u.online = true;
    Serial.print("ToF ready at 0x");
    Serial.println(u.address, HEX);
  }
}


// Only during FINAL parking, change the selected side module from 30Hz to
// 60Hz at 4x4. Front/back continue at 30Hz. Restore 30Hz on failure.
// The side will remain at 60Hz until reboot (parking ends in PARK state).
// Attempt 60 Hz on the selected parking-side module; restore 30 Hz on failure
// when possible.
bool tof_enable_fast_parking_side(DistanceDir dir) {
  if ((uint8_t)dir >= 4) return false;
  ToFUnit &u = tofUnits[(uint8_t)dir];
  if (!u.online) return false;
  if (u.sensor->getRangingFrequency() == 60) return true;
  if (!u.sensor->stopRanging()) return false; // Still ranging at 30Hz
  if (u.sensor->setRangingFrequency(60) && u.sensor->startRanging()) {
    u.lastFrameMs = 0;
    return true;
  }
  // Recover safely at the original frequency instead of leaving the
  // configured side sensor stopped mid-parking.
  u.sensor->stopRanging();
  if (u.sensor->setRangingFrequency(30) && u.sensor->startRanging()) {
    u.lastFrameMs = 0;
    return false;
  }
  u.online = false;
  return false;
}

// Read one zone in row-major order. Rows/columns are 1-based (1..4).
// The SparkFun/ST zone buffer may appear mirrored or transposed depending
// on the physical mounting orientation. Verify the intended zone once.
// Return a fresh valid zone distance in millimeters, or -1 if the zone or
// cached frame is unusable.
int tof_zone_mm(DistanceDir dir, uint8_t row, uint8_t col) {
  if (row < 1 || row > 4 || col < 1 || col > 4) return -1;
  ToFUnit &u = tofUnits[(uint8_t)dir];
  if (!u.online || !u.lastFrameMs || millis() - u.lastFrameMs > TOF_FRESH_MS) return -1;
  uint16_t v = u.mm[(row - 1) * 4 + (col - 1)];
  return v == 0 ? -1 : (int)v;
}

// Raw ST target status. -1 is NOT a negative distance: it means the
// measured target failed the firmware validity gate (status not 5 or 9),
// returned zero, or the cached frame became stale.
// Expose the cached raw target status for diagnostics, independently of the
// filtered distance.
int tof_zone_target_status(DistanceDir dir, uint8_t row, uint8_t col) {
  if ((uint8_t)dir >= 4 || row < 1 || row > 4 || col < 1 || col > 4) return 0;
  ToFUnit &u = tofUnits[(uint8_t)dir];
  if (!u.online || !u.lastFrameMs) return 0;
  return (int)u.status[(row-1)*4 + (col-1)];
}

// Telemetry-only snapshot access. Unlike navigation's tof_zone_mm(), these
// preserve the last value when older than TOF_FRESH_MS and report their age.
// The webpage must label old frames STALE; they are never used for driving.
// Expose the timestamp of the last successful frame transfer for age checks.
uint32_t tof_last_frame_ms(DistanceDir dir) {
  if ((uint8_t)dir >= 4) return 0;
  return tofUnits[(uint8_t)dir].lastFrameMs;
}
// Report whether the selected module initialized successfully and remains
// online.
bool tof_sensor_online(DistanceDir dir) {
  return (uint8_t)dir < 4 && tofUnits[(uint8_t)dir].online;
}
// Return the cached filtered zone for display even when stale; the caller
// reports frame age separately.
int tof_telemetry_zone_mm(DistanceDir dir, uint8_t row, uint8_t col) {
  if ((uint8_t)dir >= 4 || row < 1 || row > 4 || col < 1 || col > 4) return -1;
  const ToFUnit &u = tofUnits[(uint8_t)dir];
  if (!u.online || !u.lastFrameMs) return -1;
  const uint16_t v = u.mm[(row - 1) * 4 + (col - 1)];
  return v ? (int)v : -1;
}

// Read a single module without delaying the cube PID on three unnecessary I2C transfers.
// Transfer one ready frame and cache all sixteen statuses and valid distances.
void tof_poll_one(uint8_t index) {
  if (index >= 4) return;
  ToFUnit &u = tofUnits[index];
  if (!u.online || !u.sensor->isDataReady()) return;
  VL53L5CX_ResultsData frame;
  if (!u.sensor->getRangingData(&frame)) return;
  for (uint8_t z = 0; z < 16; z++) {
    uint8_t s = frame.target_status[z];
    u.status[z] = s;
    u.mm[z] = ((s == 5 || s == 9) && frame.distance_mm[z] > 0)
              ? frame.distance_mm[z] : 0;
  }
  u.lastFrameMs = millis();
}

// Refresh only the front module to avoid unnecessary bus transfers.
void tof_poll_front_only() {
  tof_poll_one((uint8_t)FRONT_DIR);
}

// Read the two selected cells from one NEW sensor frame.
// Callers keep their own lastSeenFrameMs so an old frame cannot count twice.
// Read R2C3 and R3C3 from one fresh frame and update the caller-owned frame
// timestamp.
bool tof_read_new_pair(DistanceDir dir, uint32_t &lastSeenFrameMs, int &a, int &b) {
  a = -1;
  b = -1;
  const uint8_t index = (uint8_t)dir;
  if (index >= 4) return false;

  tof_poll_one(index);
  ToFUnit &unit = tofUnits[index];
  if (!unit.online || unit.lastFrameMs == 0 ||
      unit.lastFrameMs == lastSeenFrameMs ||
      (uint32_t)(millis() - unit.lastFrameMs) > TOF_FRESH_MS) {
    return false;
  }

  lastSeenFrameMs = unit.lastFrameMs;
  a = tof_zone_mm(dir, 2, 3);
  b = tof_zone_mm(dir, 3, 3);
  return a > 0 && b > 0;
}

// Returns a new frame's single selected zone, with its own caller timestamp.
// An invalid cell still consumes the frame so it cannot count repeatedly.
// Read one selected zone from a new frame; consume invalid frames too so they
// cannot be counted repeatedly.
bool tof_read_new_zone(DistanceDir dir, uint32_t &lastSeenFrameMs,
                       uint8_t row, uint8_t col, int &mm) {
  mm = -1;
  const uint8_t index = (uint8_t)dir;
  if (index >= 4) return false;
  tof_poll_one(index);
  ToFUnit &u = tofUnits[index];
  if (!u.online || !u.lastFrameMs || u.lastFrameMs == lastSeenFrameMs ||
      (uint32_t)(millis() - u.lastFrameMs) > TOF_FRESH_MS) return false;
  lastSeenFrameMs = u.lastFrameMs;
  mm = tof_zone_mm(dir, row, col);
  return mm > 0;
}

// Existing movement and parking APIs use a single distance per direction.
// Use the nearest valid region in the central 2x2 zones.
// Provide the mode-specific scalar distance: Final averages two central cells;
// Open chooses the nearest valid central 2x2 cell.
float readDistanceMM(DistanceDir dir) {
  if (activeMode == MODE_FINAL) {
    // For parking/exit, update only the ToF being queried.
    tof_poll_one((uint8_t)dir);
    const int row2 = tof_zone_mm(dir, 2, 3);
    const int row3 = tof_zone_mm(dir, 3, 3);
    // Both samples must be fresh and valid. Average stabilizes target alignment.
    if (row2 <= 0 || row3 <= 0) return -1.0f;
    return ((float)row2 + (float)row3) * 0.5f;
  }
  tof_poll();
  int nearest = 32767;
  for (uint8_t row = 2; row <= 3; row++) {
    for (uint8_t col = 2; col <= 3; col++) {
      int d = tof_zone_mm(dir, row, col);
      if (d > 0 && d < nearest) nearest = d;
    }
  }
  return (nearest == 32767) ? -1.0f : (float)nearest;
}

// Convenience wrapper for the left-facing module.
int readLeftDistanceMM() {
  return (int)readDistanceMM(LEFT_DIR);
}
// Convenience wrapper for the right-facing module.
int readRightDistanceMM() {
  return (int)readDistanceMM(RIGHT_DIR);
}
// Convenience wrapper for the forward-facing module.
float readFrontDistanceMM() {
  return readDistanceMM(FRONT_DIR);
}

// Continuously report one direction over serial for bench diagnostics.
void debug_distance_infinite(DistanceDir dir) {
  while (true) {
    float d = readDistanceMM(dir);
    Serial.print("ToF ");
    Serial.print((int)dir);
    Serial.print(" distance: ");
    Serial.println(d);
    delay(40);
  }
}

// Refresh ready modules, limiting Open-mode transfers to the front sensor.
void tof_poll() {
  // Open round uses only the front matrix for corner control.
  // Avoid transferring the other three matrices during first-marker detection.
  const uint8_t pollCount = (activeMode == MODE_OPEN) ? 1 : 4;
  for (uint8_t i = 0; i < pollCount; i++) {
    ToFUnit &u = tofUnits[i];
    if (!u.online || !u.sensor->isDataReady()) continue;
    VL53L5CX_ResultsData frame;
    if (!u.sensor->getRangingData(&frame)) continue;
    for (uint8_t z = 0; z < 16; z++) {
      // 5: valid range; 9: valid but large pulse. Reject other statuses.
      uint8_t s = frame.target_status[z];
      u.status[z] = s;
      u.mm[z] = ((s == 5 || s == 9) && frame.distance_mm[z] > 0)
                  ? frame.distance_mm[z]
                  : 0;
    }
    u.lastFrameMs = millis();
  }

  // Open round manages its own fresh-frame confirmation and re-arming.
  // Do not create cached corner events here: they could retrigger mid-turn.

}

// Consume a pending front-turn event only after travel direction is
// established.
bool tof_consume_front_turn() {
  if (!tofTurnPending) return false;
  // Leave event pending until direction is established by first color marker.
  if (turn_direction == 0) return false;
  tofTurnPending = false;
  return true;
}

// Compute the existing front-range-dependent Open speed limit.
int tof_open_round_speed(int normalSpeed) {
  const int row2 = tof_zone_mm(FRONT_DIR, 2, 3);
  const int row3 = tof_zone_mm(FRONT_DIR, 3, 3);
  // Never race at full speed when a required wall-measurement cell is missing.
  if (row2 <= 0 || row3 <= 0) return normalSpeed < 85 ? normalSpeed : 85;
  if (abs(row2 - row3) > 220) return min(normalSpeed, 80);
  const int closest = min(row2, row3);
  if (closest >= TOF_SLOW_MM) return normalSpeed;
  if (closest <= TOF_TURN_MM) return min(normalSpeed, 80);
  const int slowPWM = min(normalSpeed, 80);
  return slowPWM + (int)((long)(normalSpeed - slowPWM) *
                          (closest - TOF_TURN_MM) / (TOF_SLOW_MM - TOF_TURN_MM));
}

// True only with a recent, valid pair of front center-column measurements.
// Evaluate the existing front-wall proximity predicate.
bool tof_front_wall_close() {
  int a = tof_zone_mm(FRONT_DIR, 2, 3);
  int b = tof_zone_mm(FRONT_DIR, 3, 3);
  return a > 0 && b > 0 && a < TOF_TURN_MM && b < TOF_TURN_MM;
}
