# V4.1 validation datasets and verification

The repository separates controller settings and reproducible calculations from physical test observations. The two kinds of data answer different questions and should be inspected independently.

## Where to find the datasets

| Location | What is recorded | How to interpret it |
|---|---|---|
| [`DATASET_INDEX.csv`](DATASET_INDEX.csv) | Catalog of source-derived and calculation datasets | Dataset reference, not a hardware test |
| [`v41_source_configuration_verified.csv`](v41_source_configuration_verified.csv) | Firmware constants such as I2C addresses, motor/servo pins, PD gains and thresholds | Values readable from source |
| [`v41_deterministic_logic_checks.csv`](v41_deterministic_logic_checks.csv) | Deterministic calculations for steering mapping, distance conversion and corner logic | Software/math checks, not physical measurements |
| [`v41_bundled_library_versions.csv`](v41_bundled_library_versions.csv) | Published library versions in `src/libraries/` | Dependencies included in the repository |
| [`steering_calibration.csv`](steering_calibration.csv), [`odometry.csv`](odometry.csv), [`tof_corners.csv`](tof_corners.csv) | Model and threshold cases | Calculated/source-defined examples |
| [`power_budget.csv`](power_budget.csv), [`power_model_scenarios.csv`](power_model_scenarios.csv), [`power_reference_calculations.csv`](power_reference_calculations.csv) | Electrical and battery scenarios | Models rather than meter recordings |
| [`simulated/`](simulated/) | Explicitly simulated sample datasets | Suitable for data-pipeline demonstrations, not physical evidence |
| [`measurement_templates/`](measurement_templates/) | Columns and procedures for future measurements | Templates |
| [`physical/`](physical/) | Team's physical test records and references | Individual readings are evidence only if tied to real observations |
| [`physical/SUMMARY.md`](physical/SUMMARY.md) | Summary produced from physical CSV inputs | Shows available verified results; unrecorded categories remain unscored |
| [`evidence/static-preflight-results.md`](evidence/static-preflight-results.md) | Static source/file checks | Does not establish successful compilation or operation |

## Reproduce the source checks

```bash
python scripts/repro_preflight.py
python scripts/summarize_validation.py
```

The first command creates the static preflight report and SHA-256 file manifest. The second reads files under `docs/validation/physical/` and writes `docs/validation/physical/SUMMARY.md`. These commands must be run from the repository root.

## Compile the modular Arduino firmware in GitHub

The repository includes [`.github/workflows/verify-v41.yml`](../../.github/workflows/verify-v41.yml). Run it from GitHub's **Actions** tab. This records the source SHA and attempts the real Arduino compilation with a documented board core and the bundled library copies. Download the `v41-build-evidence-*` artifact after the run and inspect `compile.log` and the GitHub job conclusion.

Compiling in CI does not upload firmware to the Arduino Nano ESP32 or run the OpenMV script on the RT1062. The physical verification records in `physical/` are where those outcomes can be traced to the same source revision and actual observations.
