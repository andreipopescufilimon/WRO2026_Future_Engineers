# V4.1 physical measurement records

This directory is for **genuine observations captured from the assembled robot**.

- `functional_checks.csv` retains the team's twelve previously listed checks. The original status `RUNNED AND CHECKED` is preserved, but the record has no individual measurement, date, exact flashed commit, or link yet. This is not a verified `PASS`.
- `source_configuration.csv` documents values checked in the firmware source. Those values are not measured hardware outputs.
- `published_demonstrations.csv` indexes real published footage. The filming firmware at `de34faf` differs from the later modular firmware.
- `power_runs.csv`, `tof_samples.csv`, `imu_samples.csv`, `camera_trials.csv`, `challenge_runs.csv` and `revision_comparisons.csv` are intentionally header-only, ready for measurements.
- `build_verification.csv` stores the actual build environment and compiler result once a build is performed.
- `sign_height_trials.csv` is for captured camera classification trials, not generated mockups.

Run real subsystem checks with `python tools/record_subsystem_checks.py --list` and then `python tools/record_subsystem_checks.py --test V41-MOTOR-FWD`. For numerical sensors, add measured samples to the matching CSV using the original headers.

The filled numerical CSVs in `../simulated/` are examples, **not measured V4.1 results**. Do not copy them here or run `scripts/summarize_validation.py` on those example files as though they were field evidence.
