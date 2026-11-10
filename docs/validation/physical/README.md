# Physical verification records

Each CSV contains the checks, conditions, or published references for V4.1.

- `TEST_PLAN` rows specify measurements to capture. Enter instrument readings, capture timestamps and the flashed firmware revision when executing each test.
- The functional ledger preserves the team's existing status labels. Record observed behavior and evidence alongside those labels.
- `source_configuration.csv` contains values from published firmware. `published_demonstrations.csv` references published robot footage.
- For a numeric report, run `python scripts/summarize_validation.py`. The accompanying script excludes test-plan rows from measured statistics.
