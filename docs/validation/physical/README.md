# V4.1 physical validation dataset

These seven CSVs are intentionally header-only until **real readings from the assembled robot** are recorded. They are not mock examples or calculated predictions. Each row should include its source revision, date and test ID.

| File | Capture method | Derived results |
|---|---|---|
| `power_runs.csv` | Meter/shunt at battery input; oscilloscope or logger on 5 V bus; record time series during idle, propulsion, steering and combined loads | Mean and peak battery current, minimum 5 V and pack energy |
| `tof_samples.csv` | At 200, 400, 600 and 1000 mm, use a flat target and record each frame for Front/Left/Right/Back; retain invalid frames | Mean bias, RMSE across **individual** frames, invalid counts and measured frame rate |
| `imu_samples.csv` | Record 60 s stationary (motor off and on) plus 12 measured 90° turns, using a reference angle | Bias, drift (°/min) and mean turn error |
| `camera_trials.csv` | Place red and green pillars at multiple distances and lighting conditions; preserve every trial | Detection accuracy, misses, wrong colours, mean processing time |
| `challenge_runs.csv` | Open and Final field runs under repeatable conditions, including failures and evidence links | Count of documented completed runs |
| `functional_checks.csv` | One record for each real bench verification, including evidence and PASS/FAIL | Auditable subsystem check ledger |
| `revision_comparisons.csv` | Matched V4 and V4.1 trials using identical test geometry and metric definitions | Paired before/after difference, not an inferred win |

After collecting data, from the repository root run:

```bash
python scripts/summarize_validation.py
```

The generated [`SUMMARY.md`](SUMMARY.md) is the numerical record referenced by Chapters 03 and 06. Do not change trial outcomes after analysis; preserve raw input and photos/videos. Use a calibrated instrument or describe its accuracy and bandwidth, particularly for sub-millisecond voltage dips and motor inrush. **A README, a firmware test case or an image is not a physical test result.**

For plots of real power waveforms, ToF errors, stationary gyro drift and camera classifications, install Matplotlib and run:

```bash
python -m pip install matplotlib
python scripts/plot_physical.py
```

Plots are created **only** when sufficient recorded data exists. They are not generated from the deterministic algorithm scenarios and must not be presented as experiment results without associated raw CSVs.

## Firmware-specific subsystem verification

Use [SUBSYSTEM_VERIFICATION.md](SUBSYSTEM_VERIFICATION.md) and the populated test ledger [functional_checks.csv](functional_checks.csv) to record the real V4.1 motor, steering, ToF, gyro, UART and encoder bench checks. All test statuses start as `NOT_RUN`; source-defined behavior is documented separately in [source_configuration.csv](source_configuration.csv). Project demonstrations and team-identified filming source are indexed in [published_demonstrations.csv](published_demonstrations.csv). Do not interpret the presence of a row as a completed test.
