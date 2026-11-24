# HyperLine Robotics | WRO 2026 engineering journal

This is the navigation page for the Future Engineers engineering evidence. The six core chapters are supported by source code, CAD, PCB fabrication packages and independently labelled test datasets. Technical evidence is described using one of four classes: **published source/configuration**, **analytical calculation**, **simulation/synthetic example**, or **recorded physical observation**. These are not interchangeable.

| Chapter | Subject | Evidence you can inspect |
|---|---|---|
| [01. Project overview](01-overview.md) | Robot and version scope | Team, vehicle layout, diagrams, historical demonstration |
| [02. Mobility](02-mobility.md) | Drivetrain, wheel and steering decisions | STEP files, drawings, gearing and geometry calculations |
| [03. Power and sensor architecture](03-power-sensors.md) | Electronics, battery, four ToF modules, BMI088, vision | PCB packages, wiring, sensor-selection reasoning, modeled budget and tests |
| [04. Software architecture](04-software.md) | Open/Obstacle controller state flow, UART, camera | Actual published source, controller parameters, code-to-document reconciliation |
| [05. Systems engineering](05-systems.md) | Constraints, decisions, failure analysis and iterations | Design decisions, changes to drivetrain, sensors and control |
| [06. Reproducibility](06-reproducibility.md) | CAD/PCB manufacturing, build, upload, calibration and verification | Exact source paths, versions, tools and acceptance gates |

**Optional recovered camera source:** [v11 and 15 Nov candidate versions](../src/camera_candidates/). They are separate from the original `src/camera.py` and not physically verified.

**Start with:** [README](../README.md) · [Evidence audit](validation/EVIDENCE_AUDIT.md) · [Implementation gaps](validation/IMPLEMENTATION_GAPS.md) · [Source & numerical datasets](validation/README.md) · [Physical record status](validation/physical/SUMMARY.md) · [Reproduction procedure](06-reproducibility.md).

**Scope warning:** the current ZIP has no `.git` directory, and cannot prove submission-time commit history or hardware test dates. The earlier published physical footage is not proof that the latest modular controller or camera source were flashed for that recording.

**PCB fabrication:** [JLCPCB ordering and PCBA tutorial in Chapter 06](06-reproducibility.md#ordering-custom-pcbs-and-assembly-with-jlcpcb).
cpcb), firmware installation and calibration |

The distinction between source-derived constants, estimates, synthetic datasets and physical measurements is intentional. The latter require a dated record with a source revision and identifiable raw evidence. No additional documentation chapters are needed.


[Media and evidence index](media-index.md) · V4.1 assembly, captured print settings, telemetry and ToF diagnostics.
