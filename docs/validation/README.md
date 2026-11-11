# HyperLine Robotics V4.1 validation data

This folder separates documented firmware configuration, deterministic algorithm checks, example scenarios and physical recordings.

- `v41_source_configuration_verified.csv`: source-derived hardware and software settings, including the matching camera script supplied with this patch.
- `v41_software_source_consistency.csv`: checks that documented implementation symbols and pins exist in source.
- `v41_deterministic_logic_checks.csv`, `v41_tof_corner_logic_cases.csv`, `v41_camera_height_logic_cases.csv`: computed boundary cases. These are repeatable logic examples, not sensor recordings.
- `v41_bundled_library_versions.csv`: versions read from repository metadata.
- `v41_release_manifest.csv`, `v41_release_verification_matrix.csv`: source release and status of each verification stage.
- `physical/`: original measurement plans, team test entries and published demonstrations. Actual dates, measured values, verified flashed SHA and real video are added only from tests performed on the robot.
- `simulated/`: explicitly simulated example readings.
- `measurement_templates/`: field measurement layouts.

**Build without PowerShell:** upload `.github/workflows/verify-v41.yml` and run the `V4.1 modular firmware build` workflow in GitHub Actions. Its downloadable artifact contains the actual compiled-source SHA, dependency versions, compile log and a generated build CSV. If the build fails, the captured failure is preserved rather than substituted with a success.

The older driving video is linked to the historical source commit, not to the modular V4.1 controller.
