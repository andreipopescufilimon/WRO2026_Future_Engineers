# V4.1 simulated validation CSV package

**SIMULATED DATA ONLY. NOT RECORDED FROM A ROBOT.**

Seven fully populated CSV datasets use the **exact production headers** from the existing `docs/validation/physical/` templates. The data let you try the numerical summary, graphs and software pipeline and inspect example values without supplying fake physical evidence.

All rows carry an explicit simulation marker in the `notes` column; test IDs begin `SIM_`, firmware commits are `SIMULATED_NO_REAL_FIRMWARE_COMMIT`, and no instrument or video evidence is claimed. Timestamps represent an artificial dataset clock, not dates when experiments occurred.

| Filename | Content |
| --- | --- |
| `power_runs.csv` | Modeled LiHV battery current and 5 V rail behavior across six operating states |
| `tof_samples.csv` | Four ToF directions at 200, 400, 600, 1000 mm, including invalid frames |
| `imu_samples.csv` | Stationary and motor-on drift cases and 12 turning scenarios |
| `camera_trials.csv` | Red, green and empty-field camera trials at several distances and lighting conditions |
| `challenge_runs.csv` | Hypothetical Open and Obstacle Challenge outcomes |
| `functional_checks.csv` | Simulated subsystem check results, explicitly not physical PASS outcomes |
| `revision_comparisons.csv` | Illustration-only V4/V4.1 modeled comparisons |

`SIMULATED_SUMMARY.md` was calculated from these files with the same numeric summary functions used by the existing physical validation script. It does not certify any robot performance.

**Keep these files under `docs/validation/simulated/` and leave `docs/validation/physical/` unchanged until there are actual recorded measurements.**
