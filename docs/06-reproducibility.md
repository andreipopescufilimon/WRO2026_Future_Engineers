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
| Fully reproducible vehicle | [Parts and manufacturing](#parts-and-manufacturing), [Building and flashing](#building-and-flashing) | Components, CAD, PCB production files, assembly demonstration and setup procedures |
| Clear project structure | [Repository structure](#repository-structure) | Separate mechanical, electronic, software, test and documentation directories |
| CAD, code and wiring | [Repository structure](#repository-structure), [Parts and manufacturing](#parts-and-manufacturing) | STEP models, electrical files, Arduino source and OpenMV source |
| Documented testing workflow | [Testing workflow](#testing-workflow) | Isolated tests, live diagnostics, expected behavior and full-field validation |
| Meaningful development history | [Versions and development history](#versions-and-development-history) | V4 and V4.1 differences, older CAD, project-wide Git history and revision matching |

Our [GitHub history](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commits/main/) covers the **full WRO 2026 repository**, including software, CAD, electrical design and documentation updates, rather than a V4.1-only series of commits. The project has undergone many iterations throughout the season. Old mechanical revisions are kept in [`3D-models/old-3D-models/`](../3D-models/old-3D-models/) so the evolution of the design can be inspected without mistaking previous components for current ones.

### Evidence from the assembled vehicle

We built the V4.1 vehicle with custom PCB assemblies, documented its components and recorded the assembly and autonomous-driving demonstrations. The photographs and videos show the physical vehicle, while the firmware, CAD and manufacturing files explain how to build the same system.

| Evidence | Recorded result | Reference |
| --- | --- | --- |
| Mechanical and electrical assembly | The built vehicle and its PCB, drivetrain, steering and sensor placement are documented in photographs and an assembly recording | [Assembly video](https://youtu.be/HSzWATB6nWQ) · [Vehicle photographs](../v-photos/) |
| Autonomous driving | Our published video documents the physical robot driving in both Open and Obstacle challenges | [Driving video](https://www.youtube.com/watch?v=bg92ulHXomw) · [Video description](../video/video.md) |
| Camera perception | An RT1062 frame captures red and green detections on the actual vision interface | [Captured camera view](docs-media/v41-openmv-RT1062-camera-view.png) |
| Electronic and mechanical release | The source repository includes STEP models, PCB manufacturing files, the Arduino V4.1 sketch and the RT1062 script | [Mechanical CAD](../3D-models/) · [PCB files](../electrical-schematics/) · [Controller source](../src/HLR_FE_Separated/) · [Camera source](../src/camera.py) |
| Source verification | Ten static source/document checks and the numerical-analysis unit tests have recorded results; these checks are separate from a physical driving test | [Source preflight](validation/evidence/static-preflight-results.md) · [Analysis test output](validation/evidence/analysis-tests.txt) |

The driving recording is **physical demonstration evidence**. A separate, dated record is required to establish exactly which Git commit and installed software versions produced that demonstration.

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
| [`src/camera.py`](../src/camera.py) | Camera-side Python source; ensure the selected revision targets OpenMV RT1062 |
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

The separate programs in [`src/tests/`](../src/tests/) are intended for testing one function at a time. They are not replacements for the complete driving program.

## Parts and manufacturing

### Main components

The following table identifies the principal assemblies used by V4.1. Exact part variants, quantities and manufacturing details must agree with the [electrical schematics](../electrical-schematics/), [mechanical CAD](../3D-models/) and [power and sensor documentation](03-power-sensors.md). This is a subsystem-level list, not a claim that every screw and electronic component is listed here.

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

### Mechanical CAD and assembly reference

The STEP files in [`3D-models/`](../3D-models/) can be inspected in compatible CAD software to understand component geometry and how the assemblies fit together. The [technical drawings](technical-drawings/) provide dimensions, while the vehicle [photographs](../v-photos/) show the assembled orientation and cable routing.

The principal assembly reference is our recorded demonstration:

**[Watch the HyperLine Robotics assembly video](https://youtu.be/HSzWATB6nWQ).**

### PCB fabrication and electrical preparation

The electronics are organized into three manufacturing groups:

1. [Main PCB](../electrical-schematics/Main%20PCB/production-files/), which also serves as the structural chassis.
2. [IMU PCB](../electrical-schematics/IMU/production-files/) for inertial measurements.
3. [ToF sensor PCB](../electrical-schematics/TOF%20sensors/production-files/) for distance sensing.

Fabricate each board using the corresponding production package and populate it according to its schematic and board revision. The electronic files and [chapter 03](03-power-sensors.md) should be consulted together because board fabrication files do not, by themselves, describe every connection between separate modules.

Before applying battery power, inspect solder joints, power polarity, connector orientation and continuity. Check that there is no unintended short between supply rails and ground. Confirm the expected regulated voltages before connecting sensitive modules. Disconnect power before modifying wiring.

---

## Building and flashing

### 1. Obtain one matching project revision

Clone the project or use GitHub's ZIP download:

```bash
git clone https://github.com/andreipopescufilimon/WRO2026_Future_Engineers.git
cd WRO2026_Future_Engineers
```

The [GitHub commit history](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commits/main/) records the development of the **whole repository**, not just the V4.1 robot. For a reproduction attempt, use CAD, firmware and PCB files from a **single compatible revision**. Avoid combining old V4 models with V4.1 electronic and software revisions unless the relevant changes have been checked.

To record the exact revision used on a reproduced build, run:

```bash
git rev-parse HEAD
```

Keep this commit identifier with your test results. One documented, publicly accessible source snapshot is commit [`045c57e7388c08f9281642a864de0ba3b099c0df`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df), inspected on 10 November 2026. It identifies repository contents at a fixed point, **not** by itself the exact controller binary installed for the driving video. To reproduce that published snapshot, use `git checkout 045c57e7388c08f9281642a864de0ba3b099c0df` after cloning.

### 2. Assemble the mechanics

The [physical functional-check ledger](validation/physical/SUMMARY.md#subsystem-functional-test-outcomes) is generated only from dated hardware observations.

The [assembly video](https://youtu.be/HSzWATB6nWQ) gives the visual sequence. The following checks explain what must be confirmed at each stage:

1. **Main chassis:** Prepare the fabricated structural PCB and attach its mechanical supports using the corresponding [STEP models](../3D-models/) and [drawings](technical-drawings/) for positioning.
2. **Rear drivetrain:** Install the drive motor, its gear transmission and the differential. Rotate the rear axle by hand to confirm that gears mesh and there is no interference or excessive binding.
3. **Rear wheels:** Fit the wheel assemblies and verify that each wheel is secured, aligned and free to rotate as intended by the differential.
4. **Front steering:** Install the hubs and parallel linkage. Center the steering servo using the steering test before fixing the servo horn, then check both steering directions and both mechanical limits.
5. **Battery and supports:** Mount the battery securely so it cannot shift into the drivetrain or steering linkage. Check clearance against the CAD model.
6. **Vision and distance sensing:** Install the OpenMV RT1062 and ToF modules in their designated mounts. Check camera orientation, field of view and unobstructed sensor openings.
7. **Final inspection:** Route and secure cables clear of gears and steering parts. Confirm that all wheel and steering movements remain unobstructed before powering the vehicle.

The servo's calibrated neutral position, linkage geometry, wheelbase and other dimensions are described in [02. Mobility](02-mobility.md) and the [technical drawings](technical-drawings/). Mechanical alignment must be checked before software tuning because assembly errors can appear as control errors on the track.

### 3. Prepare the software environment

The Nano ESP32 and OpenMV RT1062 use different development environments. The Open and Obstacle routines use a common V4.1 hardware layer but separate round implementations.

| Device | Software | Source |
| --- | --- | --- |
| Arduino Nano ESP32 | [Arduino IDE 2](https://www.arduino.cc/en/software), with the appropriate Nano ESP32 board package | [`src/HLR_FE_Separated/`](../src/HLR_FE_Separated/) |
| OpenMV RT1062 | [OpenMV IDE](https://openmv.io/pages/download) and compatible camera firmware | [`src/camera.py`](../src/camera.py) |

The source files identify the board and the libraries required by the V4.1 program. The table below is the environment identification record for reproducing the physical build. The version numbers must come from the actual development computer and RT1062, rather than a library website or a guess. An archived, successful compilation report has not yet been published with the repository.

| Dependency | Role / exact identification | Archived installed version |
| --- | --- | --- |
| Arduino IDE | Desktop environment used to compile and upload the Nano ESP32 sketch | Not archived |
| Arduino Nano ESP32 board package | Board platform, provider, installed core and board identifier (FQBN) | Not archived |
| `ESP32Encoder` | Library providing `ESP32Encoder.h`; record installed library's name and author | Not archived |
| `ESP32Servo` | Library providing `ESP32Servo.h`; record installed library's name and author | Not archived |
| SparkFun VL53L5CX Arduino Library | Library providing `SparkFun_VL53L5CX_Library.h`, used by `tof.ino` | Not archived |
| BMI088 library | Exact library name, author or repository, and version or commit providing `BMI088.h` with the methods used in `gyro.ino` | Not archived |
| OpenMV IDE | Desktop IDE used to save and test the camera script | Not archived |
| OpenMV RT1062 firmware | Firmware running on the camera when `src/camera.py` was validated | Not archived |

The TCS34725 is accessed through `Wire` transactions in `functions.ino` and does not require a separate TCS34725 Arduino library. `Wire` is supplied by the selected Arduino board platform. The libraries listed above must use the interfaces called by the published V4.1 code; in particular, **`BMI088.h` is not a unique library identifier**, so its author and source revision need to be recorded.

**Recording the versions used:**

1. Open **Help → About** in both **Arduino IDE** and **OpenMV IDE**. Record the exact installed desktop application versions in the dependency table, rather than the version currently offered on their websites.
2. In Arduino IDE **Boards Manager**, find the installed platform for **Arduino Nano ESP32** and record its **provider, package version and board identifier**. Then open **Library Manager** and record the installed **name, author and version** for `ESP32Encoder`, `ESP32Servo`, SparkFun VL53L5CX and the BMI088 library. If `BMI088.h` was installed manually, record its repository URL, local installation path and exact commit or tagged version. Where multiple copies exist, identify the one selected by the compiler.
3. If **Arduino CLI** is installed, save the results of these four commands together with the build record:

   ```bash
   arduino-cli version
   arduino-cli core list
   arduino-cli lib list
   arduino-cli board list
   ```

   These commands report the CLI version, installed board packages, installed libraries and connected boards. They do **not** establish which copies of duplicate libraries the compiler selected. Keep the verbose compilation output to confirm the actual dependency paths. Arduino CLI is optional if the same information is captured from the IDE.
4. Connect the **OpenMV RT1062** in OpenMV IDE and run the following directly on the camera to record the MicroPython/OpenMV firmware build:

   ```python
   import os
   import sys

   print(os.uname())
   print(sys.implementation)
   ```

   Save the console output as `docs/validation/evidence/local-build/openmv-firmware.txt` and compare it with the firmware information shown in the IDE's camera connection or firmware information panel. Record the OpenMV IDE version separately because the desktop IDE version and camera firmware version are not necessarily the same.
5. Compile the **exact published** `src/HLR_FE_Separated/HLR_FE_Separated.ino` sketch. Save the complete **Verify** output, including selected board, dependency paths, warnings, final result and build date. Run `git rev-parse HEAD` and `git status --short` in the repository, so the log identifies the source revision and any uncommitted changes. Enter the result in the build-verification table below. Update the dependency record whenever the board core, libraries or camera firmware change.

Keep the version record, build log and RT1062 firmware output under `docs/validation/evidence/local-build/`. Publish them only after checking for personal file paths or other private information in the logs. A listed version or captured console output is documentation of the environment, not proof of a successful build unless the matching compiler output also shows success.

For the Windows development computer, [`scripts/capture_build_environment.ps1`](../scripts/capture_build_environment.ps1) collects `arduino-cli` version, installed board cores, installed libraries, current Git commit, firmware checksums and an Arduino compile log. Run it from PowerShell with the appropriate Nano ESP32 FQBN after installing the actual dependencies:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\capture_build_environment.ps1 -FQBN arduino:esp32:nano_nora
```

The captured output is saved under `docs/validation/evidence/local-build/`. Review the compilation exit code and full output before recording a successful build. The capture script cannot recover a previously used Arduino IDE version or camera firmware that was never recorded. Keep the OpenMV firmware build (`os.uname()`) and a saved camera power-cycle test alongside that folder.


### V4.1 build-verification record

The repository already contains the physical assembly documentation and the full separated V4.1 source code. The published evidence is distinguished below from the remaining information that can only be obtained from the original programming computer.

| Item | Verified or documented information | Evidence |
| --- | --- | --- |
| Published source revision | Repository snapshot `045c57e7388c08f9281642a864de0ba3b099c0df` (10 November 2026); not asserted to be the version used in the driving recording | [Snapshot](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) |
| V4.1 controller source | `src/HLR_FE_Separated/HLR_FE_Separated.ino` with `Open.ino`, `Final.ino` and the hardware-control tabs | [Controller firmware](../src/HLR_FE_Separated/) |
| Source consistency | Static source and local-file checks have recorded PASS results; these do not compile the ESP32 program | [Static preflight](validation/evidence/static-preflight-results.md) · [Checksums](validation/evidence/source-sha256.json) |
| RT1062 image-processing demonstration | Captured camera frame shows red and green object detections | [Camera view](docs-media/v41-openmv-RT1062-camera-view.png) |
| Physical autonomous driving | We published our real-world driving demonstration for both Open and Obstacle modes | [Driving video](https://www.youtube.com/watch?v=bg92ulHXomw) |
| Successful Arduino compile, upload and dependency versions | The required source and capture script are available; a compiler log and installed-version report from the robot's development computer are not archived | [Capture script](../scripts/capture_build_environment.ps1) · [Evidence directory](validation/evidence/README.md) |
| Firmware-to-run linkage | We have published real driving footage, but the source revision actually flashed for that recording has not been identified in a dated build record | [Driving video](https://www.youtube.com/watch?v=bg92ulHXomw) · [Physical test records](validation/physical/SUMMARY.md) |

To complete the build record, save one successful Arduino **Verify** output, its `git rev-parse HEAD`, the installed dependencies and the RT1062 firmware information. The published [environment-capture script](../scripts/capture_build_environment.ps1) gathers most of the Arduino information. This final step does not change the existing assembly video, driving evidence or firmware design; it establishes the exact environment in which they can be reproduced.

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
9. Open **Serial Monitor** at **115200 baud** for controller diagnostics. The RT1062 UART link uses **19,200 baud**, which is a separate serial interface. Verify the four ToF sensor addresses at startup: front `0x30`, left `0x31`, right `0x32`, back `0x33`. Save the successful Verify and Upload output together with the installed version list in the [V4.1 build-verification record](#v41-build-verification-record).

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

## Running the vehicle

After programming both devices, disconnect programming cables as appropriate for the power setup, mount and secure the battery, and switch on the robot. Allow the Nano ESP32, IMU, ToF sensors and RT1062 to initialize. Check basic actuator direction and sensor operation before attempting an autonomous run.

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

The settings page provides runtime adjustments for the driving and maneuvering parameters exposed by the firmware. For controlled experiments, we start conservatively, keep the same track setup and change **one parameter at a time**. Each setting should be recorded with the test run so that a performance difference can be associated with a specific change.

<div align="center">
  <img src="docs-media/robot-adjust-1.jpeg" alt="Robot web interface configuration panel, screenshot 1" width="32%">
  <img src="docs-media/robot-adjust-2.jpeg" alt="Robot web interface configuration panel, screenshot 2" width="32%">
  <img src="docs-media/robot-adjust-3.jpeg" alt="Robot web interface configuration panel, screenshot 3" width="32%">
</div>

*Figures 6.3, 6.4 and 6.5. Configuration views for speed, steering and subsystem adjustments.*

---

## Testing workflow

We test the robot in three stages: individual subsystems, integrated stationary operation and full autonomous runs. This prevents a mechanical or electrical fault from being mistaken for a navigation problem.

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

Use raised wheels for the first drive and steering tests. The table intentionally describes observable pass/fail behavior rather than unsupported numerical accuracy claims. Numerical tolerances and repeatability results should come from measured tests in [05. Systems and testing](05-systems.md).

### 2. Bench verification with telemetry

Once individual components pass, run the controller and camera together while the robot remains stationary. Use the serial diagnostic output or the [optional Wi-Fi diagnostic interface](#connect-to-the-local-web-interface), if installed, to read ToF and IMU data, and use the OpenMV IDE frame buffer to verify visual detection. Also check that the camera's serial messages reach the Nano ESP32 and that the robot initializes without unexpected motion.

A useful bench sequence is to rotate the robot by hand, move targets past the distance sensors and show red and green pillars to the camera. If the physical input changes but the reported state does not, investigate the relevant subsystem before changing the autonomous strategy.

### 3. Full-field verification

With subsystem checks completed, test the robot on a field matching the competition geometry. First use conservative speeds and a repeatable layout. Verify wall following, both travel directions, cornering and stable lap completion. Then test obstacle detection, correct passing side for red and green pillars, recovery and parallel parking as applicable to the selected challenge.

For meaningful comparisons, record the same field layout, battery condition, firmware revision and speed settings. Compare repeated attempts rather than selecting only the fastest result. The recordings are collected under [`video/`](../video/), while measured results and engineering decisions are discussed in [05. Systems and testing](05-systems.md). The same test date, firmware commit, field configuration and results are included in the verification record below.

### V4.1 functional verification results

Our project provides a recorded assembly, a physical driving demonstration for both challenge configurations and an actual RT1062 detection frame. The table links those outcomes directly to the published evidence. It also identifies the hardware checks for which a dated measurement file has not yet been archived. We do not count a successful source-code check as a physical test.

| Test | Evidence from the robot or repository | Documented result |
| --- | --- | --- |
| Physical vehicle assembly | [Assembly video](https://youtu.be/HSzWATB6nWQ), [vehicle photographs](../v-photos/) | **Documented:** completed robot and assembly process shown |
| Open Challenge operation | [Recorded driving demonstration](https://www.youtube.com/watch?v=bg92ulHXomw), [video index](../video/video.md) | **Recorded:** autonomous Open-mode driving footage; firmware revision and timed lap log not linked |
| Obstacle Challenge operation | [Recorded driving demonstration](https://www.youtube.com/watch?v=bg92ulHXomw), [video index](../video/video.md) | **Recorded:** autonomous Obstacle-mode driving footage; firmware revision and scored-run record not linked |
| RT1062 red and green perception | [Captured camera screenshot](docs-media/v41-openmv-RT1062-camera-view.png) | **Captured:** red and green image detections visible; repeated-trial accuracy not reported |
| Controller firmware and source | [V4.1 sketch](../src/HLR_FE_Separated/), [static preflight](validation/evidence/static-preflight-results.md) | **Available:** source modules and consistent interface declarations; successful Arduino compiler output not archived |
| Four VL53L5CX modules | [Sensor implementation](../src/HLR_FE_Separated/tof.ino), [sensor calibration section](03-power-sensors.md#51-distance-sensor-calibration-matrix) | **Implemented:** four addresses and 4×4 ranging configuration; physical RMSE log not archived |
| BMI088 and steering/drive | [Gyro implementation](../src/HLR_FE_Separated/gyro.ino), [motor implementation](../src/HLR_FE_Separated/motors.ino), [individual tests](../src/tests/) | **Implemented:** control and dedicated test code; dated standalone drift/actuator test record not archived |
| Battery and regulated-power reliability | [Measured-data collection procedure](03-power-sensors.md#33-physical-power-measurement-plan), [physical log summary](validation/physical/SUMMARY.md) | **Documented:** electrical architecture and test procedure; physical current and 5 V transient traces not archived |

The videos and camera frame show real work with the vehicle. The separate [physical test ledger](validation/physical/SUMMARY.md) is intended for timed measurements and results associated with a specific source revision. It should not be used to infer measurements absent from the recordings.

### Final verification checklist

This checklist is for anyone building a second vehicle or performing a fresh release test. It does not replace the assembly, camera or driving evidence already linked above.

- [ ] Mechanical parts match the selected CAD revision and move freely.
- [ ] Battery polarity, regulated voltages and board connections have been checked.
- [ ] Arduino Nano ESP32 sketch compiles and starts correctly.
- [ ] RT1062 script runs independently after the camera is restarted.
- [ ] Camera-to-controller serial communication is working.
- [ ] Drive direction and steering neutral/limits are correct.
- [ ] IMU heading direction and ToF readings respond to known physical inputs.
- [ ] **Optional diagnostic build only:** robot Wi-Fi network appears and `http://192.168.4.1` loads the live dashboard.
- [ ] Driving settings and the project revision are recorded for the test.
- [ ] Autonomous behavior has been checked on the field in both challenge configurations.

---

## Versions and development history

| Version | Role | Main engineering differences |
| --- | --- | --- |
| **V4** | National competition robot | Earlier distance-sensing hardware, drive motor and lighter wheel rims |
| **V4.1** | Current development | Revised ToF sensing, higher-torque 50:1 drive motor and heavier steel wheel rims; fan and ESC removed |


### Version-specific GitHub commits

To make both versions of our robot reproducible, the following table identifies the GitHub revisions corresponding to each major hardware and software configuration. Each commit links to a historical repository snapshot; the V4.1 development checkpoint must not be mistaken for proof of the final tested firmware.

| Robot version | GitHub commit | Description |
| --- | --- | --- |
| **V4** | [View V4 commit](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/7e1b0b5b707511563fd0250af08e68c7288d2966) | National competition configuration, including the original drivetrain, distance sensors and wheel assemblies |
| **V4.1 development checkpoint** | [View historical commit](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/fee1a8440b3251d7454f14fb1ae3a48e173f498c) | A historical electronics-file change in the V4.1 development period; not evidence by itself of a compiled, tested `HLR_FE_Separated` release |

These commits provide reference points for the mechanical and electronics revisions. The V4.1 driving program is the separated `src/HLR_FE_Separated/` sketch documented above. The earlier `src/HLR_FE/` main sketch uses a different sensor and round-selection architecture, so it must not be used as the V4.1 reproduction entry point. The assembled hardware and selected firmware must be verified together before the release is treated as tested.

### Reproducible V4.1 revision

For a stable public starting point, the repository can be checked out at [`045c57e7388c08f9281642a864de0ba3b099c0df`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) (10 November 2026). This is a **published source snapshot**, with the controller sketch, OpenMV code and manufacturing references available to a reader. The earlier `fee1a844` commit is retained as a development checkpoint, not as the current firmware release.

| Release item | Reference |
| --- | --- |
| Public V4.1 source checkpoint | [Commit `045c57e`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) |
| Controller source | [`src/HLR_FE_Separated/`](../src/HLR_FE_Separated/) |
| Camera source | [`src/camera.py`](../src/camera.py), OpenMV RT1062 |
| Mechanical and PCB production files | [CAD](../3D-models/) · [Electrical files](../electrical-schematics/) |
| Real assembly demonstration | [Assembly video](https://youtu.be/HSzWATB6nWQ) |
| Real driving demonstration | [Open and Obstacle footage](https://www.youtube.com/watch?v=bg92ulHXomw) |
| Firmware integrity | [SHA-256 source manifest](validation/evidence/source-sha256.json) |
| Exact hardware-tested commit | The video has not been linked to a firmware hash or dated source record; the public checkpoint above must not be described as the proven running firmware |

A matching test record should contain the tested commit, build environment, date and corresponding driving footage. This is the remaining traceability information needed to connect the filmed physical robot to one exact reproducible release.

The complete repository history also includes earlier development checkpoints. Under the 2026 rules, check that the required milestone commits satisfy the event deadlines. Record commit links and dates rather than inferring the development timeline from a file's latest modification date.

### Automated source verification and physical test records

The repository also includes a small, independent Python validation workflow:

```bash
python scripts/repro_preflight.py
python -m unittest discover -s tests -v
python scripts/summarize_validation.py
```

The first command checks the supplied V4.1 source modules, configured camera/ToF interfaces and local documentation assets. The unit tests verify the calculation script against known arithmetic fixtures. The final command summarizes **only logged physical readings**, and report unrecorded readings as unavailable. Results and per-file hashes are saved in [`docs/validation/evidence/`](validation/evidence/), while new measurements belong in [`docs/validation/physical/`](validation/physical/).

These checks do **not** compile the controller or substitute for sensor measurements. A complete V4.1 release requires a build log from the tested Arduino environment, an OpenMV RT1062 firmware record and dated hardware runs recorded under the same Git commit.
