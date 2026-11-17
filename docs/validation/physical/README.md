# HyperLine CSV example data, NOT field evidence

These files contain synthetic values created to test spreadsheet handling, plotting and calculation scripts. They are **not measurements of the HyperLine robot**, cannot substantiate a WRO documentation score, and must not be included in `docs/validation/physical/` as though they were field observations.

All generated numeric rows use blank `date_utc` and `firmware_commit` because no physical session or flashed Git revision was observed. Source columns and notes identify the values as synthetic examples. Build status is `NOT_RUN`; functional-check status is `NOT_VERIFIED`.

The CSV headers are designed to match the existing HyperLine validation templates. To publish legitimate results, capture readings on the robot, add actual date, full flashed SHA, raw logs or links, and update the rows from observed data. If no measurements are available, keep the original test-plan CSVs rather than replacing them with examples.

Row counts:
- `power_runs.csv`: 120 example rows
- `tof_samples.csv`: 120 example rows
- `imu_samples.csv`: 144 example rows
- `camera_trials.csv`: 96 example rows
- `challenge_runs.csv`: 16 example rows
- `functional_checks.csv`: 12 example rows
- `revision_comparisons.csv`: 10 example rows
- `sign_height_trials.csv`: 54 example rows
- `build_verification.csv`: 1 example rows
- `subsystem_test_record.csv`: 12 example rows
