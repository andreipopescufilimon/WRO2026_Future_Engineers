# V4.1 engineering data

This directory preserves the controller configuration, analytical models, development test inputs and measurement worksheets used during the project.

- [Power model assumptions](power_model_assumptions.csv) and [calculated power scenarios](power_model_scenarios.csv).
- [Controller configuration](v41_source_configuration_verified.csv).
- [Source manifest](evidence/source-sha256.json) and [successful firmware compilation](evidence/ci-build-summary.md).
- [Measurement worksheets](measurement_templates/) and [physical observation ledgers](physical/).
- [Recorded observations](physical/SUMMARY.md), [actual diagnostic matrix](physical/tof_diagnostic_frame_633.csv) and [source and dependency manifest](v41_release_manifest.csv).
- [Camera source checks](camera_source_contract.csv) and [earlier height development function regression](v41_camera_height_function_regression.csv).
- `historical/` and `simulated/` contain earlier development inputs and simulated examples, separate from physical observations.

For the V4.1 photographs, actual ToF diagnostic frame, yaw history and manufacturing screenshots, see the [media index](../media-index.md). The [reproducibility chapter](../06-reproducibility.md) describes the assembly and testing procedure.
