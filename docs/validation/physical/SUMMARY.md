# V4.1 physical validation summary

Generated from recorded physical observations only. Blank input files produce no results. This summary is not an automated claim that any test passed.

## Electrical load and 5 V stability

| Test / state | Samples | Mean pack current | Peak pack current | Minimum 5 V rail | Recorded pack energy |
|---|---:|---:|---:|---:|---:|
| No physical power log provided | 0 | not available | not available | not available | not available |

## Distance sensor accuracy

Valid ToF samples are evaluated individually against ruler distance. Invalid samples are counted separately; zone frame-rate estimates require timestamps.

| Sensor | Samples | Valid | Invalid | Mean bias | RMSE | Frame rate |
|---|---:|---:|---:|---:|---:|---:|
| No physical ToF log provided | 0 | 0 | 0 | not available | not available | not available |

## BMI088 drift and heading accuracy

| Test / condition | Samples | Mean gyro Z rate | Heading drift | Mean absolute turn error |
|---|---:|---:|---:|---:|
| No physical IMU log provided | 0 | not available | not available | not available |

## OpenMV RT1062 classification

A trial is counted as detected when a red/green classification is reported, and correct only when it matches the reference color.

| Lighting / distance | Trials | Correct | Missed | Wrong colour | Accuracy | Mean processing latency |
|---|---:|---:|---:|---:|---:|---:|
| No recorded camera trials | 0 | 0 | 0 | 0 | not available | not available |

## Subsystem functional test outcomes

| Test | Subsystem | Result | Observation | Evidence |
|---|---|---|---|---|
| No dated physical subsystem test records provided | not available | not available | not available | not available |

## Challenge runs and revision comparisons

| Mode | Runs logged | Documented successful runs | Evidence links recorded |
|---|---:|---:|---:|
| No dated V4.1 run records provided | 0 | 0 | 0 |

<a id="v4-v41-engineering-comparison"></a>
## V4 → V4.1 engineering comparison

| Subsystem | Test method / metric | V4 result | V4.1 result | Assessment |
|---|---|---|---|---|
| No paired revision measurements provided | not available | not available | not available | Cannot quantify improvement |

Every reported result must retain its source CSV, firmware revision, hardware revision, date, conditions and evidence link. This script does not infer test success from the presence of a photograph, sketch or video.
