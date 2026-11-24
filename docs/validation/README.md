# HyperLine Robotics V4.1 validation and evidence index

The data here is intentionally divided into **source configuration/calculations**, **unintegrated conceptual tests**, **simulated examples**, **historical files**, and **real physical records**. A dataset name containing `verified` does **not** prove a motor was tested. Read the `evidence_class` and timestamp fields before drawing conclusions.

- [Evidence audit](EVIDENCE_AUDIT.md) is regenerated from the current sources and primary ledgers by `python tools/documentation_audit.py`.
- [Implementation gaps](IMPLEMENTATION_GAPS.md) explicitly documents the older area-based camera source.
- `v41_source_configuration_verified.csv` contains source-derived pins/configurations only; unsupported camera-height settings have been removed.
- `v41_software_source_consistency.csv` records **failed source matches** for height features not actually published.
- `camera_tests.csv` contains calculated UART command scenarios using the actual area-based camera source. These are **not camera frames or physical observations**.
- `camera_source_contract.csv` contains reproducible pure source tests, **not obstacle accuracy evidence**.
- `v41_camera_height_logic_cases.csv` contains **unintegrated design examples**, not runtime camera tests.
- `historical/` preserves previous example tables and superseded generated claims for transparency.
- `simulated/` contains explicitly simulated test input; these results are never counted in physical summaries.
- `measurement_templates/` and `physical/` contain acceptance criteria and empty data slots for real measurements.
- `physical/SUMMARY.md` is generated only from physically observed rows by `python scripts/summarize_validation.py`.
- `DATASET_INDEX.csv` lists the actual CSV inventory.

**Evidence needed before full reproducibility:** a dated successful Arduino compiler record, the exact SHA of the flashed firmware, V4.1 physical subsystem logs, camera test frames, and representative full challenge trials. The ZIP contains none of those complete current-revision observations.

**Build validation:** the included `.github/workflows/verify-v41.yml` compiles the modular Nano ESP32 controller and captures logs when run on GitHub. It now checks the **actual** bundled camera source contract; the 50/100/150 mm height rule is reported as missing rather than accidentally counted as passing.

## V4.1 image evidence

The [media index](../media-index.md) records new physical assembly photographs, a captured slicer profile, dashboard interface and yaw history, and a populated 8×8 ToF diagnostic frame. These are genuine team-supplied captures; they do not supply a flashed SHA, calibrated reference distances, full trial records or a compiler log. Rows labeled `SYNTHETIC_EXAMPLE_ONLY` in physical-area CSVs are excluded from measured results. Earlier media-directory simulation files are archived separately.
