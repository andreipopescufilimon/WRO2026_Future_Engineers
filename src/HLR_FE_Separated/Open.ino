void open_configure() {
  max_turns = 12;
  robot_speed = 250;
  kp = 0.034;
  ki = 0;
  kd = 0.082;
  delayTurns = 1000;
}

void open_color_poll() {
    if (turn_direction != 0) return;
    if (!tcs_read_raw_fast()) return;
    tcsCurrentColor = tcs_detect_color_fast();
    if (tcsCurrentColor == TCS_BLUE) {
      turn_direction = -1;
    } else if (tcsCurrentColor == TCS_ORANGE || tcsCurrentColor == TCS_RED) {
      turn_direction = 1;
    }
    if (turn_direction != 0) {
      tcsLatchedColor = tcsCurrentColor;
      if (debug) {
        Serial.print("Open direction locked (first color sample): ");
        Serial.println(turn_direction == -1 ? "BLUE / LEFT" : "RED-ORANGE / RIGHT");
      }
    }
    return;
}

void open_execute() {
  // First marker sets direction, front ToF starts corners.
  if (turn_direction == 0) open_color_poll();
  read_gyro_data();
  tof_poll();
  robot_speed = (turn_count == 0) ? 80 : 250;
  if (tof_consume_front_turn() && turn_direction != 0 && turn_count < max_turns) {
    open_execute_command("TOF_FRONT");
  }
  print_tcs_debug();
  if (turn_count >= max_turns) {
    if (!openCornerActive) {
      move_straight_on_gyro_no_flush(OPEN_FINISH_SPEED, OPEN_FINISH_DURATION_MS);
      move(0);
      steeringServo.write(STEERING_CENTER);
      if (debug) Serial.println("Maximum turns reached. Stopping...");
      while (true) { delay(100); }
    }
  }
  switch (currentState) {
    case PID: {
  // PID branch from original open-round code, unchanged.
  double err = current_angle_gyro - gz;
  pid_error = (err)*kp + (err - pid_last_error) * kd;
  pid_last_error = err;
  int commandSpeed = robot_speed;
  double steeringCommand = pid_error;
  if (openCornerActive) {
    const double headingError = fabs(err);
    if (headingError <= OPEN_CORNER_DONE_ANGLE_DEG || err * turn_direction <= 0.0) {
      openCornerActive = false;
      pid_error = 0;
      pid_last_error = 0;
      steeringCommand = 0.0;
      commandSpeed = robot_speed;
    } else {
      steeringCommand = constrain(0.055 * err, -1.0, 1.0);
      commandSpeed = OPEN_CORNER_SPEED;
    }
  }
  if (!openCornerActive) commandSpeed = tof_open_round_speed(commandSpeed);
  if (turn_direction == 0 && tof_front_wall_close()) {
    commandSpeed = 0;
    steeringCommand = 0;
  }
  steer(steeringCommand);
  move(commandSpeed);
      break;
    }
    case FOLLOW_CUBE:
      {
        if (millis() - last_follow_cube > FOLLOW_CUBE_DEAD_TIME) {
          currentState = PID;
        }
        // steer toward cube
        steer(follow_cube_angle);
        move(robot_speed);
        break;
      }

    case AVOID_CUBE:
      {
        // avoidance maneuver
        pass_cube(cube_avoid_direction);
        break;
      }

    case AFTER_CUBE:
      {
        int angle_addition = (cube_last == 1) ? 0 : 0;
        double err = current_angle_gyro - gz + cube_last * (CORRECTION_ANGLE + angle_addition);
        if (abs(err) < 10) {
          if (cube_last == turn_direction) {
            move_cm(19, robot_speed, current_angle_gyro + cube_last * (CORRECTION_ANGLE + angle_addition));
          } else {
            move_cm(19, robot_speed, current_angle_gyro + cube_last * (CORRECTION_ANGLE + angle_addition));
          }
          currentState = PID;
        } else {
          pid_error = (err)*kp + (pid_error - pid_last_error) * kd;
          pid_last_error = pid_error;
          steer(pid_error);
        }
        move(robot_speed);
        //flush_messages();
        break;
      }

    case PARK:
      {
        // in functions.ino
        break;
      }
  }
  // Keep the original OpenMV UART dispatch. Only the handler differs.
  while (Serial0.available() > 0) {
    char c = Serial0.read();
    if (c == '\n') {
      receivedMessage.trim();
      receivedMessage.toUpperCase();
      open_execute_command(receivedMessage);
      receivedMessage = "";
      flush_messages();
    } else receivedMessage += c;
  }
}

void open_execute_command(String cmd) {
  cmd.trim();
  cmd.toUpperCase();
  if (cmd == "TOF_FRONT") {
    // Original open-round corner handler; only the event ownership changed.
    turn_count++;
    if (debug) Serial.println("Received 'BLACK' command. Turning 90°...");
    if (abs(current_angle_gyro - gz) < 10 && currentState != AFTER_CUBE) {
      move_cm_gyro(10, OPEN_CORNER_SPEED, current_angle_gyro);
    } else if (currentState == AFTER_CUBE) {
      if (cube_last == turn_direction) {
        move_until_angle(robot_speed, current_angle_gyro);
      } else {
        move_until_angle(robot_speed, current_angle_gyro + turn_direction * 5);
        move_cm_gyro(8, robot_speed, current_angle_gyro + turn_direction * 5);
      }
      currentState = PID;
    }
    current_angle_gyro += turn_direction * 90;
    openCornerActive = true;
    pid_error = 0;
    pid_last_error = 0;
    delayTurns = 1000;
    lastTurnTime = millis();
    lastLineDetectedTime = 0;
  }
  // Preserve original UART color command handling, although it is not
  // involved in normal open-round steering.
  char c = cmd.charAt(0);
  if (cmd.indexOf("RED") != -1) c = 'R';
  else if (cmd.indexOf("GREEN") != -1) c = 'G';
  if (c == 'R' || c == 'G') {
    cube_avoid_direction = (c == 'R' ? 'L' : 'R');
    currentState = AVOID_CUBE;
    last_cube_time = millis();
    if (debug) {
      Serial.print("AVOID_CUBE dir → ");
      Serial.println(cube_avoid_direction);
    }
    return;
  }
  if (debugcam) { Serial.print("Ignored cmd → "); Serial.println(cmd); }
}
