#define DRIFT_TEST_TIME 10

BMI088 imu(BMI088_ACC_ADDRESS, BMI088_GYRO_ADDRESS);

bool debugGyro = false;
bool gyroAvailable = false;
double gyroRateDps = 0.0;
float imuAccelX = 0.0f, imuAccelY = 0.0f, imuAccelZ = 0.0f;
float imuPitchDeg = 0.0f, imuRollDeg = 0.0f, imuAccelMagnitude = 0.0f;
static uint32_t imuLastAccelMs = 0;
void imu_read_acceleration() {
  // Accelerometer gives pitch/roll relative to gravity, not absolute yaw.
  if (!gyroAvailable || millis() - imuLastAccelMs < 120UL) return;
  imuLastAccelMs = millis();
  imuAccelX = imu.getAccelerationX();
  imuAccelY = imu.getAccelerationY();
  imuAccelZ = imu.getAccelerationZ();
  if (isfinite(imuAccelX) && isfinite(imuAccelY) && isfinite(imuAccelZ)) {
    const float x = imuAccelX, y = imuAccelY, z = imuAccelZ;
    imuAccelMagnitude = sqrtf(x*x + y*y + z*z);
    imuRollDeg = atan2f(y, z) * 180.0f / PI;
    imuPitchDeg = atan2f(-x, sqrtf(y*y + z*z)) * 180.0f / PI;
  }
}

uint32_t gyroLastMicros = 0;
static double gyroPreviousRateDps = 0.0;

// Z is the yaw axis for THIS mounting. The reference code uses X on a
// differently mounted BMI088; do not change axes without a rotation test.
static const double GYRO_STILL_RATE_LIMIT_DPS = 1.50;
static const double GYRO_BIAS_UPDATE_ALPHA = 0.003;
static const uint32_t GYRO_STILL_REQUIRED_US = 800000UL;
static const double GYRO_MAX_NORMAL_DT_S = 0.12;

volatile int gyroMotorPwm = 0;
static uint32_t gyroStoppedAtUs = 0;
static uint32_t gyroStillSinceUs = 0;
static int32_t gyroLastEncoderTicks = 0;
static uint32_t gyroLongGapCount = 0;

void gyro_note_motor_speed(int commandedPwm) {
  const int power = abs(commandedPwm);
  if (power > 0) {
    gyroStoppedAtUs = 0;
    gyroStillSinceUs = 0;
  } else if (gyroMotorPwm != 0 || gyroStoppedAtUs == 0) {
    gyroStoppedAtUs = micros();
    gyroStillSinceUs = 0;
  }
  gyroMotorPwm = power;
}

void gyro_setup() {
  Wire.begin();
  imu.initialize();
  imu.setAccScaleRange(RANGE_6G);
  imu.setAccOutputDataRate(ODR_200);
  imu.setAccPoweMode(ACC_ACTIVE);
  imu.setGyroScaleRange(RANGE_2000);
  imu.setGyroOutputDataRate(ODR_400_BW_47);
  imu.setGyroPoweMode(GYRO_NORMAL);

  gyroAvailable = imu.isConnection();
  if (!gyroAvailable) {
    Serial.println("GYRO ERROR: BMI088 connection failed");
    return;
  }

  delay(150);
  double total = 0.0;
  double totalSquares = 0.0;
  uint32_t samples = 0;
  uint32_t started = millis();
  while ((uint32_t)(millis() - started) < DRIFT_TEST_TIME * 1000UL) {
    const double raw = (double)imu.getGyroscopeZ();
    if (isfinite(raw)) { total += raw; totalSquares += raw * raw; ++samples; }
    delay(4);
  }
  if (samples < 100) {
    gyroAvailable = false;
    Serial.println("GYRO ERROR: no valid calibration samples");
    return;
  }
  drifts_z = total / samples;
  const double variance = totalSquares / samples - drifts_z * drifts_z;
  const double gyroStdDev = sqrt(variance > 0.0 ? variance : 0.0);
  Serial.print("GYRO calibration rate stddev (deg/s): ");
  Serial.println(gyroStdDev, 4);
  if (gyroStdDev > 1.2) {
    gyroAvailable = false;
    Serial.println("GYRO ERROR: sensor moved during stationary startup calibration");
    return;
  }
  gyroRateDps = 0.0;
  gyroPreviousRateDps = 0.0;
  gz = 0.0;
  gyroLastMicros = micros();
  gyro_last_read_time = millis();
  gyroLastEncoderTicks = encoder_ticks;
  gyroStoppedAtUs = gyroLastMicros;
  gyroStillSinceUs = 0;
  Serial.print("GYRO bias Z (deg/s): ");
  Serial.println(drifts_z, 6);
}

void read_gyro_data() {
  if (!gyroAvailable) return;
  const double raw = (double)imu.getGyroscopeZ();
  const uint32_t nowUs = micros();
  const double dt = (uint32_t)(nowUs - gyroLastMicros) * 0.000001;
  gyroLastMicros = nowUs;
  if (!isfinite(raw) || !isfinite(dt)) {
    gyroAvailable = false;
    Serial.println("GYRO ERROR: invalid rate/timestamp");
    return;
  }

  const double rate = raw - drifts_z;
  if (dt > 0.0) {
    // Integrate the elapsed time even after I2C delays. Dropping a long
    // interval would permanently lose part of a parking turn.
    gz += (gyroPreviousRateDps + rate) * 0.5 * dt;
    if (dt > GYRO_MAX_NORMAL_DT_S) {
      ++gyroLongGapCount;
      if (gyroLongGapCount <= 5 || gyroLongGapCount % 100 == 0) {
        Serial.print("GYRO WARNING: long sensor gap ms=");
        Serial.println(dt * 1000.0, 1);
      }
    }
  }
  gyroPreviousRateDps = rate;
  gyroRateDps = rate;
  gyro_last_read_time = millis();

  // Stationary-only zero-rate update. Do not calibrate during slow turning.
  // Both motors commanded STOP and unchanged encoder ticks are required.
  const int32_t ticks = encoder_ticks;
  const bool encoderUnchanged = ticks == gyroLastEncoderTicks;
  gyroLastEncoderTicks = ticks;
  const bool stillCandidate = gyroMotorPwm == 0 &&
      gyroStoppedAtUs != 0 &&
      (uint32_t)(nowUs - gyroStoppedAtUs) >= GYRO_STILL_REQUIRED_US &&
      encoderUnchanged && fabs(rate) < GYRO_STILL_RATE_LIMIT_DPS;

  if (!stillCandidate) {
    gyroStillSinceUs = 0;
  } else {
    if (gyroStillSinceUs == 0) gyroStillSinceUs = nowUs;
    if ((uint32_t)(nowUs - gyroStillSinceUs) >= 100000UL) {
      drifts_z += (raw - drifts_z) * GYRO_BIAS_UPDATE_ALPHA;
      gyroPreviousRateDps = raw - drifts_z;
    }
  }

  telemetry_tick();
  if (debugGyro) {
    Serial.print("Gyro yaw:"); Serial.print(gz, 3);
    Serial.print(" dps:"); Serial.print(gyroRateDps, 4);
    Serial.print(" bias:"); Serial.println(drifts_z, 5);
  }
}

void recalibrate_gyro_angle(double expected_angle) {
  // Only call this when the heading is known from an EXTERNAL reference.
  // Do not force the accumulated angle to the commanded target after a turn.
  read_gyro_data();
  gz = expected_angle;
  gyroLastMicros = micros();
  gyroPreviousRateDps = gyroRateDps;
  gyro_last_read_time = millis();
}
