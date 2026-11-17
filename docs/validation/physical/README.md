# Physical verification records | V4.1

This directory is reserved for **actual observations from the assembled physical robot**. It contains prepared CSV headers and test-plan rows, but the supplied archive does not provide measured current/ToF/IMU/camera/run datasets. Empty measurement cells must remain empty until the test is performed.

`functional_checks.csv` formerly contained an undocumented `RUNNED AND CHECKED` label without dates, observations, commits or primary evidence. Those cases are now `REPORTED_NOT_VERIFIED`; the original wording is preserved in the notes. The companion `subsystem_test_record.csv` also correctly records unverified tests. A claim is counted as physically verified only with a **dated** observed result, full flashed source SHA and a direct evidence file or link.

| File | What to enter after a real test |
|---|---|
| `power_runs.csv` | timestamp, rail voltages, battery current, operating state, instrument and source |
| `tof_samples.csv` | independently measured target distance, sensor/zone reading and timestamp, valid/invalid |
| `imu_samples.csv` | stationary drift or independently referenced turn and observed gyro values |
| `camera_trials.csv`, `sign_height_trials.csv` | actual frame, observed classification, UART command, known target/distance/lighting |
| `challenge_runs.csv` | field layout, direction, laps, outcome, time and full video link |
| `functional_checks.csv`, `subsystem_test_record.csv` | one consistent recorded hardware check per ID |
| `build_verification.csv` | actual compiler command, version, exit code, log and source SHA |
| `revision_comparisons.csv` | paired physical design comparisons under comparable conditions |

## Suggested evidence collection order

1. Save an exact Git commit **after** uploading the source to GitHub, and flash that commit to your controller. A ZIP without `.git` has no trustworthy Git SHA by itself.
2. Use `python tools/record_subsystem_checks.py --list` and then `--test V41-MOTOR-FWD` (repeat the ID for each physical check); the tool asks for observed values and real evidence, and updates both subsystem ledgers.
3. Record instrument exports to the CSV templates. A script that manufactures plausible observations is **not** a substitute.
4. Generate the summary with `python scripts/summarize_validation.py`; inspect the input counts in the generated report.
5. Run `python tools/documentation_audit.py` and review `docs/validation/EVIDENCE_AUDIT.md`.

Published source parameters and model calculations reside in other datasets. Simulated inputs reside only in `../simulated/` or `../../docs-media/validation/simulated/`, and may not be combined with this folder to imply real performance.
