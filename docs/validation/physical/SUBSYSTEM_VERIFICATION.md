# V4.1 subsystem verification guide

This checklist is tied to the **current modular V4.1 controller** under [`src/HLR_FE_Separated/`](../../../src/HLR_FE_Separated/). It records practical tests of the assembled vehicle, rather than results inferred from the source code. The source-level values are listed separately in [`source_configuration.csv`](source_configuration.csv).

## Recording rules

- Enter the **actual test date in UTC**, obtained when the test is performed, not the date this checklist was prepared.
- Enter the **exact Git commit of the code flashed on the Nano ESP32**, not just the most recent documentation commit. Record any uncommitted source changes in the notes.
- Record the hardware revision and any relevant field conditions in `observed` or `notes`.
- Mark `PASS` only after observing the expected behavior on the robot; otherwise record `FAIL`. Leave `NOT_RUN` for checks not yet performed.
- Preserve a serial output, photo, video, ruler measurement or scope capture. Add its actual link or repository path to `evidence_url`.

| ID | Real-world test | Setup and observation | Expected result, based on published firmware |
|---|---|---|---|
| V41-MOTOR-FWD | Drive forward | Lift driven wheels; send a conservative forward command | Rear wheel movement agrees with the intended forward direction; no reset |
| V41-MOTOR-REV | Drive reverse | Lift driven wheels; send a reverse command | Rear wheels reverse; no binding or controller reset |
| V41-SERVO-CENTER | Steering center | Apply servo command `85` | Wheels align with mechanical neutral |
| V41-SERVO-LEFT | Left steering | Apply servo command `39` | Linkage moves left with no hard-stop interference |
| V41-SERVO-RIGHT | Right steering | Apply servo command `131` | Linkage moves right with no hard-stop interference |
| V41-TOF-FRONT | Front ToF | Inspect startup output; move flat target | Sensor initialized at `0x30`; distance responds |
| V41-TOF-LEFT | Left ToF | Same, left module | Sensor initialized at `0x31`; distance responds |
| V41-TOF-RIGHT | Right ToF | Same, right module | Sensor initialized at `0x32`; distance responds |
| V41-TOF-BACK | Rear ToF | Same, rear module | Sensor initialized at `0x33`; distance responds |
| V41-BMI088-YAW | Heading | Keep stationary for the initial **10 seconds**; rotate by a known direction | Reported yaw changes in the same direction and shows a plausible angle; record actual angle and reference |
| V41-OPENMV-UART | Camera UART | RT1062 P4 TX to controller UART RX, common GND; present targets | Messages arrive at **19,200 baud** with expected syntax (`RED`, `GREEN`, `S...`) |
| V41-ENCODER-200 | Encoder distance | Mark start/end on floor, command 200 mm movement and measure with ruler | Record measured travel in mm, tick count when accessible, and signed error; determine PASS against an explicitly chosen tolerance |

The firmware constants are **commands and configuration**, not evidence that a motor, servo, camera, IMU or range sensor passed a bench test. The encoder's `1.831` mm/tick is a *software setting*, not a substitute for measuring traveled distance.

### Useful observed-result examples

An encoder result can be recorded as `commanded=200 mm; actual=... mm; ticks=...; signed error=... mm; surface=...`. For the ToF checks, record the actual serial lines (for example `ToF ready at 0x30`) and an observed distance change, not only that the code declares the address. For the gyro check, record two displayed headings and the independently observed rotation direction.

### Easiest way to fill the ledger

From your cloned repository, run:

```bash
python tools/record_subsystem_checks.py --list
python tools/record_subsystem_checks.py --test V41-MOTOR-FWD
```

Each completed check is written to `docs/validation/physical/functional_checks.csv` with a real UTC timestamp and your local Git commit where available. Repeat the second command with the remaining IDs. The script does **not** automatically mark any hardware test as passing.

The source-grounded values and the project team's published videos are listed in separate CSVs. The filmed robot's historical source is commit `de34faf`; its demonstration does not imply that the newer modular firmware has passed each of these physical checks.
