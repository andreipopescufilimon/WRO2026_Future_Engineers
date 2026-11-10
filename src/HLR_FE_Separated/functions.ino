TwoWire TCSWire(1);

void tcs_write8(uint8_t reg, uint8_t value) {
  TCSWire.beginTransmission(TCS34725_ADDR);
  TCSWire.write(TCS_CMD | reg);
  TCSWire.write(value);
  TCSWire.endTransmission();
}

uint8_t tcs_read8(uint8_t reg) {
  TCSWire.beginTransmission(TCS34725_ADDR);
  TCSWire.write(TCS_CMD | reg);
  TCSWire.endTransmission(false);

  TCSWire.requestFrom(TCS34725_ADDR, (uint8_t)1);

  if (TCSWire.available()) {
    return TCSWire.read();
  }

  return 0;
}

void tcs_setup_fast() {
  // Use a separate I2C bus for the color sensor
  TCSWire.begin(A6, A7);

  // I2C clock for TCS34725
  TCSWire.setClock(400000);

  // Power on
  tcs_write8(TCS_ENABLE, TCS_PON);
  delay(3);

  // Fastest integration
  tcs_write8(TCS_ATIME, TCS_ATIME_FAST);

  // Gain
  tcs_write8(TCS_CONTROL, TCS_GAIN_FAST);

  // Enable RGBC
  tcs_write8(TCS_ENABLE, TCS_PON | TCS_AEN);

  delay(3);

  tcsReady = true;
}

bool tcs_read_raw_fast() {
  if (!tcsReady) return false;

  // Sensor STATUS indicates when new color data is ready. No software delay gate.
  uint8_t status = tcs_read8(TCS_STATUS);

  // Bit 0 = valid color data
  if (!(status & 0x01)) {
    return false;
  }

  TCSWire.beginTransmission(TCS34725_ADDR);
  TCSWire.write(TCS_CMD | TCS_AUTO_INC | TCS_CDATAL);
  TCSWire.endTransmission(false);

  TCSWire.requestFrom(TCS34725_ADDR, (uint8_t)8);

  if (TCSWire.available() < 8) {
    return false;
  }

  tcsC = TCSWire.read();
  tcsC |= TCSWire.read() << 8;

  tcsR = TCSWire.read();
  tcsR |= TCSWire.read() << 8;

  tcsG = TCSWire.read();
  tcsG |= TCSWire.read() << 8;

  tcsB = TCSWire.read();
  tcsB |= TCSWire.read() << 8;

  tcsLastReadMicros = micros();

  return true;
}

TCSColor tcs_detect_color_fast() {
  if (tcsC < TCS_MIN_CLEAR) {
    return TCS_NONE;
  }

  uint32_t sum = (uint32_t)tcsR + tcsG + tcsB;
  if (sum < 10) {
    return TCS_UNKNOWN;
  }

  // Ratios multiplied by 1000, faster than float
  uint16_t r = ((uint32_t)tcsR * 1000) / sum;
  uint16_t g = ((uint32_t)tcsG * 1000) / sum;
  uint16_t b = ((uint32_t)tcsB * 1000) / sum;

  // WHITE from your sample:
  // R about 443, G about 346, B about 211
  if (tcsC > 700 && r > 360 && r < 520 && g > 250 && g < 430 && b > 140 && b < 300) {
    return TCS_WHITE;
  }

  // ORANGE usually has high red, medium green, low blue
  if (r > 450 && g > 260 && g < 420 && b < 240 && r > g && g > b) {
    return TCS_ORANGE;
  }

  // BLUE usually has strongest blue ratio
  // A blue floor stripe under warm illumination can still have R >= B.
  // Compare against the strong red bias of the measured white floor
  // (white: R~443 G~346 B~211), while rejecting normal white.
  if ((b > 330 && b > r && b > g) ||
      (b >= 285 && (uint32_t)b * 100 > (uint32_t)r * 82 &&
       (uint32_t)b * 100 > (uint32_t)g * 88)) {
    return TCS_BLUE;
  }

  // RED cube or red mark
  if (r > 520 && r > g && r > b) {
    return TCS_RED;
  }

  // GREEN cube or mark
  if (g > 430 && g > r && g > b) {
    return TCS_GREEN;
  }

  return TCS_UNKNOWN;
}

const char* tcs_color_name(TCSColor color) {
  switch (color) {
    case TCS_WHITE: return "WHITE";
    case TCS_BLUE: return "BLUE";
    case TCS_ORANGE: return "ORANGE";
    case TCS_RED: return "RED";
    case TCS_GREEN: return "GREEN";
    case TCS_UNKNOWN: return "UNKNOWN";
    default: return "NONE";
  }
}

#define AVOIDANCE_ANGLE 50  // how far to pivot around the cube
int delayTurns = 1500;      // 1500 - Cubes ; 700 - Open

void flush_messages() {
  if (activeMode == MODE_OPEN) { if (turn_direction == 0) open_color_poll(); }
  else if (activeMode == MODE_FINAL) final_color_poll();
  while (Serial0.available() > 0) {
    char receivedChar = Serial0.read();
    if (receivedChar == '\n') {
      receivedMessage = "";
    } else {
      receivedMessage += receivedChar;
    }
  }
}


// -----------------------------------------------------------
//                Cube Following / Avoidance Functions
// -----------------------------------------------------------
void pass_cube(char cube_direction) {
  double angle_addition = (cube_direction == 'R') ? 0 : 0;

  read_gyro_data();
  // translate 'R' → +1, 'L' → -1
  cube_last = (cube_direction == 'R') ? 1 : -1;

  double start_angle = gz;
  move_until_angle(robot_speed, start_angle - cube_last * (AVOIDANCE_ANGLE + angle_addition));
  move_cm_gyro(12, robot_speed, start_angle - cube_last * (AVOIDANCE_ANGLE + angle_addition));

  last_cube_time = millis();
  currentState = AFTER_CUBE;
  flush_messages();
}

// -----------------------------------------------------------
//                           Functions
// -----------------------------------------------------------
void custom_delay(long long delay_time) {
  long long start_time = millis();
  while (millis() - start_time < delay_time) {
    flush_messages();
  }
}

double map_double(double x, double in_min, double in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void blink_led(int led_pin, int time) {
  digitalWrite(led_pin, HIGH);
  custom_delay(time);
  digitalWrite(led_pin, LOW);
  custom_delay(time);
  digitalWrite(led_pin, LOW);
  custom_delay(time);
  digitalWrite(led_pin, LOW);
  custom_delay(time);
  digitalWrite(led_pin, LOW);
  custom_delay(time);
}


void print_tcs_debug() {
  Serial.print("TCS C="); Serial.print(tcsC);
  Serial.print(" R="); Serial.print(tcsR);
  Serial.print(" G="); Serial.print(tcsG);
  Serial.print(" B="); Serial.print(tcsB);
  Serial.print(" COLOR="); Serial.println(tcs_color_name(tcsCurrentColor));
}
