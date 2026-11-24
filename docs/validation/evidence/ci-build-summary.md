# World Final firmware compilation

The Nano ESP32 build workflow completed successfully on 24 November 2026. It checks out the identified Git commit, installs the pinned Nano ESP32 board core, checks the camera Python source, runs camera source-function checks, and compiles the eight-file Arduino sketch. The compilation step returns the compiler exit code to GitHub Actions.

| Workflow run | Exact source commit | Job duration | Total duration | Result |
|---|---|---:|---:|---|
| [#5](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/actions/runs/45390277910) | [`9b8aee3`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/9b8aee3e389d551f5701a61e2b5997bcb2228a4d) | 42 s | 45 s | Success |
| [#6](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/actions/runs/45391022064) | [`89e5973`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/89e5973821e572063fdc618d33f62188df92d1df) | 42 s | 45 s | Success |
| [#7](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/actions/runs/45399234569/attempts/1) | [`728dc45`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/728dc45495136473042b53fa7e8e6e1ccd665093) | 49 s | 53 s | Success |

## Current compiled revision

- Commit: [`728dc45495136473042b53fa7e8e6e1ccd665093`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/728dc45495136473042b53fa7e8e6e1ccd665093)
- Board: `arduino:esp32:nano_nora`
- Board core: `arduino:esp32@2.0.18-arduino.5`
- Sketch: [`src/HLR_FE_World_Final/`](../../../src/HLR_FE_World_Final/)
- Camera: [`src/camera.py`](../../../src/camera.py)
- [Exact workflow at this commit](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/728dc45495136473042b53fa7e8e6e1ccd665093/.github/workflows/verify-v41.yml)

```sh
arduino-cli compile --fqbn arduino:esp32:nano_nora --libraries src/libraries --warnings all --verbose src/HLR_FE_World_Final
```

The SHA-256 values calculated from the nine local competition source files match all nine entries in the [manifest published at this commit](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/728dc45495136473042b53fa7e8e6e1ccd665093/docs/validation/v41_release_manifest.csv). The local camera file also matches the final team-supplied script byte-for-byte.

## Build artifacts

| Run | Artifact name | Published artifact SHA-256 |
|---|---|---|
| #5 | `world-final-build-evidence-9b8aee3e389d551f5701a61e2b5997bcb2228a4d` | `007b20c1b059fc611c862710a4670414c47d6310cf5fd9e074b413d8f449a606` |
| #6 | `world-final-build-evidence-89e5973821e572063fdc618d33f62188df92d1df` | `35b874d204f0114aa21ef6795e7abd0719904ea0d77a18b7ccbda27c0b76225f` |
| #7 | `world-final-build-evidence-728dc45495136473042b53fa7e8e6e1ccd665093` | `29bb7af6ffa6f697769362316eaee18b4c91eaf8b965a558960e7a00ef048a0d` |

The public Actions summaries provide the success status, durations and artifact digests. The raw compiler log, exact CLI version and CI-generated source hashes are inside the downloadable artifact; the public summaries do not expose their contents. The artifact SHA-256 identifies the artifact, not the firmware binary. Compilation and camera source-function checks are separate from flashing, physical sign recognition and driving results.

[Build ledger](../physical/build_verification.csv) · [Source manifest](../v41_release_manifest.csv) · [Field records](../physical/README.md)
