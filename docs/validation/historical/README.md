# Historical/obsolete evidence

These files preserve prior logic examples and automated results that are **not reproducible from the current bundled camera source**. They must not be counted as verified current firmware tests. `v41_camera_function_unit_tests_unreproducible_previous.csv` was previously presented as passing height-function tests, but `src/camera.py` lacks the functions, so the output cannot be regenerated using the included sources. `camera_area_trigger_legacy_examples.csv` is a derived illustration, not a physically measured camera trial.

- `camera_bottom_height_concept_not_integrated.csv`: old camera examples referring to bottom-Y and height classification absent from `src/camera.py`.
- `v41_source_configuration_claims_before_reconciliation.csv`: earlier metadata claiming missing camera constants; archived to preserve the change trail.
- `nested_camera_tests.csv` and `nested_DATASET_INDEX.csv` if present: files recovered from the duplicate `validation/validation/` tree before it was removed.
