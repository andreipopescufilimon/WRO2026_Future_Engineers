#define DRIFT_TEST_TIME 10  // stationary calibration at startup, seconds

BMI088 imu(BMI088_ACC_ADDRESS, BMI088_GYRO_ADDRESS);

bool debugGyro = false;
double gyroRateDps = 0.0;
uint32_t gyroLastMicros = 0;

// Estimate the stationary bias directly in degrees/second, not from
// integrating an angle using mixed millisecond timestamps.
void gyro_setup() {
  Wire.begin();
  imu.initialize();
  imu.setAccScaleRange(RANGE_6G);
  imu.setAccOutputDataRate(ODR_200);
  imu.setAccPoweMode(ACC_ACTIVE);
  imu.setGyroScaleRange(RANGE_2000);
  imu.setGyroOutputDataRate(ODR_400_BW_47);
  imu.setGyroPoweMode(GYRO_NORMAL);

  if (!imu.isConnection()) {
    if (debugGyro) Serial.println("BMI088 connection failed!");
    return;
  }

  // Keep the robot completely stationary during this calibration.
  // Allow the sensor to begin producing stable measurements.
  delay(100);
  double sumRate = 0.0;
  uint32_t samples = 0;
  const uint32_t start = millis();
  while ((uint32_t)(millis() - start) < DRIFT_TEST_TIME * 1000UL) {
    sumRate += imu.getGyroscopeZ();
    ++samples;
    delay(4);  // close to the gyro's 400 Hz output data rate
  }
  drifts_z = samples ? sumRate / (double)samples : 0.0;
  gz = 0.0;
  gyroRateDps = 0.0;
  gyro_last_read_time = millis();
  gyroLastMicros = micros();
  if (debugGyro) {
    Serial.print("Stationary gyro bias (deg/s): ");
    Serial.println(drifts_z, 6);
  }
}

void read_gyro_data() {
  // Timestamp after the sensor read. This avoids integrating the previous
  // gyro sample across an unrelated I2C read using inconsistent timestamps.
  const float rate = imu.getGyroscopeZ();
  const uint32_t nowUs = micros();
  const double dt = (uint32_t)(nowUs - gyroLastMicros) * 0.000001;
  gyroLastMicros = nowUs;
  const double corrected = (double)rate - drifts_z;
  gyroRateDps = corrected;

  // A very long stalled loop does not represent a fresh angular-rate sample.
  // Avoid integrating a single stale reading over that entire interval.
  if (dt > 0.0 && dt <= 0.05) gz += corrected * dt;
  gyro_last_read_time = millis();

  if (debugGyro) {
    Serial.print("Gyro angle (deg): ");
    Serial.println(gz, 4);
  }
}

void recalibrate_gyro_angle(double expected_angle) {
  read_gyro_data();
  const double error = expected_angle - gz;
  gz = expected_angle;
  gyro_last_read_time = millis();
  gyroLastMicros = micros();
  if (debugGyro) {
    Serial.print("Gyro re-zero correction (deg): ");
    Serial.println(error, 4);
  }
}
