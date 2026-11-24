# V4.1 field and bench records

[Recorded observations](SUMMARY.md) · [Subsystem test procedure](SUBSYSTEM_VERIFICATION.md) · [Controller configuration](source_configuration.csv)

The physical ledgers collect the original measurements, run results and test conditions. A test is recorded with its date, actual flashed revision and evidence. Range-matrix observations extracted from a diagnostic capture are stored separately from ruler-reference accuracy trials. Analytical and simulated examples are stored outside this directory.

| Record | File |
|---|---|
| Electrical load and regulator measurements | [power_runs.csv](power_runs.csv) |
| Reference distance and timing | [tof_samples.csv](tof_samples.csv) |
| Actual 8×8 diagnostic frame | [tof_diagnostic_frame_633.csv](tof_diagnostic_frame_633.csv) |
| IMU heading and angular rate | [imu_samples.csv](imu_samples.csv) |
| Camera color and height | [camera_trials.csv](camera_trials.csv), [sign_height_trials.csv](sign_height_trials.csv) |
| Autonomous runs and parking | [challenge_runs.csv](challenge_runs.csv) |
| Development comparison | [revision_comparisons.csv](revision_comparisons.csv) |
| Hardware checks and build environment | [functional_checks.csv](functional_checks.csv), [build_verification.csv](build_verification.csv) |
| Published driving and assembly recordings | [published_demonstrations.csv](published_demonstrations.csv) |

Run `python scripts/summarize_validation.py --output docs/validation/physical/MEASUREMENTS.md` to aggregate dated numerical measurements after recording them.
