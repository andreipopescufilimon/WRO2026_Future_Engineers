# V4.1 validation datasets

`power_component_ratings.csv` contains manufacturer operating points and stated ratings; `power_reference_calculations.csv` contains arithmetic from those ratings. `power_test_matrix.csv` contains a complete instrumentation protocol but **no recorded physical readings**. `power_budget.csv`, `tof_corners.csv` and other populated scenario tables describe deterministic firmware behavior, **not measured trials**.

For field reproducibility, capture electrical readings in `measurement_templates/power_budget.csv`, along with firmware commit and test conditions. Never treat calculated or manufacturer values as observed.
