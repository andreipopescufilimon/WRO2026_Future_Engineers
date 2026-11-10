# 06. Reproducibility

| Chapter | Topic |
|---|---|
| [01 · Overview](01-overview.md) | Team, robot, challenges, evidence and results |
| [02 · Mobility](02-mobility.md) | Chassis, drivetrain, steering, tires and geometry |
| [03 · Power and sensors](03-power-sensors.md) | Electrical system, PCB, camera, IMU and validation |
| [04 · Software](04-software.md) | Control, perception, state machines and challenge strategy |
| [05 · Systems](05-systems.md) | Engineering trade-offs, iterations, performance and risk |
| [06 · Reproducibility](06-reproducibility.md) | Assembly, firmware, costs and test procedures |

---

| WRO documentation criterion | Where it is covered | Evidence |
| --- | --- | --- |
| Fully reproducible vehicle | [Parts and manufacturing](#parts-and-manufacturing), [Building and flashing](#building-and-flashing), [Calibration and setup](#calibration-and-setup) | Source revision, CAD, manufacturing files, firmware installation, practical calibration and driving demonstration |
| Clear project structure | [Repository structure](#repository-structure) | Separate mechanical, electronic, software, test and documentation directories |
| CAD, code and wiring | [Repository structure](#repository-structure), [Parts and manufacturing](#parts-and-manufacturing) | STEP models, electrical files, Arduino source and OpenMV source |
| Documented testing workflow | [Testing workflow](#testing-workflow) | Bench checks, sensor and actuator diagnostics, field sequence and real driving footage |
| Meaningful development history | [Versions and development history](#versions-and-development-history) | V4 and V4.1 differences, older CAD, project-wide Git history and revision matching |

Our [GitHub history](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commits/main/) covers the **full WRO 2026 repository**, including software, CAD, electrical design and documentation updates, rather than a V4.1-only series of commits. The project has undergone many iterations throughout the season. Old mechanical revisions are kept in [`3D-models/old-3D-models/`](../3D-models/old-3D-models/) so the evolution of the design can be inspected without mistaking previous components for current ones.

### Reproduction routes

The repository preserves the program used in the filmed demonstration and a later modular V4.1 implementation. They are **two different software revisions** and should not be mixed when assembling a complete system.

| Reproduction target | Fixed Git revision | Controller entry point | What it represents |
| --- | --- | --- | --- |
| **Filmed vehicle** | [`de34faf039a70443361197de540879f212e3de81`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) | [`src/HLR_FE/HLR_FE.ino`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/de34faf039a70443361197de540879f212e3de81/src/HLR_FE/HLR_FE.ino) | Source revision identified by the team as running during the [Open and Obstacle driving video](https://www.youtube.com/watch?v=bg92ulHXomw) |
| **Modular V4.1 source** | [`045c57e7388c08f9281642a864de0ba3b099c0df`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) | [`src/HLR_FE_Separated/HLR_FE_Separated.ino`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/045c57e7388c08f9281642a864de0ba3b099c0df/src/HLR_FE_Separated/HLR_FE_Separated.ino) | Later source revision with separate Open and Final files and four VL53L5CX interfaces |

**Practical build sequence:** select one revision, fabricate its matching mechanical/electrical configuration, assemble the vehicle, install its controller and camera programs, perform [calibration](#calibration-and-setup), and follow the [testing workflow](#testing-workflow). The chassis, sensor wiring and firmware are treated as one system, not interchangeable parts from different development stages.

### Evidence from the assembled vehicle

Our documentation covers the physical vehicle, its custom PCB assemblies, the mechanical construction and autonomous-driving demonstrations. The photographs, assembly recording, source code and manufacturing files show the development of the robot from hardware design to operation on the field.

| Evidence | Recorded result | Reference |
| --- | --- | --- |
| Mechanical and electrical assembly | The built vehicle and its PCB, drivetrain, steering and sensor placement are documented in photographs and an assembly recording | [Assembly video](https://youtu.be/HSzWATB6nWQ) · [Vehicle photographs](../v-photos/) |
| Autonomous driving | Our published video documents the physical robot in both Open and Obstacle challenges; the team identifies the filming software as commit `de34faf` | [Driving video](https://www.youtube.com/watch?v=bg92ulHXomw) · [Filming firmware commit `de34faf`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) |
| Camera perception | An RT1062 frame captures red and green detections on the actual vision interface | [Captured camera view](docs-media/v41-openmv-RT1062-camera-view.png) |
| Electronic and mechanical release | The source repository includes STEP models, PCB manufacturing files, the Arduino V4.1 sketch and the RT1062 script | [Mechanical CAD](../3D-models/) · [PCB files](../electrical-schematics/) · [Controller source](../src/HLR_FE_Separated/) · [Camera source](../src/camera.py) |

**Firmware used during filming:** according to the team, the driving video was recorded with Git commit [`de34faf039a70443361197de540879f212e3de81`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81). The associated Arduino program is [`src/HLR_FE/HLR_FE.ino`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/de34faf039a70443361197de540879f212e3de81/src/HLR_FE/HLR_FE.ino). The newer [`src/HLR_FE_Separated/`](../src/HLR_FE_Separated/) firmware is documented separately because its software architecture differs from the filmed version.

---

## Repository structure

Paths are shown from the repository root, while the Markdown links resolve from this chapter inside `docs/`. The six engineering journal chapters are stored in `docs/`.

| Path | Content |
| --- | --- |
| [`README.md`](../README.md) | Main entry point and project navigation |
| [`docs/`](.) | Six technical chapters |
| [`docs/docs-media/`](docs-media/) | Journal illustrations, camera view and dashboard screenshots |
| [`docs/technical-drawings/`](technical-drawings/) | Component and assembly dimensions |
| [`3D-models/`](../3D-models/) | Mechanical STEP models for the chassis assembly, drivetrain, steering, wheels and mounts |
| [`3D-models/old-3D-models/`](../3D-models/old-3D-models/) | Earlier designs preserved for comparison |
| [`electrical-schematics/`](../electrical-schematics/) | PCB schematics, board documentation and manufacturing outputs |
| [`electrical-schematics/Main PCB/production-files/`](../electrical-schematics/Main%20PCB/production-files/) | Main PCB production files |
| [`electrical-schematics/IMU/production-files/`](../electrical-schematics/IMU/production-files/) | IMU PCB production files |
| [`electrical-schematics/TOF sensors/production-files/`](../electrical-schematics/TOF%20sensors/production-files/) | ToF PCB production files |
| [`src/HLR_FE_Separated/`](../src/HLR_FE_Separated/) | V4.1 Arduino Nano ESP32 Open/Final firmware and supporting `.ino` files |
| [`src/camera.py`](../src/camera.py) | OpenMV RT1062 camera-side Python source |
| [`src/HLR_FE_Separated/README.txt`](../src/HLR_FE_Separated/README.txt) | Firmware setup and mode-selection reference |
| [`src/tests/`](../src/tests/) | Motor, steering, IMU, straight-driving and communication tests |
| [`v-photos/`](../v-photos/) | Vehicle photographs from all sides |
| [`t-photos/`](../t-photos/) | Team photograph |
| [`video/`](../video/) | Driving and challenge recordings |
| [`other/`](../other/) | Additional development resources |

The Arduino sketch is divided into multiple tabs, which the Arduino IDE compiles together. Keep them in the same `HLR_FE_Separated` directory.

| File | Responsibility |
| --- | --- |
| [`HLR_FE_Separated.ino`](../src/HLR_FE_Separated/HLR_FE_Separated.ino) | Main initialization, V4.1 mode selection, hardware pins, startup and dispatch |
| [`functions.ino`](../src/HLR_FE_Separated/functions.ino) | Shared movement and floor-marker routines |
| [`gyro.ino`](../src/HLR_FE_Separated/gyro.ino) | BMI088 acquisition and heading calculations |
| [`motors.ino`](../src/HLR_FE_Separated/motors.ino) | Motor, encoder and steering control |
| [`tof.ino`](../src/HLR_FE_Separated/tof.ino) | Four VL53L5CX devices and address sequencing |
| [`Open.ino`](../src/HLR_FE_Separated/Open.ino) | Open Challenge navigation and transitions |
| [`Final.ino`](../src/HLR_FE_Separated/Final.ino) | Obstacle Challenge navigation, colored pillars and parking |

The independent subsystem examples in [`src/tests/`](../src/tests/) cover individual functions; the complete robot program is in `src/HLR_FE_Separated/`.

## Parts and manufacturing

### Main components

The main V4.1 assemblies are listed below. Component variants and manufacturing details are documented in the [electrical schematics](../electrical-schematics/), [mechanical CAD](../3D-models/) and [power and sensor chapter](03-power-sensors.md).

| Assembly | Component or source | Reproduction reference |
| --- | --- | --- |
| Main controller | Arduino Nano ESP32 | [Controller firmware](../src/HLR_FE_Separated/) |
| Vision | OpenMV RT1062 | [Camera script](../src/camera.py) |
| Main structure | Custom PCB chassis | [Main PCB manufacturing files](../electrical-schematics/Main%20PCB/production-files/) |
| Drive | Pololu 50:1 HPCB geared motor, V4.1 configuration | [Mechanical design](02-mobility.md) and motor mounting CAD |
| Rear axle | Geared drivetrain with mechanical differential | [3D models](../3D-models/) |
| Steering | Front steering servo, hubs and parallel linkage | [3D models](../3D-models/) |
| IMU | Custom board based on BMI088 | [IMU manufacturing files](../electrical-schematics/IMU/production-files/) |
| Distance sensing | Custom VL53L5CX ToF sensor boards | [ToF manufacturing files](../electrical-schematics/TOF%20sensors/production-files/) |
| Wheels | Silicone tyres and custom steel rims | [Wheel and mechanical CAD](../3D-models/) |
| Battery | GNB 2S LiHV, 7.6 V nominal, 300 mAh | [Power architecture](03-power-sensors.md) |
| Printed supports | Camera, sensor, steering and battery mounts | [3D models](../3D-models/) |

### Cost analysis and bill of materials

Our robot combines commercial modules, custom PCBs, machined drivetrain parts and printed supports. The values below are **the cost entries supplied by the team for this build**, not live supplier quotations. Custom PCB figures include the assembled board, so the integrated motor driver and board components are not charged again.

| Component or assembly | Quantity | Cost per unit / set (USD) | Extended cost (USD) |
| --- | ---: | ---: | ---: |
| Arduino Nano ESP32 | 1 | $22.43 | $22.43 |
| OpenMV RT1062 camera | 1 | $165.00 | $165.00 |
| Assembled VL53L5CX sensor boards | 4 | $12.00 | $48.00 |
| Assembled BMI088 IMU PCB | 1 | $20.00 | $20.00 |
| Assembled main PCB chassis (including IFX9201SG) | 1 | $13.00 | $13.00 |
| Pololu 50:1 HPCB geared motor with encoder | 1 | $27.00 | $27.00 |
| MG90S steering servo | 1 | $4.00 | $4.00 |
| TCS34725 floor-color sensor | 1 | $2.30* | $2.30* |
| Pololu D24V50F5 5 V regulator | 1 | $32.95 | $32.95 |
| GNB 2S LiHV 300 mAh battery | 1 | $5.10 | $5.10 |
| Wheels and tires | 4 | $6.25 | $25.00 |
| Differential, gears and drivetrain | 1 set | $17.00 | $17.00 |
| 3D-printed components and mounts | 1 set | $10.00 | $10.00 |
| Bearings, fasteners, connectors and wiring | 1 set | $10.00 | $10.00 |
| **Recorded-cost subtotal, excluding provisional item** | | | **$399.48** |
| **Illustrative total including TCS34725 estimate** | | | **$401.78*** |

*The original TCS34725 line contains both `TBD` and `$2.30`. We retain $2.30 as a **provisional estimate**, not a confirmed amount paid. The $401.78 figure includes that estimate; the sum of the other recorded amounts is $399.48. Shipping, tax, tooling, rejected fabrication runs and labor are not included in these component totals.*

#### Manufacturing and cost considerations

**Custom PCBs.** We combine the controller connectors and IFX9201SG driver on the structural main PCB and manufacture separate BMI088 and VL53L5CX boards. This reduces independent module mounts and wiring, but custom fabrication becomes part of the bill of materials. See the [PCB production packages](../electrical-schematics/).

**Drivetrain and mechanics.** The 50:1 gearbox trades theoretical maximum speed for increased gearbox torque multiplication relative to V4's 30:1 arrangement. Printed supports allow changes without replacing the entire chassis. The [mechanical CAD](../3D-models/) and [Chapter 02](02-mobility.md) document the fits and dimensions.

**Reproduction costing.** The table reflects one robot's installed quantities. For commercial purchases, a second team can substitute current prices for the same part numbers; for custom assemblies, batch manufacturing charges should be allocated across the installed units. Competition travel, accommodation and team administration are outside the vehicle reproduction cost.

### Mechanical CAD and assembly reference

The STEP files in [`3D-models/`](../3D-models/) can be inspected in compatible CAD software to understand component geometry and how the assemblies fit together. The [technical drawings](technical-drawings/) provide dimensions, while the vehicle [photographs](../v-photos/) show the assembled orientation and cable routing.

The recorded assembly sequence provides the visual reference for component orientation and placement:

**[Watch the HyperLine Robotics assembly video](https://youtu.be/HSzWATB6nWQ).**

### PCB fabrication and electrical preparation

The electronics are organized into three manufacturing groups:

1. [Main PCB](../electrical-schematics/Main%20PCB/production-files/), which also serves as the structural chassis.
2. [IMU PCB](../electrical-schematics/IMU/production-files/) for inertial measurements.
3. [ToF sensor PCB](../electrical-schematics/TOF%20sensors/production-files/) for distance sensing.

The released manufacturing packages include Gerber, BOM and pick-and-place files for the main chassis, IMU and ToF boards. Fabricate each board using its matching package and populate it according to the schematic and board revision. The electronic files and [chapter 03](03-power-sensors.md) should be consulted together because board fabrication files do not, by themselves, describe every connection between separate modules.

Before applying battery power, inspect solder joints, power polarity, connector orientation and continuity. Check that there is no unintended short between supply rails and ground. Confirm the expected regulated voltages before connecting sensitive modules. Disconnect power before modifying wiring.

---

## Building and flashing

### 1. Obtain a fixed project revision

Clone the repository and check out the software configuration associated with the build being reproduced:

```bash
git clone https://github.com/andreipopescufilimon/WRO2026_Future_Engineers.git
cd WRO2026_Future_Engineers
```

For the **published driving demonstration** (the earlier `HLR_FE` sketch), use:

```bash
git checkout de34faf039a70443361197de540879f212e3de81
```

For the **later modular V4.1 design** (`HLR_FE_Separated` with `Open.ino` and `Final.ino`), use:

```bash
git checkout 045c57e7388c08f9281642a864de0ba3b099c0df
```

The filmed program is identified by the team in the [driving demonstration](https://www.youtube.com/watch?v=bg92ulHXomw). The modular sketch is a later implementation, not the original recorded binary. Use the CAD, component variant, PCB production files and camera source **from the selected snapshot**. When switching revisions, reopen the corresponding sketch and review the wiring and calibration settings instead of reusing values blindly.

### 2. Assemble the mechanics


The [assembly video](https://youtu.be/HSzWATB6nWQ) shows the construction sequence. The corresponding assembly stages are:

1. **Main chassis:** Prepare the fabricated structural PCB and attach its mechanical supports using the corresponding [STEP models](../3D-models/) and [drawings](technical-drawings/) for positioning.
2. **Rear drivetrain:** Install the drive motor, its gear transmission and the differential. Rotate the rear axle by hand to confirm that gears mesh and there is no interference or excessive binding.
3. **Rear wheels:** Fit the wheel assemblies and verify that each wheel is secured, aligned and free to rotate as intended by the differential.
4. **Front steering:** Install the hubs and parallel linkage. Center the steering servo using the steering test before fixing the servo horn, then check both steering directions and both mechanical limits.
5. **Battery and supports:** Mount the battery securely so it cannot shift into the drivetrain or steering linkage. Check clearance against the CAD model.
6. **Vision and distance sensing:** Install the OpenMV RT1062 and ToF modules in their designated mounts. Check camera orientation, field of view and unobstructed sensor openings.
7. **Final inspection:** Route and secure cables clear of gears and steering parts. Confirm that all wheel and steering movements remain unobstructed before powering the vehicle.

The servo neutral position, steering geometry and wheelbase are described in [02. Mobility](02-mobility.md) and the [technical drawings](technical-drawings/). Correct mechanical alignment prevents assembly errors from appearing as steering-control problems.

### 3. Prepare the software environment

The Arduino Nano ESP32 runs the vehicle controller; the OpenMV RT1062 runs the separate camera script. The source files are part of the selected project snapshot. The later V4.1 controller imports the following libraries and interfaces:

| Device or dependency | Required interface | Installation or source reference |
| --- | --- | --- |
| Arduino Nano ESP32 board package | Nano digital/analog pin names, `Serial0`, `Wire`, GPIO and PWM | Install the **Arduino Nano ESP32** board platform through Arduino IDE Boards Manager |
| `ESP32Encoder` | Encoder library included by the controller | Install through Arduino IDE Library Manager |
| `ESP32Servo` | `Servo.attach()` and `Servo.write()` for front-wheel steering | Install through Arduino IDE Library Manager |
| SparkFun VL53L5CX Arduino Library | `SparkFun_VL53L5CX` methods used by [`tof.ino`](../src/HLR_FE_Separated/tof.ino) | Install the SparkFun VL53L5CX Arduino Library |
| BMI088 library | `BMI088.h` exposing `setAccScaleRange`, `setAccPoweMode`, `setGyroOutputDataRate` and `getGyroscopeZ` | Use a BMI088 implementation with the **same API as [`gyro.ino`](../src/HLR_FE_Separated/gyro.ino)**; libraries with the same header name are not necessarily interchangeable |
| OpenMV IDE and RT1062 firmware | `sensor`, `machine.UART` and `machine.LED` in MicroPython | Run [`src/camera.py`](../src/camera.py) on the RT1062 |

`Wire` is supplied by the Arduino board platform. The TCS34725 is accessed directly over its separate I²C bus, rather than through an additional Arduino color-sensor library.

**Included Arduino libraries.** To make the software environment easier to reproduce, we have included the library versions used during development in [`src/libraries/`](../src/libraries/). The folder contains **Seeed Studio BMI088 v1.0.0**, **ESP32Encoder v5.0.0** by Kevin Harrington, and **Adafruit TCS34725 v1.4.4**. These copies preserve the library implementations alongside our source code, particularly the BMI088 interface used by the robot's gyroscope routines. To install them, copy each library directory into the Arduino sketchbook's `libraries` folder, restart Arduino IDE, and verify that the compiler selects these copies. The Arduino Nano ESP32 board package, `ESP32Servo` and SparkFun VL53L5CX library are installed separately through Arduino IDE.

**Dependency identification:** the Git snapshot fixes the project's source but does not encode the development PC's exact installed board-core and library version numbers. On a computer reproducing the build, Arduino IDE's **Boards Manager** and **Library Manager** identify the selected installations. Arduino CLI can record the same information:

```bash
arduino-cli version
arduino-cli core list
arduino-cli lib list
arduino-cli board list
```

Use the selected Nano ESP32 board and the corresponding sketch revision when verifying the build. In Arduino IDE, **Sketch → Verify/Compile** displays errors, warnings and the actual dependency paths selected by the compiler. If a different BMI088 API is installed, resolve the library mismatch rather than changing the robot's pinout or sensor algorithm to silence errors.

### Published V4.1 implementation references

The repository contains the assembled vehicle documentation and the separated V4.1 controller source. The following references give builders the files and recordings needed to understand the implementation.

| Item | Verified or documented information | Evidence |
| --- | --- | --- |
| Filmed robot's source revision | de34faf039a70443361197de540879f212e3de81 identified by the team as the code running in the driving recording; original sketch in `src/HLR_FE/` | [Filming firmware commit `de34faf`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) |
| Current V4.1 source checkpoint | Repository snapshot `045c57e7388c08f9281642a864de0ba3b099c0df` (10 November 2026), containing the newer separated sketch | [Later source snapshot](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) |
| V4.1 controller source | `src/HLR_FE_Separated/HLR_FE_Separated.ino` with `Open.ino`, `Final.ino` and the hardware-control tabs | [Controller firmware](../src/HLR_FE_Separated/) |
| RT1062 image-processing demonstration | Captured camera frame shows red and green object detections | [Camera view](docs-media/v41-openmv-RT1062-camera-view.png) |
| Physical autonomous driving | We published our real-world driving demonstration for both Open and Obstacle modes | [Driving video](https://www.youtube.com/watch?v=bg92ulHXomw) |

The separate V4.1 sketch tabs compile together as one Arduino program. The board package and supporting libraries are installed through Arduino IDE before uploading.

### 4. Flash the Arduino Nano ESP32

The Arduino Nano ESP32 executes the vehicle control software. Its entry point is [`src/HLR_FE_Separated/HLR_FE_Separated.ino`](../src/HLR_FE_Separated/HLR_FE_Separated.ino).

1. Install Arduino IDE and the **Arduino Nano ESP32** board support package through **Boards Manager**.
2. Connect the controller using a **USB-C data cable**.
3. Open `src/HLR_FE_Separated/HLR_FE_Separated.ino`. Confirm that `functions.ino`, `gyro.ino`, `motors.ino`, `tof.ino`, `Open.ino` and `Final.ino` open as tabs in the same sketch.
4. Install the dependencies needed by the selected revision. Use the supplied library copies where appropriate and check for compilation conflicts.
5. Select **Tools → Board → Arduino Nano ESP32**, then choose the correct port in **Tools → Port**.
6. Before power-up, select the challenge at the D11/D12 inputs: **D11 HIGH and D12 LOW = Final; D12 HIGH and D11 LOW = Open**. Both HIGH or both LOW activate a safe stop. These inputs accept **3.3 V logic**. Inspect the motor direction, steering limits, encoder parameters, IMU settings, camera communication and driving speeds against the assembled hardware. The unused fan control remains at zero.
7. Select **Verify** and resolve compile errors before uploading.
8. Raise the driving wheels off the floor, select **Upload**, then restart the controller.
9. Open **Serial Monitor** at **115200 baud** for controller diagnostics. The RT1062 UART link uses **19,200 baud** on a separate interface. The four ToF addresses are front `0x30`, left `0x31`, right `0x32` and back `0x33`.

### 5. Flash the OpenMV RT1062

The OpenMV RT1062 performs image capture and detection of coloured obstacles. The project camera source is [`src/camera.py`](../src/camera.py).

1. Install **OpenMV IDE** and connect the **OpenMV RT1062** through a USB data cable.
2. Connect to the camera in OpenMV IDE and complete any required camera firmware initialization.
3. Open the RT1062-compatible `camera.py`, select **Run**, and inspect the image in the IDE frame buffer.
4. Present red and green traffic signs within the field of view. Check the image orientation, region of interest, colour classification and detection overlays.
5. Verify the communication link to the Nano ESP32. Camera transmit must reach the appropriate controller receive connection; the controllers must share a common ground and use a matching baud rate and message format. Use the [Arduino/OpenMV communication test](../src/tests/arduino_openmv_serial_communication/) to check the connection independently of driving.
6. When the script runs correctly, use **Tools → Save open script to OpenMV Cam** so that it is saved as the camera's startup program. Restart the RT1062 without the IDE and verify that detection and communication resume.

On the RT1062, **P4 = UART1 TX** and **P5 = UART1 RX**, at **19,200 baud** in this release. Connect TX to the Nano UART receive and use a common GND. Both devices use 3.3 V logic. Keep the matching camera script and controller revision together. The controller also routes its camera communication over D0/D1 (Serial0).

<img src="docs-media/v41-openmv-RT1062-camera-view.png" alt="OpenMV RT1062 camera frame showing the image used for obstacle detection" width="32%">

*Figure 6.1. OpenMV RT1062 frame buffer used to verify image orientation, detection thresholds and the active region of interest.*

---

## Calibration and setup

The steps below connect the assembled robot to the values in the **modular V4.1** firmware. The earlier filmed program has a different hardware and software layout and uses the constants in its own commit. This section is a compact index of the settings that affect repeatability; the calculations and full sensing rationale remain in [02. Mobility](02-mobility.md), [03. Power and sensors](03-power-sensors.md) and [04. Software](04-software.md).

### Mechanical, sensor and communications calibration

| Order | Subsystem | V4.1 source setting and procedure | Expected check |
| ---: | --- | --- | --- |
| 1 | **Steering neutral and stops** | [`HLR_FE_Separated.ino`](../src/HLR_FE_Separated/HLR_FE_Separated.ino) sets `STEERING_CENTER = 85`, `STEERING_LEFT = 39`, `STEERING_RIGHT = 131`, and parking endpoints `40` / `138`. Center the linkage before fixing the servo horn; inspect both wheel clearances with the wheels raised. | The wheels are aligned at neutral; commanded travel does not force the links into the chassis or steering stops. Servo command values are not physical wheel angles. |
| 2 | **Motor direction and encoder** | Rear motor PWM on D7, direction on D8; encoder A on D5 (rising-edge interrupt), B on D4 (direction). The source defines `WHEEL_DIAM = 29.0` mm and `MM_PER_TICK = 1.831` mm for its distance helper. Drive a measured straight distance on the actual surface and compare to `read_cm()`. | Positive wheel travel has the expected encoder direction and measured displacement agrees with the selected distance conversion. |
| 3 | **BMI088 heading** | [`gyro.ino`](../src/HLR_FE_Separated/gyro.ino) estimates Z-axis bias over `DRIFT_TEST_TIME = 10` seconds at boot, with ±2000°/s gyro and ±6 g accelerometer settings. Keep the robot still during this interval, then rotate it manually both ways. | Heading increases/decreases consistently with the sign expected by the steering controller, without visible stationary jumps. |
| 4 | **Four ToF matrices** | [`tof.ino`](../src/HLR_FE_Separated/tof.ino) releases one LPn pin at a time and assigns front A3=`0x30`, left A2=`0x31`, right A1=`0x32`, rear A0=`0x33`; each is 4×4, configured at 30 Hz. Test all four optical directions with a flat target and check that mount/wheels do not occlude the field of view. | All four sensors initialize with unique addresses and provide valid ranges for targets in their respective directions. |
| 5 | **Front corner zones** | The Open controller uses front ToF zones `(2,3)` and `(3,3)` (1-based row/column) for a corner below 600 mm, with rearming once both are at least 800 mm. Data older than 150 ms is rejected. Check which physical side corresponds to those zones after mounting. | The correct wall and zones trigger a single turn, without mirrored indexing or repeated corner events. |
| 6 | **Floor colors** | The TCS34725 uses separate I²C pins A6/A7. [`functions.ino`](../src/HLR_FE_Separated/functions.ino) reads normalized RGB channels; the source selects 16× gain and a short exposure. Present white, blue and red/orange field markings at the installed floor clearance. | White re-arms detection and colored markers set or confirm the intended travel direction. |
| 7 | **RT1062 image and UART** | [`camera.py`](../src/camera.py) configures QVGA 320×240, `vflip=True`, `hmirror=True`, gain/AWB disabled, fixed 16,000 µs exposure and `UART(1, 19200)`. Connect P4 TX to Nano RX D0, and share ground. Recheck red/green blob thresholds under the actual lighting. | Red and green objects appear in the configured region of interest; the Nano receives correctly formatted UART messages. |
| 8 | **Round selection and start** | D12 HIGH and D11 LOW selects Open; D11 HIGH and D12 LOW selects Final. Both HIGH or both LOW leave the program stopped. Restart after changing mode. D10 is an active-LOW start input. | The requested challenge starts only after initialization and the start action; invalid mode combinations never drive the motor. |

For encoder calibration, with the same counting convention as the selected sketch, a useful correction after measuring one straight run is:

\[
\mathrm{MM\_PER\_TICK}_{\mathrm{new}}=\mathrm{MM\_PER\_TICK}_{\mathrm{old}}\frac{d_{\mathrm{measured}}}{d_{\mathrm{commanded}}}.
\]

A physical wheel diameter does **not** by itself establish the correct ticks-per-millimeter value, because interrupt edge counting and the drivetrain's actual motion must be considered. Repeat the distance check after any tire, wheel, encoder or gearbox change.

### Challenge configuration reference

The following numbers are **firmware configuration values**, not measured lap-time results. They identify the modular source revision for reproducing the controller behavior.

| Setting | Open Challenge (`Open.ino`) | Obstacle / Final Challenge (`Final.ino`) |
| --- | ---: | ---: |
| `max_turns` | 12 | 13 |
| Initial `robot_speed` command | 250 (first-marker logic temporarily uses 80) | 100 |
| `kp` | 0.034 | 0.044 |
| `ki` | 0 | 0 |
| `kd` | 0.082 | 0.082 |
| `delayTurns` | 1000 ms | 1500 ms |
| Additional behavior | ToF-driven turns; corner PWM command 80 | Colored-pillar interaction and parking; `park_speed = 45` in main sketch |

Motor speed values are controller PWM commands on an 8-bit scale, **not km/h or motor terminal voltage**. The nominal 6 V motor is intentionally operated from the robot's 2S LiHV drive rail as documented in [Chapter 03](03-power-sensors.md); avoid interpreting the PWM fraction as an electrical or thermal protection guarantee.

---

## Running the vehicle

After flashing both devices, disconnect programming cables as appropriate for the power setup and secure the LiHV battery. Select the mode with **one** 3.3 V HIGH input: D12 for Open or D11 for Final. The sketch latches that selection when powered on. During initialization, keep the vehicle stationary for the BMI088's 10-second bias calibration; the start input is D10 (active LOW). The controller then waits for a start command before driving. The [calibration and setup sequence](#calibration-and-setup) summarizes the checks that connect the code settings to the assembled geometry.

### Connect to the local web interface

The **Wi-Fi-enabled diagnostic firmware variant** provides a Wi-Fi access point and browser-based interface for live measurements and configuration. The supplied `HLR_FE_Separated` autonomous firmware does **not** contain this dashboard server. The dashboard steps below apply only after separately flashing and validating a Wi-Fi-capable diagnostic revision.

1. Power the robot using the Wi-Fi-enabled V4.1 firmware.
2. On a phone or computer, join the Wi-Fi network broadcast by the robot. The network name and credentials are configured in the controller firmware.
3. While connected to this network, open **[http://192.168.4.1](http://192.168.4.1)** in a browser.
4. Use the dashboard to inspect telemetry and change the available driving parameters. No external internet connection is required.

The dashboard allows us to inspect the internal state while testing. It is intended for setup and debugging, not as a substitute for checking what the physical vehicle is doing.

### Live distance sensor checks

The ToF telemetry is used to observe the distances reported by the installed sensors. Place a flat target in front of a sensor, move it to a few known distances and compare the reported measurements with a ruler. Check each sensor separately. If readings do not change, fluctuate unexpectedly or disagree strongly with the target position, inspect the sensor orientation, wiring, configuration and field of view before tuning driving behavior.

### IMU and heading checks

With the vehicle stationary, observe the IMU readings and look for unexpected offset or rapid drift. Rotate the robot manually in a known direction and check that the reported heading changes consistently with the physical rotation. Repeat in the opposite direction to detect an inverted sign or incorrect axis mapping.

<img src="docs-media/telemetry-debug.jpeg" alt="Web dashboard displaying live robot telemetry and IMU debugging values" width="32%">

*Figure 6.2. Live telemetry used to inspect IMU and sensor behavior before on-track testing.*

### Speed and configuration controls

The settings page provides runtime adjustments for the driving and maneuvering parameters exposed by the diagnostic firmware. We use consistent track conditions when changing speed or steering parameters, adjusting one parameter at a time to distinguish its effect.

<div align="center">
  <img src="docs-media/robot-adjust-1.jpeg" alt="Robot web interface configuration panel, screenshot 1" width="32%">
  <img src="docs-media/robot-adjust-2.jpeg" alt="Robot web interface configuration panel, screenshot 2" width="32%">
  <img src="docs-media/robot-adjust-3.jpeg" alt="Robot web interface configuration panel, screenshot 3" width="32%">
</div>

*Figures 6.3, 6.4 and 6.5. Configuration views for speed, steering and subsystem adjustments.*

---

## Testing workflow

Our verification approach has three stages: individual subsystems, integrated stationary operation and full-field autonomous driving. This separates mechanical faults, sensor configuration errors and navigation behavior. Calibration settings are listed in [Calibration and setup](#calibration-and-setup); the [driving video](https://www.youtube.com/watch?v=bg92ulHXomw) documents the physical vehicle running both challenge configurations using the filmed source revision identified above.

### 1. Independent subsystem tests

The test code is organized under [`src/tests/`](../src/tests/). Each test isolates a part of the vehicle before it is used together with the other systems.

| Test | Source | What a successful check looks like |
| --- | --- | --- |
| Drive motor | [`motor_test`](../src/tests/motor_test/) | A forward command produces the intended wheel direction; there is no binding or unexpected restart |
| Steering | [`servo_steering`](../src/tests/servo_steering/) | Wheels return to neutral and move in both steering directions without hitting mechanical stops |
| IMU | [`gyro_test`](../src/tests/gyro_test/) | Readings respond to manual rotation with the correct sign and do not show an obvious stationary fault |
| Straight-line controller | [`move_straight_on_gyro`](../src/tests/move_straight_on_gyro/) | The correction acts against a heading deviation rather than increasing it |
| Camera communication | [`arduino_openmv_serial_communication`](../src/tests/arduino_openmv_serial_communication/) | Nano receives valid, correctly formatted messages from the RT1062 |
| ToF sensor checks | [`tof.ino`](../src/HLR_FE_Separated/tof.ino) and optional [diagnostic dashboard](#live-distance-sensor-checks) | Each installed sensor responds to a target moving through its field of view |
| Camera classification | [OpenMV frame buffer](#5-flash-the-openmv-rt1062) | Red and green objects in the configured ROI are identified consistently |

Initial motor and steering checks are performed with the driven wheels raised. The table describes the visible and electronic responses of each subsystem.

### 2. Bench verification with telemetry

Once individual components pass, run the controller and camera together while the robot remains stationary. Use the serial diagnostic output or the [optional Wi-Fi diagnostic interface](#connect-to-the-local-web-interface), if installed, to read ToF and IMU data, and use the OpenMV IDE frame buffer to verify visual detection. Also check that the camera's serial messages reach the Nano ESP32 and that the robot initializes without unexpected motion.

A useful bench sequence is to rotate the robot by hand, move targets past the distance sensors and show red and green pillars to the camera. If the physical input changes but the reported state does not, investigate the relevant subsystem before changing the autonomous strategy.

### 3. Full-field verification

For full-field verification, start from the assembled and calibrated vehicle, then run Open Challenge in both travel directions and Obstacle Challenge with the intended red/green pillar arrangement and parking geometry. Observe turn detection, obstacle pass side, heading recovery and the final stop. The video index and footage below document the robot on the field; the exact filming software revision is [`de34faf`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81).

Field runs use the obstacle geometry and driving behavior described in [04. Software](04-software.md). Video evidence is collected under [`video/`](../video/), while design changes and engineering decisions are discussed in [05. Systems and testing](05-systems.md).

### V4.1 functional verification results

Our project includes a recorded assembly, physical driving footage for both challenge configurations and an RT1062 detection frame. The table links those materials to the relevant hardware and firmware documentation.

| Test | Evidence from the robot or repository | Documented result |
| --- | --- | --- |
| Physical vehicle assembly | [Assembly video](https://youtu.be/HSzWATB6nWQ), [vehicle photographs](../v-photos/) | **Documented:** completed robot and assembly process shown |
| Open Challenge operation | [Recorded driving demonstration](https://www.youtube.com/watch?v=bg92ulHXomw), [video index](../video/video.md) | **Recorded:** physical robot driving autonomously in the Open Challenge |
| Obstacle Challenge operation | [Recorded driving demonstration](https://www.youtube.com/watch?v=bg92ulHXomw), [video index](../video/video.md) | **Recorded:** physical robot driving autonomously in the Obstacle Challenge |
| RT1062 red and green perception | [Captured camera screenshot](docs-media/v41-openmv-RT1062-camera-view.png) | **Captured:** detection interface displaying red and green targets |
| Controller firmware and source | [V4.1 sketch](../src/HLR_FE_Separated/) | **Available:** published source modules for the controller and both challenge modes |
| Four VL53L5CX modules | [Sensor implementation](../src/HLR_FE_Separated/tof.ino), [sensor calibration section](03-power-sensors.md#51-distance-sensor-calibration-matrix) | **Implemented in source:** four sensor addresses and 4×4 ranging configuration |
| BMI088 and steering/drive | [Gyro implementation](../src/HLR_FE_Separated/gyro.ino), [motor implementation](../src/HLR_FE_Separated/motors.ino) | **Implemented in source:** IMU acquisition, steering and motor-control routines |
| Battery and regulated-power reliability | [Power and sensor architecture](03-power-sensors.md) | **Documented:** 2S LiHV supply, regulator, power distribution and protection considerations |

The videos and camera frame document the assembled robot and its operation. The historical controller and camera source correspond to the filmed revision identified above. The separated V4.1 source is provided as the newer modular implementation.

The assembly recording, challenge footage and RT1062 frame buffer provide the practical examples accompanying this reproduction guide. The testing examples above describe how each subsystem is inspected before autonomous field operation.


### 4. Verifying one complete modular V4.1 revision

The modular implementation must be checked as a **single hardware, controller and camera combination**. The earlier `de34faf` footage demonstrates the historical vehicle; it is not substituted for an execution record of `src/HLR_FE_Separated/`. The `045c57e` revision above identifies an earlier modular source checkpoint. When reproducing the latest Open/Final code, capture the **full Git SHA of the actual release being flashed** so the controller, camera code, bundled libraries and manufacturing resources can be traced to the same repository state.

From a clean clone of the intended modular release, record its identity before installing or compiling:

```bash
git rev-parse HEAD
git status --short
python scripts/repro_preflight.py
```

The preflight checks required files, firmware declarations and source integrity; it also creates a `docs/validation/evidence/source-sha256.json` and a `docs/validation/evidence/static-preflight-results.md` when run from the repository root. These checks establish which files were inspected. They do **not** substitute for compilation or a physical driving test. Record any local firmware modifications alongside the SHA, since uncommitted changes are not represented by `git rev-parse HEAD` alone.

For the **actual Nano ESP32 compilation** on the Windows development computer, install the board platform and the [provided libraries](../src/libraries/), then run the repository's existing build-capture tool from PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\capture_build_environment.ps1
Get-Content .\docs\validation\evidence\local-build\arduino-build-result.txt
```

This tool records the selected Arduino CLI version, board platforms, installed libraries, Git revision, source-file hashes and full compile output in `docs/validation/evidence/local-build/`. A **successful build record** consists of `arduino-build-result.txt` reporting exit code `0` and an `arduino-build.log` showing the same build. If the compiler reports a missing library, ambiguous `BMI088.h`, or another error, fix the environment and rerun the capture. The recorded result is more useful than a claim that a sketch *should* compile. The library copies in `src/libraries/` identify the exact supplied implementations; the build log is also needed to identify the versions actually selected by the compiler.

For the RT1062, open the matching `src/camera.py` with OpenMV IDE, record the camera firmware version shown by the connected camera, save the script to the camera and retain one captured frame and one UART message log. The controller and camera must agree on **19,200 baud**; the image demonstration and any height-classification results must identify the camera script revision used.

| Evidence item | Source or output location | Minimum information for reproduction |
| --- | --- | --- |
| Reproduced repository release | `git rev-parse HEAD` and source manifest | Full Git commit SHA, local changes if any, matching controller and RT1062 scripts |
| Arduino compilation | [`scripts/capture_build_environment.ps1`](../scripts/capture_build_environment.ps1) | Compile command, installed dependency versions, exit code and full compiler log |
| Compilation summary | [`build_verification.csv`](validation/physical/build_verification.csv) | Test date, hardware revision, flashed source SHA, board/core versions, result and retained log path |
| Source integrity | [`scripts/repro_preflight.py`](../scripts/repro_preflight.py) | File-presence and configuration checks plus SHA-256 manifest |
| RT1062 setup | [`camera.py`](../src/camera.py) and OpenMV IDE | Camera firmware version, script revision, image capture and received UART data |
| Physical checks | [`functional_checks.csv`](validation/physical/functional_checks.csv) | Each test's observed behavior, actual date, firmware SHA, outcome and supporting media/log |
| Autonomous operation | [`challenge_runs.csv`](validation/physical/challenge_runs.csv) | Challenge mode, course direction, outcome, laps, filming source SHA and video |

### 5. Physical subsystem checks tied to the same firmware

The [subsystem verification guide](validation/physical/SUBSYSTEM_VERIFICATION.md) lists the bench procedure for each installed actuator, sensor and communications link. The [functional test ledger](validation/physical/functional_checks.csv) and [individual observation record](validation/physical/subsystem_test_record.csv) contain the corresponding test identifiers. A check becomes independently reviewable when it records **what the physical robot did**, not only the intended firmware behavior.

| Check group | Test IDs | Observation to retain |
| --- | --- | --- |
| Rear drive | `V41-MOTOR-FWD`, `V41-MOTOR-REV` | Actual wheel rotation direction and whether the controller reset or the drivetrain bound |
| Steering | `V41-SERVO-CENTER`, `V41-SERVO-LEFT`, `V41-SERVO-RIGHT` | Commands 85, 39 and 131, wheel/linkage response and clearance at each end |
| Four VL53L5CX modules | `V41-TOF-FRONT`, `V41-TOF-LEFT`, `V41-TOF-RIGHT`, `V41-TOF-BACK` | Startup lines for addresses `0x30`, `0x31`, `0x32`, `0x33` and at least two actual target-distance readings per direction |
| BMI088 | `V41-BMI088-YAW` | Ten-second stationary calibration, before/after heading values and the known direction of manual rotation |
| RT1062 UART | `V41-OPENMV-UART` | Actual received `S...`, `RED` or `GREEN` message at 19,200 baud and which physical target caused it |
| Encoder motion | `V41-ENCODER-200` | 200 mm commanded movement, measured chassis travel, ticks if available, surface and signed distance error |

The existing [`tools/record_subsystem_checks.py`](../tools/record_subsystem_checks.py) writes a **dated** observation into the primary CSV after a test. Run it from the repository root after performing each check:

```bash
python tools/record_subsystem_checks.py --list
python tools/record_subsystem_checks.py --test V41-ENCODER-200
```

The recorder requests the observed result, outcome, flashed firmware SHA, hardware revision and evidence path. Apply the same test ID and firmware SHA when updating the separate [`subsystem_test_record.csv`](validation/physical/subsystem_test_record.csv), so the two ledgers refer to the same physical event. Where possible, keep the raw serial capture, photographs, video or ruler reading in `docs/validation/evidence/` and link to the specific file rather than to an unrelated general demonstration.

For numeric characterization, the appropriate datasets are [`tof_samples.csv`](validation/physical/tof_samples.csv), [`imu_samples.csv`](validation/physical/imu_samples.csv), [`power_runs.csv`](validation/physical/power_runs.csv) and [`camera_trials.csv`](validation/physical/camera_trials.csv). Each recorded range or sensor reading should retain its measurement conditions and the source revision. After entering genuine data, run `python scripts/summarize_validation.py` to create the `docs/validation/physical/SUMMARY.md`. Reference data and calculated software scenarios belong in their corresponding non-physical datasets; they should not be merged into measured results.

### 6. Demonstrating the current Open, Obstacle and Surprise Rule implementation

The modular V4.1 Open program chooses a course direction from the first valid TCS34725 floor marker, then uses two front ToF zones with **600 mm trigger** and **800 mm re-arm** thresholds for corners. The Obstacle program receives the RT1062's red/green and steering messages and uses the ToF sensors for distance-dependent positioning and parking. [Chapter 04](04-software.md) documents the exact state transitions and code; the following test matrix records how to verify the implemented behavior on a complete field.

| Mode or feature | Physical procedure | What the recording must show | Dataset |
| --- | --- | --- | --- |
| Open, left course | Select D12 HIGH; present the blue floor marker; run a regulation-layout course | Direction selected, front ToF corners recognized, achieved corner/lap count and safe final stop | [`challenge_runs.csv`](validation/physical/challenge_runs.csv) |
| Open, right course | Select D12 HIGH; present the red/orange marker; repeat | Opposite course direction and completed behavior, including any missed or duplicate turns | [`challenge_runs.csv`](validation/physical/challenge_runs.csv) |
| Obstacle, left/right course | Select D11 HIGH; use the intended red/green signs and parking area | Follow command, correct pass side, recovery, accepted floor markers and parking attempt | [`challenge_runs.csv`](validation/physical/challenge_runs.csv) |
| Camera sign heights | Present **50, 100 and 150 mm** high, **50 mm** wide signs, both red and green, at the test distances used by the team | Captured frame, actual predicted height, transmitted passing command and correct/incorrect classification | [`sign_height_trials.csv`](validation/physical/sign_height_trials.csv) |

For camera-height tests, the algorithm's intended mapping is **50 mm and 150 mm: invert the visible red/green command; 100 mm: retain it**. The [camera source](../src/camera.py) also contains tracking, partial-visibility handling and an unknown-class fallback. Record both correctly classified and misclassified trials; a threshold written in software is not a measured classification success rate.

Each video or run record for the modular release should include the full **source SHA actually flashed**, the camera revision, hardware version, challenge direction and outcome. This is separate from the real driving video for historical commit `de34faf`. A short continuous run with clearly identifiable firmware is more convincing evidence for the newer release than an unversioned performance claim. Keep failed or partial runs in the log as well as successful ones so another team can understand the operating limits.

### 7. Final reproducibility audit

Before submission, compare the repository's physical evidence and source files using the following acceptance checklist:

| Audit item | Check before submission |
| --- | --- |
| One consistent release | The controller sketch, camera script, bundled libraries, PCB and mechanical revision match a recorded Git SHA |
| Compilable firmware | A dated compiler log shows the **actual** Nano ESP32 build command, dependencies and exit code 0 |
| Reassembled vehicle | Manufacturing files, dimensions, connector orientation, component positions and assembly media resolve from the same repository |
| Physical subsystem checks | Each claimed PASS has a test date, actual observation, flashed revision and identifiable supporting evidence |
| Repeated sensor behavior | ToF zone readings, IMU heading, encoder movement and UART message data can be inspected beyond source constants |
| Obstacle and Open operation | At least one modular-version run is linked to its video, mode, layout, hardware revision and firmware SHA |
| Camera Surprise Rule | Height-dependent behavior includes known-height targets, multiple distances, both colors and the captured decision |
| GitHub links and generated reports | Chapter links resolve and generated results identify the recorded inputs used to produce them |

This final audit adds a verification trail to the design, assembly and software instructions already provided above. It keeps the filmed historical program and the modular V4.1 implementation individually reproducible while allowing each physical observation to be traced to the version that produced it.

---

## Versions and development history

| Version | Role | Main engineering differences |
| --- | --- | --- |
| **V4** | National competition robot | Earlier distance-sensing hardware, drive motor and lighter wheel rims |
| **V4.1** | Current development | Revised ToF sensing, higher-torque 50:1 drive motor and heavier steel wheel rims; fan and ESC removed |

### Version-specific GitHub commits

The GitHub history preserves earlier mechanical and software configurations, the source used during filming and the later V4.1 development checkpoints.

| Robot version | GitHub commit | Description |
| --- | --- | --- |
| **V4** | [View V4 commit](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/7e1b0b5b707511563fd0250af08e68c7288d2966) | National competition configuration, including the original drivetrain, distance sensors and wheel assemblies |
| **Filmed robot software** | [Filming firmware commit `de34faf`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) | Team-identified source revision used for the real Open and Obstacle Challenge recording; Arduino entry point `src/HLR_FE/HLR_FE.ino` |
| **V4.1 development checkpoint** | [View historical commit](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/fee1a8440b3251d7454f14fb1ae3a48e173f498c) | Electronics manufacturing-file update from the V4.1 development period |

The filmed robot used the earlier `src/HLR_FE/` sketch. The later V4.1 source separates the Open and Obstacle Challenge routines in `src/HLR_FE_Separated/`, reflecting the changes in sensor handling and program structure.

### Filming firmware and modular revision

The team identifies [`de34faf039a70443361197de540879f212e3de81`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) as the code revision running in the [real Open and Obstacle Challenge footage](https://www.youtube.com/watch?v=bg92ulHXomw). That snapshot contains `src/HLR_FE/HLR_FE.ino`. Its source, camera program and mechanical/electrical documentation are accessible as a fixed historical Git state.

The later source snapshot [`045c57e7388c08f9281642a864de0ba3b099c0df`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) contains `src/HLR_FE_Separated/`, which moves Open and Final behavior into their respective files and uses the four-matrix ToF interface. It is the basis of the modular configuration and calibration references in this chapter. This distinction keeps the historical demonstration and newer implementation individually traceable.

| Release resource | Reference |
| --- | --- |
| Filmed source and matching historical files | [Commit `de34faf`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) |
| Modular V4.1 controller and project snapshot | [Commit `045c57e`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) |
| Assembled vehicle | [Assembly video](https://youtu.be/HSzWATB6nWQ) and [robot photographs](../v-photos/) |
| Real-world autonomous operation | [Open and Obstacle driving video](https://www.youtube.com/watch?v=bg92ulHXomw) |
| Physical sensor and control references | [Power and sensors](03-power-sensors.md), [mobility](02-mobility.md) and [systems](05-systems.md) |
