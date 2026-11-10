void final_configure() {
  max_turns = 13;
  robot_speed = 100;
  kp = 0.044;
  ki = 0;
  kd = 0.082;
  delayTurns = 1500;
}

void final_exit_parking() {
  // Start from parking
    if (readDistanceMM(LEFT_DIR) < readDistanceMM(RIGHT_DIR)) {  // exit right/.
      move_until_angle_max(100, 75);

      move_cm_gyro(25, 100, 75);

      turn_direction = 1;
      move_until_angle_max(exit_speed, 0);
      steeringServo.write(STEERING_CENTER);

      move_cm_gyro(35, -100, current_angle_gyro);
      //move_straight_on_gyro(-robot_speed, 1100);
      current_angle_gyro = 0.0;
    } else {  // exit left
      move_until_angle_max(100, -70);

      move_cm_gyro(25, 100, -70);

      turn_direction = -1;

      move_until_angle_max(exit_speed, 0);
      steeringServo.write(STEERING_CENTER);

      move_cm_gyro(60, -100, current_angle_gyro);
      //move_straight_on_gyro(-robot_speed, 1900);
      current_angle_gyro = -0.0;
    }

}

void final_execute() {
  final_color_poll();
  read_gyro_data();
  tof_poll();
  print_tcs_debug();
  if (turn_count >= max_turns) currentState = PARK;
  switch (currentState) {
    case PID: {
      double err = current_angle_gyro - gz;
      pid_error = (err)*kp + (err - pid_last_error) * kd;
      pid_last_error = err;
      int commandSpeed = robot_speed;
      double steeringCommand = pid_error;
      steer(steeringCommand);
      move(commandSpeed);
      break;
    }
    case FOLLOW_CUBE: {
      if (millis() - last_follow_cube > FOLLOW_CUBE_DEAD_TIME) currentState = PID;
      steer(follow_cube_angle);
      move(robot_speed);
      break;
    }
    case AVOID_CUBE: {
      pass_cube(cube_avoid_direction);
      break;
    }
    case AFTER_CUBE: {
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
      break;
    }
    case PARK: {
      // Parking action is performed on the 13th marker in final_execute_command.
      break;
    }
  }
  while (Serial0.available() > 0) {
    char c = Serial0.read();
    if (c == '\n') {
      receivedMessage.trim();
      receivedMessage.toUpperCase();
      final_execute_command(receivedMessage);
      receivedMessage = "";
      flush_messages();
    } else receivedMessage += c;
  }
}

void final_color_poll() {
  tcs_update_fast();
  if (!tcsExecutingMarker) tcs_process_marker();
}

bool tcs_is_expected_marker_detected() {
  // Unknown direction: accept first BLUE or ORANGE
  if (turn_direction == 0) {
    return tcsCurrentColor == TCS_BLUE || tcsCurrentColor == TCS_RED;
  }

  // Direction already known
  if (turn_direction == -1) {
    return tcsCurrentColor == TCS_BLUE;
  }

  if (turn_direction == 1) {
    return tcsCurrentColor == TCS_RED;
  }

  return false;
}


