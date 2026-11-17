# Source-to-document mismatches and outstanding implementation evidence

**Audit scope:** files supplied in the uploaded repository ZIP, not a live robot inspection. The documentation previously described functionality that is **not present in the supplied code**. This page preserves that distinction instead of leaving contradictory claims in the judging journal.

## Confirmed mismatches

| Subject | What the current source implements | Prior documentation claim | Corrected evidence status |
|---|---|---|---|
| Camera proximity | `src/camera.py` uses **blob area ≥ 5000** to trigger visible-colour commands | Bottom-edge threshold `CLOSE_BOTTOM_Y = 148` | **Not in bundled camera firmware** |
| Camera exposure | `sensor.set_auto_exposure(False, exposure_us=16000)` | 40,000 µs | Corrected to **16,000 µs** |
| Camera PD gains | `kp_cube=0.4`, `kd_cube=2.4` | `0.80`, `3.20` | Corrected to **0.4/2.4** |
| Camera LAB threshold and ROI | Multiple red/green thresholds; cube ROI starts at 40% height, spans 60% | Different thresholds, 45%/30% ROI | Current file is source of truth; Chapter 04 corrected |
| Camera direction output | `0`, `1`, `2` lines transmitted every loop | No direction messages | Current file sends numeric direction messages |
| Surprise Rule sign heights | No class/track/passing-colour function in `src/camera.py` | `SURPRISE_RULE=True`; `get_cube_height`; 50/100/150 mm | **Not implemented in the primary `src/camera.py`.** Recovered v11 candidate includes it and passes pure-function tests, but no current hardware verification exists |
| ToF mode | `tof.ino` defines `TOF_FRAME_SIDE=4`, `TOF_RANGING_HZ=30` | 8×8 at 60 Hz | Corrected README: active configuration **4×4 at nominal 30 Hz** |
| Real-world verification | Current `docs/validation/physical/SUMMARY.md` shows zero populated physical observation datasets | General descriptive claims of testing | Do not claim measured V4.1 reliability without retained measurements |

## Tasks that require the physical robot or an authentic source revision

- Complete and field-verify the 2026 Surprise Rule implementation, **without altering the prior valid driving release without safety/regression testing**.
- Export a successful Arduino compilation and exact dependency list using the included CI workflow or local build capture. The ZIP has no `.git` directory or Arduino CLI binary.
- Record the 12 subsystem checks using the actual physical robot, with date, **40-character flashed SHA**, observed reading and direct evidence link.
- Run repeated Open and Obstacle layouts, reporting correct passing, laps, parking, battery and failure events, on the same flashed release.
- Collect sensor power/load, ToF ground truth, IMU drift and camera false-positive/false-negative observations to substantiate category 2 at the 6-point level.
- Establish historical competition milestone commits from GitHub itself; no downloaded ZIP can independently reconstruct the absent `.git` history.

The `tools/documentation_audit.py` script regenerates the objective findings and counts. **It cannot generate physical performance data.** A reported or simulated PASS without matching primary evidence must not be treated as a passed physical test.
