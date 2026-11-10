# V4.1 SIMULATED validation summary (NOT PHYSICAL TEST DATA)

All values below are computer-generated demonstration data with no measurement instrument, physical run or tested firmware behind them. They are not evidence of actual robot performance.

## Electrical load and 5 V stability

| Test / state | Samples | Mean pack current | Peak pack current | Minimum 5 V rail | Recorded pack energy |
|---|---:|---:|---:|---:|---:|
| SIM_PWR_01 / start_up | 241 | 0.395 A | 0.585 A | 4.994 V | 0.0217 Wh |
| SIM_PWR_02 / idle_sensors | 241 | 0.364 A | 0.495 A | 4.987 V | 0.0199 Wh |
| SIM_PWR_03 / straight_drive | 241 | 1.102 A | 1.562 A | 4.983 V | 0.0584 Wh |
| SIM_PWR_04 / alternating_steering | 241 | 0.820 A | 1.040 A | 4.962 V | 0.0440 Wh |
| SIM_PWR_05 / camera_and_four_ToFs | 241 | 0.500 A | 0.669 A | 4.975 V | 0.0273 Wh |
| SIM_PWR_06 / combined_motion_vision | 241 | 1.589 A | 1.955 A | 4.945 V | 0.0823 Wh |

## Distance sensor accuracy

Valid ToF samples are evaluated individually against ruler distance. Invalid samples are counted separately; zone frame-rate estimates require timestamps.

| Sensor | Samples | Valid | Invalid | Mean bias | RMSE | Frame rate |
|---|---:|---:|---:|---:|---:|---:|
| back | 320 | 317 | 3 | -1.87 mm | 11.52 mm | 30.0 Hz |
| front | 320 | 316 | 4 | 2.26 mm | 10.37 mm | 30.0 Hz |
| left | 320 | 317 | 3 | -2.28 mm | 10.32 mm | 30.0 Hz |
| right | 320 | 319 | 1 | 2.94 mm | 10.65 mm | 30.0 Hz |

## BMI088 drift and heading accuracy

| Test / condition | Samples | Mean gyro Z rate | Heading drift | Mean absolute turn error |
|---|---:|---:|---:|---:|
| SIM_IMU_STATIONARY_OFF / stationary | 301 | 0.007 °/s | 0.43 °/min | not available |
| SIM_IMU_STATIONARY_ON / stationary | 301 | 0.026 °/s | 1.57 °/min | not available |
| SIM_IMU_TURN_01 / turn_90_deg | 21 | 41.850 °/s | not available | 1.99° |
| SIM_IMU_TURN_02 / turn_90_deg | 21 | -43.677 °/s | not available | 2.10° |
| SIM_IMU_TURN_03 / turn_90_deg | 21 | 42.055 °/s | not available | 1.33° |
| SIM_IMU_TURN_04 / turn_90_deg | 21 | -43.715 °/s | not available | 1.79° |
| SIM_IMU_TURN_05 / turn_90_deg | 21 | 43.580 °/s | not available | 1.88° |
| SIM_IMU_TURN_06 / turn_90_deg | 21 | -42.548 °/s | not available | 0.30° |
| SIM_IMU_TURN_07 / turn_90_deg | 21 | 41.352 °/s | not available | 2.75° |
| SIM_IMU_TURN_08 / turn_90_deg | 21 | -43.097 °/s | not available | 0.86° |
| SIM_IMU_TURN_09 / turn_90_deg | 21 | 42.228 °/s | not available | 1.33° |
| SIM_IMU_TURN_10 / turn_90_deg | 21 | -42.704 °/s | not available | 0.13° |
| SIM_IMU_TURN_11 / turn_90_deg | 21 | 41.933 °/s | not available | 1.71° |
| SIM_IMU_TURN_12 / turn_90_deg | 21 | -42.298 °/s | not available | 1.00° |

## OpenMV RT1062 classification

A trial is counted as detected when a red/green classification is reported, and correct only when it matches the reference color.