void tcs_update_fast() {
  if (!tcs_read_raw_fast()) {
    return;
  }

  TCSColor color = tcs_detect_color_fast();
  tcsCurrentColor = color;

  if (color == TCS_WHITE) {
    tcsLastWhiteMs = millis();

    if (millis() - tcsLastMarkerMs > TCS_REARM_WHITE_MS) {
      tcsMarkerArmed = true;
    }
  }

  // Unknown direction: first valid marker is allowed to latch
  if (turn_direction == 0) {
    if (color == TCS_BLUE || color == TCS_RED || color == TCS_ORANGE) {
      tcsLatchedColor = color;
    }
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

  if (tcsLatchedColor == TCS_NONE) {
    return;
  }

  // If turn_direction is unknown, first BLUE/ORANGE marker decides it
  if (turn_direction == 0) {
    if (tcsLatchedColor == TCS_BLUE) {
      turn_direction = -1;
    }
    else if (tcsLatchedColor == TCS_RED || tcsLatchedColor == TCS_ORANGE) {
      turn_direction = 1;
    }
    else {
      tcsLatchedColor = TCS_NONE;
      return;
    }

  }

  // After direction is known, reject wrong marker
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


void final_park() {
        move(0);
        steeringServo.write(STEERING_CENTER);
        delay(200);

        kp = 0.087;
        kd = 0.130;

        if (turn_direction == -1) {
          move_straight_on_gyro_no_flush(robot_speed, 230);
          move_until_angle_no_flush(park_speed, (current_angle_gyro) + 90);
          current_angle_gyro = current_angle_gyro + 90;
          move_cm_gyro(9, 45, current_angle_gyro);
          move_cm_gyro(9, -45, current_angle_gyro);
          move_cm_gyro(9, 45, current_angle_gyro);
          move_to_front_distance(275.0f, 5.0f, 45, current_angle_gyro, false);
          move_until_angle_no_flush(47, (current_angle_gyro) + 88);

          current_angle_gyro = current_angle_gyro + 88;
          move_cm_gyro(5, 37, current_angle_gyro);
          move_straight_until_distance('L', 37, 175, false, false);
          move_cm_gyro(10, 30, current_angle_gyro);
          move_straight_until_distance('L', 30, 175, false, true);
          move_cm_gyro(4, 30, current_angle_gyro);

          move_until_angle_max_no_flush(-49, (current_angle_gyro) + 75);
          move_until_angle_max_no_flush(-42, (current_angle_gyro) + 2);
        } else {
          move_straight_on_gyro_no_flush(robot_speed, 250);
          move_until_angle_no_flush(park_speed, (current_angle_gyro)-90);
          current_angle_gyro = current_angle_gyro - 90;
          move_cm_gyro(7, 45, current_angle_gyro);
          move_cm_gyro(9, -45, current_angle_gyro);
          move_cm_gyro(7, 45, current_angle_gyro);
          move_to_front_distance(280.0f, 5.0f, 45, current_angle_gyro, false);
          move_until_angle_no_flush(47, (current_angle_gyro)-91.5);

          current_angle_gyro = current_angle_gyro - 91.5;
          move_cm_gyro(5, 37, current_angle_gyro);
          move_straight_until_distance('R', 32, 175, false, false);
          move_cm_gyro(10, 30, current_angle_gyro);
          move_straight_until_distance('R', 30, 175, false, true);
          move_cm_gyro(4, 29, current_angle_gyro);

          move_until_angle_max_no_flush(-49, (current_angle_gyro)-75);
          move_until_angle_max_no_flush(-45, (current_angle_gyro)-2);
        }

        move_straight_on_gyro_no_flush(robot_speed, 80);
        steeringServo.write(STEERING_CENTER);
        move(0);
        delay(20000);
}

void final_execute_command(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  //––– 1) Special “S…” steering override for RUN_MODE=1 –––––––––––––––
  if (cmd.startsWith("S")) {
    desiredSteering = cmd.substring(1).toFloat();
    currentState = FOLLOW_CUBE;

    float val = desiredSteering;
    follow_cube_angle = val;
    last_follow_cube = millis();

    if (debug) {
      Serial.print("FOLLOW_CUBE angle → ");
      Serial.print(follow_cube_angle);
      Serial.print("      Steer-override → ");
      Serial.println(desiredSteering);
    }
    return;
  }

  //––– 2) Map color words to single-char code, set turn_direction on first BLUE/ORANGE –––
  char c = cmd.charAt(0);
  if (cmd.indexOf("RED") != -1) {
    c = 'R';
  } else if (cmd.indexOf("GREEN") != -1) {
    c = 'G';
  }

  //––– 3) BLACK-line detection → 90° turn in PID––––––––––––––
  if (tcs_is_expected_marker_detected()) {
    // Open round relies on the ToF clear/rearm state instead of a time lockout.
    // Keep the original marker lockout for cube mode only.
    if (millis() - lastTurnTime > delayTurns) {
      turn_count++;
      if (debug) Serial.println("Received 'BLACK' command. Turning 90°...");
      if (turn_count == 13) {
        final_park();
      } else {
        if (abs(current_angle_gyro - gz) < 10 && currentState != AFTER_CUBE) {
          move_cm_gyro(13, robot_speed, current_angle_gyro);  // 15
        } else if (currentState == AFTER_CUBE) {
          if (cube_last == turn_direction) {
            move_until_angle(robot_speed, current_angle_gyro);  // *30
          } else {
            move_until_angle(robot_speed, current_angle_gyro + turn_direction * 5);
            move_cm_gyro(8, robot_speed, current_angle_gyro + turn_direction * 5);
          }
          currentState = PID;
        }
      }

      current_angle_gyro += turn_direction * 90;

      //move_until_angle_max(robot_speed, current_angle_gyro - 10);
      //move_straight_on_gyro(-robot_speed, 1100);

      delayTurns = 1500;  // 700 - Open ; 1500 - Cubes
      lastTurnTime = millis();
      lastLineDetectedTime = 0;
    }
  }


  //––– 4) Cube-avoidance: see a cube up close (R/G) –––––––––––––––––––––
  if ((c == 'R' || c == 'G')) {
    // R means avoid left, G avoid right
    cube_avoid_direction = (c == 'R' ? 'L' : 'R');
    currentState = AVOID_CUBE;
    last_cube_time = millis();
    if (debug) {
      Serial.print("AVOID_CUBE dir → ");
      Serial.println(cube_avoid_direction);
    }
    return;
  }

  //––– 5) All other incoming commands (P, O, L, etc.)—stay in your current state or ignore –––
  if (debugcam) {
    Serial.print("Ignored cmd → ");
    Serial.println(cmd);
  }
}