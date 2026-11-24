# V4.1 engineering observations

## Spatial ranging

The [original diagnostic screenshot](../../docs-media/v41-screenshot-tof-8x8-diagnostic.png) records frame 633 in 8×8 mode. The [transcribed matrix](tof_diagnostic_frame_633.csv) preserves every displayed distance and target status. This capture demonstrates spatial separation of nearby and distant surfaces.

| Quantity | Observed or calculated from the capture |
|---|---:|
| Reported frame age | 3 ms |
| Configured ranging frequency | 15 Hz |
| Displayed failed reads | 0 |
| Valid zones | 49/64 |
| Invalid zones | 15/64 |
| Nearest valid distance | 71 mm |
| Farthest valid distance | 2323 mm |
| Selected R3–R4/C5–C6 median | 630 mm |

The sample contains range returns rather than reference-distance accuracy measurements. Invalid zones retain their displayed status codes. A single frame is not used to calculate a time-series failure rate.

## Heading control

The [recorded yaw-history graph](../../docs-media/v41-screenshot-telemetry-yaw-history.png) compares integrated heading and target heading over a displayed 48.348 s history. The traces follow successive heading steps and converge at the final steady target. The plot provides qualitative feedback-response evidence; numerical error statistics require the original time series.

## Assembly and autonomous operation

- [Assembly recording](https://youtu.be/HSzWATB6nWQ) and [six vehicle views](../../../v-photos/).
- [Published Open and Obstacle driving recording](https://www.youtube.com/watch?v=bg92ulHXomw), identified by the team with controller commit `de34faf039a70443361197de540879f212e3de81`.
- [V4.1 assembly photographs and manufacturing captures](../../media-index.md).

<a id="v4-v41-engineering-comparison"></a>
## V4 to V4.1 development

The [systems chapter](../../05-systems.md) records design changes and the engineering rationale. The [mobility chapter](../../02-mobility.md) contains the team's physical dimensional checks and mechanical calculations.

[Measurement records and test procedures](README.md) keep reference-distance tests, electrical measurements and field-run results separate from the captured diagnostic observations.

## Firmware compilation

The [24 November World Final build results](../evidence/ci-build-summary.md) record successful GitHub Actions compilation of the competition revision `728dc45`. For autonomous recordings, see [Open Round](../../../video/video.md#open-round-open-challenge) and [Final Round](../../../video/video.md#final-round-obstacle-challenge). Camera height trials and full-field runs are recorded in [sign_height_trials.csv](sign_height_trials.csv) and [challenge_runs.csv](challenge_runs.csv), separately from software checks.