| Lighting / distance | Trials | Correct | Missed | Wrong colour | Accuracy | Mean processing latency |
|---|---:|---:|---:|---:|---:|---:|
| bright_backlight / 250 mm | 54 | 52 | 1 | 1 | 96.3% | 24.08 ms |
| bright_backlight / 500 mm | 54 | 52 | 1 | 1 | 96.3% | 24.38 ms |
| bright_backlight / 800 mm | 54 | 50 | 4 | 0 | 92.6% | 24.73 ms |
| dim / 250 mm | 54 | 50 | 3 | 1 | 92.6% | 24.54 ms |
| dim / 500 mm | 54 | 50 | 4 | 0 | 92.6% | 24.59 ms |
| dim / 800 mm | 54 | 49 | 5 | 0 | 90.7% | 24.67 ms |
| normal / 250 mm | 54 | 53 | 0 | 1 | 98.1% | 24.13 ms |
| normal / 500 mm | 54 | 54 | 0 | 0 | 100.0% | 23.59 ms |
| normal / 800 mm | 54 | 53 | 1 | 0 | 98.1% | 24.49 ms |
| startup_facing_wall / 250 mm | 54 | 50 | 3 | 1 | 92.6% | 24.13 ms |
| startup_facing_wall / 500 mm | 54 | 52 | 2 | 0 | 96.3% | 24.35 ms |
| startup_facing_wall / 800 mm | 54 | 50 | 3 | 1 | 92.6% | 24.83 ms |

## Subsystem functional test outcomes

| Test | Subsystem | Result | Observation | Evidence |
|---|---|---|---|---|
| SIM_CHECK_01 | logic state initialization | SIMULATED_PASS | boot sequence progressed |  |
| SIM_CHECK_02 | four ToF address model | SIMULATED_PASS | 0x30 0x31 0x32 0x33 |  |
| SIM_CHECK_03 | stale distance rejection | SIMULATED_PASS | stale frame returned invalid |  |
| SIM_CHECK_04 | front 2-zone corner gate | SIMULATED_PASS | corner flag triggered |  |
| SIM_CHECK_05 | 800mm rearm gate | SIMULATED_PASS | rearmed at 800mm |  |
| SIM_CHECK_06 | PWM mapping | SIMULATED_PASS | 98.04% |  |
| SIM_CHECK_07 | servo command bounds | SIMULATED_PASS | bounded command |  |
| SIM_CHECK_08 | bias integration math | SIMULATED_PASS | small simulated drift |  |
| SIM_CHECK_09 | floor marker lockout | SIMULATED_PASS | no double count |  |
| SIM_CHECK_10 | UART message parsing | SIMULATED_PASS | matching command decoded |  |
| SIM_CHECK_11 | D11 D12 exclusivity | SIMULATED_PASS | safe stop |  |
| SIM_CHECK_12 | invalid range fallback | SIMULATED_PASS | fallback path selected |  |
| SIM_CHECK_13 | state transitions | SIMULATED_PASS | simulated sequence completed |  |
| SIM_CHECK_14 | voltage sag scenario | SIMULATED_PASS | minimum simulated 5V above 4.8V |  |

## Challenge runs and revision comparisons

| Mode | Runs logged | Documented successful runs | Evidence links recorded |
|---|---:|---:|---:|
| obstacle | 12 | 7 | 0 |
| open | 12 | 11 | 0 |

## V4 → V4.1 engineering comparison

| Subsystem | Test method / metric | V4 result | V4.1 result | Assessment |
|---|---|---|---|---|
| camera | classification_accuracy, hypothetical_obstacle_sequence | 88.71 | 92.27 | 3.56 (V4.1−V4) |
| drive | parking_stop_error, hypothetical_parking_250mm | 31.90 | 20.79 | -11.11 (V4.1−V4) |
| drive | startup_delay, hypothetical_same_mass | 512.88 | 313.51 | -199.38 (V4.1−V4) |
| inertial | yaw_turn_absolute_error, hypothetical_90deg_turn | 5.77 | 2.69 | -3.08 (V4.1−V4) |
| wall_sensing | distance_RMSE, hypothetical_wall_600mm | 27.32 | 13.14 | -14.19 (V4.1−V4) |

This report contains only simulated observations. For physical validation, collect and keep genuine measurements and their evidence separately.

