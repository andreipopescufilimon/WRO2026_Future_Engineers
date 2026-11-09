# V4.1 code-derived CSV reference records

These eight populated CSVs contain **software configurations, numerical predictions, and deterministic test vectors derived from code**. The rows are **not real experiments** and must not be presented as completed robot test runs. Records are explicitly classified as `CODE_CONFIG`, `CODIFIED_SEQUENCE`, or `DETERMINISTIC_SCENARIO` in the `record_type` field. No row is `OBSERVED`.

The **original measurement columns are retained** and physical measurements are left blank, including trial dates, actual firmware commit IDs, voltages, currents, servo-wheel angles, measured turning radii, ruler distances, measured gyro bias, live camera frame results, recorded ToF zones and full-run scores. Calculated outputs are placed in new, clearly labeled columns such as `expected_speed_pwm` and `minimum_abs_ticks_for_stop`.

| File | Rows | What the additional columns contain |
| --- | ---: | --- |
| `full_runs.csv` | 2 | programmed Open and Final round configuration and corner targets |
| `parking.csv` | 4 | commanded encoder-only move segments and ToF approach targets |
| `odometry.csv` | 13 | computed tick thresholds using 1.831 mm/tick |
| `steering_calibration.csv` | 10 | servo commands produced by normalized steering mapping |
| `gyro_test.csv` | 9 | gyro initialization and scripted relative/absolute yaw targets |
| `tof_corners.csv` | 13 | boundary cases for 600/800 mm thresholds, 150 ms freshness and 40% speed reduction |
| `power_budget.csv` | 11 | 8-bit 1 kHz motor PWM settings and nominal duty percentages, not electrical loads |
| `camera_tests.csv` | 8 | UART expectations for controlled blob scenarios for one camera script variant |

## How to use them

- Filter `record_type` to distinguish code-derived rows from physical tests. **Never include these records in success-rate, sensor-accuracy or experimental error statistics.**
- The `measurement_templates/` subfolder contains the original clean headers. Duplicate them when collecting real robot measurements, then fill `date_utc`, `firmware_commit`, and observational columns from the actual tests.
- If you mix future physical records into the populated CSVs, use `record_type=OBSERVED`, keep their original raw inputs, and label the firmware commit and setup for each observation.
- In `odometry.csv`, **48 QUAD_EDGES is declared but the ISR counts only rising edges of channel A**. `MM_PER_TICK=1.831` is the software conversion constant and must not be replaced with a purely theoretical wheel-circumference calculation without encoder placement/ratio verification.
- In `steering_calibration.csv`, servo angles are command arguments, **not physical front-wheel angles**. The mechanism is parallel steering. Turning radii require measured physical steering angles or turn tests.
- In `parking.csv`, encoder-distance sums exclude timed moves, gyro turns, and ToF-targeted driving. They are not total driven distance or measured parking trajectory.
- In `power_budget.csv`, PWM duty is not battery current, motor current, or a measured voltage. These fields must be measured under load.
- In `tof_corners.csv`, input distances are synthetic mathematical test vectors; the original `reported_distance_mm`, `reference_distance_mm`, and `frame_age_ms` observational columns remain empty.
- **Camera-script ambiguity:** The camera test vectors use a shared `camera(1).py` variant with `CLOSE_BOTTOM_Y=148`, `CLOSE_CONFIRM_FRAMES=1`, `kp_cube=0.80`, `kd_cube=3.20` and UART at 19,200 baud. That script is labeled RT1062 in its comments. The actual N6 deployment has **not** been verified against this variant. Do not treat these vectors as N6 calibration results or camera test results.

## Main source code used

- `HLR_FE_Separated.ino`
- `Open.ino`
- `Final.ino`
- `motors.ino`
- `gyro.ino`
- `tof.ino`
- `functions.ino`
- `camera(1).py` (unverified deployed variant, used for camera algorithm examples only)

All eight CSV files use comma delimiters and retain the original measurement-template headers as their leading columns. Missing numeric observations are stored as empty cells, not zero.

## Figures embedded in the engineering chapters

The current code-derived plots are available directly in the respective Markdown chapters and as PNG/SVG pairs in `docs/docs-media/`:

| Dataset | Chapter | Generated illustration |
|---|---|---|
| [steering_calibration.csv](steering_calibration.csv) | [02 Mobility](../02-mobility.md) | [Servo command map](../docs-media/data-steering-command-map.png) |
| [odometry.csv](odometry.csv) | [02 Mobility](../02-mobility.md) | [Command to tick thresholds](../docs-media/data-odometry-tick-thresholds.png) |
| [power_budget.csv](power_budget.csv) | [03 Power and sensors](../03-power-sensors.md) | [Nominal PWM duty settings](../docs-media/data-pwm-duty-commands.png) |
| [tof_corners.csv](tof_corners.csv) | [03 Power and sensors](../03-power-sensors.md) | [Synthetic ToF boundary cases](../docs-media/data-tof-threshold-cases.png) |
| [gyro_test.csv](gyro_test.csv) | [04 Software](../04-software.md) | [Scripted yaw commands](../docs-media/data-gyro-commanded-yaw.png) |
| [parking.csv](parking.csv) | [04 Software](../04-software.md) | [Encoder-distance commands](../docs-media/data-parking-distance-commands.png) |
| [camera_tests.csv](camera_tests.csv) | [04 Software](../04-software.md) | Embedded deterministic scenario table |
| [full_runs.csv](full_runs.csv) | [04 Software](../04-software.md) and [05 Systems](../05-systems.md) | Embedded mode configuration table |

To recreate all six figures after changing the code-derived inputs, use `python docs/validation/generate_charts.py` at the repository root. This requires Python with NumPy and matplotlib. Real observed run results require a separate analysis that validates the measurement conditions and firmware revision; these code-generated figures are not substitutes for measured performance.
