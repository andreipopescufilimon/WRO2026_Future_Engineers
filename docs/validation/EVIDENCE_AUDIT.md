# V4.1 documentation evidence audit

**Evidence scope:** this is an automated inspection of the supplied files and CSV records, not a competition judge score or physical-robot test. It does not turn calculations, pre-filled templates, simulated rows or source-code checks into field validation.

| Check | Result | What was inspected | Evidence path |
|---|---|---|---|
| Six engineering journal chapters | **PASS** | Six WRO chapters are present | `docs/` |
| Minimum README length | **PASS** | 16,844 Unicode characters, minimum 5,000 | `README.md` |
| Mechanical CAD and PCB packages | **PASS** | 8 STEP files, 3 Gerber ZIPs | `3D-models/; electrical-schematics/` |
| V4.1 firmware tabs | **PASS** | 8 tabs present, including Telemetry.ino | `src/HLR_FE_World_Final/` |
| Area-based current camera source | **PASS** | Current area threshold and current branching are present | `src/camera.py` |
| Height-based Surprise Rule in uploaded camera | **MISSING** | The 50/100/150 mm rule and bottom-position close condition are not in the uploaded camera program | `src/camera.py` |
| Recovered v11 candidate camera pure logic | **PASS** | 23/23 candidate pure-function checks pass; no camera hardware or 2026 field run certified | `src/camera_candidates/rt1062_surprise_rule_v11.py` |
| WRO 2026 ToF active configuration | **PASS** | Configured 4×4 zones and nominal 30 Hz, not 8×8/60 Hz | `src/HLR_FE_World_Final/tof.ino` |
| Source-hash availability | **PASS** | SHA-256 for camera: 72598c701cd8b184a686060465d356451bef1a838d551e4b0cdc9191bb3d5a8a | `src/camera.py` |
| Local markdown links | **PASS** | 503 local links checked; 0 unresolved | `docs/` |
| Documented physical subsystem observations | **PENDING** | 0/12 checks have date, 40-character SHA, measured observation, pass/fail status and evidence | `docs/validation/physical/functional_checks.csv` |
| Arduino compilation: team report vs. retained evidence | **TEAM-REPORTED; LOG PENDING** | Team confirms successful local Arduino compilation on 17 November 2026. This ZIP has 0 captured compiler output records with actual exit code, version and source SHA. Lack of a saved log does **not** imply compilation failure. | `docs/validation/physical/build_verification.csv`; `docs/validation/TEAM_REPORTED_COMPILATION.md` |
| Firmware-linked full-field runs | **PENDING** | 0 runs with date, flashed SHA, actual result and video link | `docs/validation/physical/challenge_runs.csv` |
| Measured power_runs.csv | **PENDING** | 0 dated physical measurements with source SHA and required numeric fields | `docs/validation/physical/power_runs.csv` |
| Measured tof_samples.csv | **PENDING** | 0 dated physical measurements with source SHA and required numeric fields | `docs/validation/physical/tof_samples.csv` |
| Measured imu_samples.csv | **PENDING** | 0 dated physical measurements with source SHA and required numeric fields | `docs/validation/physical/imu_samples.csv` |
| Measured camera_trials.csv | **PENDING** | 0 dated physical measurements with source SHA and required numeric fields | `docs/validation/physical/camera_trials.csv` |
| Companion subsystem ledger | **PASS** | Physical ledger present; check ID reconciliation requires real test outcomes | `docs/validation/physical/subsystem_test_record.csv` |
| Historical Git commit verification | **PENDING** | No .git directory in the supplied ZIP | `GitHub history` |

**Summary:** 10 checks pass; 9 findings need attention. This is **not** a score out of 30.

## Interpreting this report

- PASS means the stated **file or reproducible source-level condition** was observed. A file-presence PASS cannot substitute for a motor test.
- PENDING means a real compiler, hardware, camera or field observation is not available with traceable evidence.
- MISSING means an advertised source feature was absent. Do not claim that feature was tested.
- Source-only checks by themselves do not establish board compilation or camera execution. In addition, the team reports a successful **local Arduino** build. That report is not the same as a retained log or an OpenMV-on-device test.
- Retain actual instrument logs, failed trials, and the exact flashed SHA before making an engineering reliability claim.

See [judging evidence map](../07-judging-evidence.md), [implementation gaps](IMPLEMENTATION_GAPS.md), and [physical verification directory](physical/README.md).
