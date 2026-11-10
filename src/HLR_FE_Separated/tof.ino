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
#define TOF_TURN_MM 600
#define TOF_SLOW_MM 800

SparkFun_VL53L5CX tofFront, tofLeft, tofRight, tofBack;

struct ToFUnit {
  SparkFun_VL53L5CX *sensor;
  uint8_t lpn;
  uint8_t address;
  bool online;
  uint16_t mm[16];
  uint32_t lastFrameMs;
};

ToFUnit tofUnits[4] = {
  { &tofFront, TOF_LPN_FRONT, 0x30, false, { 0 }, 0 },
  { &tofLeft, TOF_LPN_LEFT, 0x31, false, { 0 }, 0 },
  { &tofRight, TOF_LPN_RIGHT, 0x32, false, { 0 }, 0 },
  { &tofBack, TOF_LPN_BACK, 0x33, false, { 0 }, 0 }
};

bool tofTurnArmed = true;
bool tofTurnPending = false;

void tof_prepare_pins() {
  for (uint8_t i = 0; i < 4; i++) {
    digitalWrite(tofUnits[i].lpn, LOW);
    pinMode(tofUnits[i].lpn, OUTPUT);
  }
}

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

// Read one zone in row-major order. Rows/columns are 1-based (1..4).
// The SparkFun/ST zone buffer may appear mirrored or transposed depending
// on the physical mounting orientation. Verify the intended zone once.
int tof_zone_mm(DistanceDir dir, uint8_t row, uint8_t col) {
  if (row < 1 || row > 4 || col < 1 || col > 4) return -1;
  ToFUnit &u = tofUnits[(uint8_t)dir];
  if (!u.online || !u.lastFrameMs || millis() - u.lastFrameMs > TOF_FRESH_MS) return -1;
  uint16_t v = u.mm[(row - 1) * 4 + (col - 1)];
  return v == 0 ? -1 : (int)v;
}

// Existing movement and parking APIs use a single distance per direction.
// Use the nearest valid region in the central 2x2 zones.
float readDistanceMM(DistanceDir dir) {
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

int readLeftDistanceMM() {
  return (int)readDistanceMM(LEFT_DIR);
}
int readRightDistanceMM() {
  return (int)readDistanceMM(RIGHT_DIR);
}
float readFrontDistanceMM() {
  return readDistanceMM(FRONT_DIR);
}

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
      u.mm[z] = ((s == 5 || s == 9) && frame.distance_mm[z] > 0)
                  ? frame.distance_mm[z]
                  : 0;
    }
    u.lastFrameMs = millis();
  }

  if (activeMode != MODE_OPEN) return;
  const int row2 = tof_zone_mm(FRONT_DIR, 2, 3);
  const int row3 = tof_zone_mm(FRONT_DIR, 3, 3);
  const bool bothValid = row2 > 0 && row3 > 0;
  const bool close = bothValid && row2 < TOF_TURN_MM && row3 < TOF_TURN_MM;
  const bool clear = bothValid && row2 >= TOF_SLOW_MM && row3 >= TOF_SLOW_MM;

  if (clear) {
    tofTurnArmed = true;
    tofTurnPending = false;  // Discard a previous corner if it was never consumed.
  }
  if (close && tofTurnArmed) {
    tofTurnPending = true;
    tofTurnArmed = false;
  }
}

bool tof_consume_front_turn() {
  if (!tofTurnPending) return false;
  // Leave event pending until direction is established by first color marker.
  if (turn_direction == 0) return false;
  tofTurnPending = false;
  return true;
}

int tof_open_round_speed(int normalSpeed) {
  const int row2 = tof_zone_mm(FRONT_DIR, 2, 3);
  const int row3 = tof_zone_mm(FRONT_DIR, 3, 3);
  // Missing/stale sensor frames do not cause speed changes.
  if (row2 <= 0 || row3 <= 0) return normalSpeed;
  if (row2 < TOF_SLOW_MM || row3 < TOF_SLOW_MM)
    return (normalSpeed * 40) / 100;
  return normalSpeed;
}

// True only with a recent, valid pair of front center-column measurements.
bool tof_front_wall_close() {
  int a = tof_zone_mm(FRONT_DIR, 2, 3);
  int b = tof_zone_mm(FRONT_DIR, 3, 3);
  return a > 0 && b > 0 && a < TOF_TURN_MM && b < TOF_TURN_MM;
}
