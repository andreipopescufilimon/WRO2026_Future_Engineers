# Alternative camera implementations | source candidates, not verified uploads

The original uploaded repository has [`src/camera.py`](../camera.py), an area-based OpenMV RT1062 detector. That file is left unchanged so existing robot behavior is not silently overwritten.

Two **user-authored earlier camera files** were recovered from the user's saved project files, not generated as simulated firmware here:

| File | Source origin | Features | Status |
|---|---|---|---|
| [`rt1062_surprise_rule_v11.py`](rt1062_surprise_rule_v11.py) | Earlier saved `OpenMV_RT1062_WRO2026_Surprise_Rule_v11.py`, dated 10 Nov 2026 | `SURPRISE_RULE`, ratio-classified 50/100/150 mm signs, track consistency, top-clipping logic, bottom-Y close threshold; `get_passing_color` mapping | **23/23 source-only pure function tests PASS**, but no physical upload or run record |
| [`rt1062_nov15_height_track_experimental.py`](rt1062_nov15_height_track_experimental.py) | Earlier saved `Pasted text(20261115-183030).txt`, dated 15 Nov 2026 | Alternative top-of-frame height measurement and height tracking, source-level variation after v11 | Python syntax valid; **no included regression or physical test record** |

The November 15 file is chronologically newer, but that does not establish that it is more reliable or compatible with the assembled car. Neither candidate has been verified as the exact program flashed for the published driving demonstration. They must not be described as the competition-ready camera program until tested.

## Reproduce the source-level test

```bash
python tools/test_camera_height_functions.py \
  --source src/camera_candidates/rt1062_surprise_rule_v11.py \
  --output docs/validation/v11_camera_candidate_unit_tests.csv
```

This extracts and executes **pure Python functions** from the v11 file, with controlled input ratios and colors. It does **not** run OpenMV APIs, obtain an image, send data over real UART or certify height classification accuracy. The result is linked from the validation index.

## Making a candidate an actual verified release

1. Keep a copy of the currently working camera code. Select one candidate and document the exact SHA of that file and the matched controller revision.
2. Confirm Python syntax and correct OpenMV API/firmware compatibility **on the actual OpenMV device**. Review any differences in camera hardware/model and calibration constants.
3. Verify UART traffic against the Nano parser at 19,200 baud using the physical cable and power supply, including partial, missing and wrong-color detections.
4. Record calibration frames for all six (height, visible-color) combinations, several measured distances, both driving directions and top-cropped signs. Include false positives, misses and processing latency.
5. Drive at reduced speed first. Retain the complete source and the exact flashed SHA for Open and Obstacle trials.
6. Only then switch the primary `src/camera.py` (or declare an alternate release path) and update all documentation and firmware source hashes together.

A passing algorithm test on a **candidate** is useful progress but not sufficient to upgrade the physical verification status or claim a WRO 30/30 score.
