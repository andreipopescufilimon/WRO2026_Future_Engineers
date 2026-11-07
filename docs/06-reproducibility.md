# 06. Reproducibility

| The rubric asks for | Section | Evidence |
| --- | --- | --- |
| Fully reproducible vehicle | [Building and flashing](#building-and-flashing) | Mechanical step by step assembly video, CAD, electronics production files, Arduino Nano ESP32 and OpenMV N6 installation |
| Clear project structure | [Repository structure](#repository-structure) | Directories for mechanical parts, electronics, firmware, photos, tests and technical journal |
| CAD, code and wiring | [Repository structure](#repository-structure) | Published STEP components, main PCB/IMU/ToF manufacturing files and firmware sources; electrical description in [chapter 03](03-power-sensors.md) |
| Documented test workflow | [Testing workflow](#testing-workflow) | Isolated subsystem tests, on-vehicle telemetry and on-track verification |
| Meaningful development history | [Versions](#versions) | V4 national robot versus V4.1 current robot, older models and repository commits |

The robot can be examined as separate 3 subsystems: mechanical, electronic and software. Our current V4.1 design uses the **Arduino Nano ESP32** for vehicle control and the **OpenMV N6** for vision. V4.1 introduces improved, faster ToF sensing, a higher-torque drive motor and heavier wheel rims compared with V4, the national competition version. Mechanical reasoning is in [chapter 02](02-mobility.md), electronics in [chapter 03](03-power-sensors.md), driving software in [chapter 04](04-software.md), and system testing in [chapter 05](05-systems.md).

---

## Repository structure

All paths below are relative to the repository root. The documentation chapters are directly under `docs/`.

| Path | Content |
| --- | --- |
| [`README.md`](../README.md) | Main repository entry, project introduction and navigation |
| [`docs/`](.) | Six journal chapters, supporting diagrams, technical drawings and figures |
| [`docs/docs-media/`](docs-media/) | Figures and screenshots referenced for the journal |
| [`docs/technical-drawings/`](technical-drawings/) | Mechanical dimensions and component drawings |
| [`3D-models/`](../3D-models/) | STEP files for battery, drive train, steering, wheels and sensor mounts |
| [`3D-models/old-3D-models/`](../3D-models/old-3D-models/) | Earlier mechanical designs retained for comparison |
| [`electrical-schematics/`](../electrical-schematics/) | PCB documentation and manufacturing outputs |
| [`electrical-schematics/Main PCB/production-files/`](../electrical-schematics/Main%20PCB/production-files/) | Main structural/electrical PCB manufacturing data |
| [`electrical-schematics/IMU/production-files/`](../electrical-schematics/IMU/production-files/) | IMU board production files |
| [`electrical-schematics/TOF sensors/production-files/`](../electrical-schematics/TOF%20sensors/production-files/) | ToF sensor PCB production files |
| [`src/HLR_FE/`](../src/HLR_FE/) | Arduino Nano ESP32 vehicle firmware and supporting tabs |
| [`src/camera.py`](../src/camera.py) | Camera-side Python source in the repository; the N6 must use its matching updated revision |
| [`src/tests/`](../src/tests/) | Independent tests for actuators, sensors and communication |
| [`v-photos/`](../v-photos/) | Top, bottom, front, rear, left and right vehicle photographs |
| [`t-photos/`](../t-photos/) | Team photograph |
| [`video/`](../video/) | Driving recordings and challenge videos |
| [`other/`](../other/) | Additional development resources |

The Arduino sketch is split into tabs that the Arduino IDE builds together:

| File | Responsibility |
| --- | --- |
| [`HLR_FE.ino`](../src/HLR_FE/HLR_FE.ino) | Main setup, robot state machine, configuration and run logic |
| [`functions.ino`](../src/HLR_FE/functions.ino) | Movement sequences, distance handling and shared routines |
| [`gyro-code.ino`](../src/HLR_FE/gyro-code.ino) | IMU acquisition and heading estimation |
| [`motors-code.ino`](../src/HLR_FE/motors-code.ino) | Motor, encoder and steering control |

The [`src/tests/`](../src/tests/) directory groups the **motor test**, **gyro test**, **servo steering**, **straight-on-gyro**, and **Arduino/OpenMV serial communication** experiments. They let a reproduced robot be checked before the full autonomous program is run.

## Building and flashing

**Getting the sources.** Clone the repository or download its ZIP from GitHub. Use a single commit for all firmware, CAD and wiring files so that different revisions are not mixed.

```bash
git clone https://github.com/andreipopescufilimon/WRO2026_Future_Engineers.git
cd WRO2026_Future_Engineers
```

**Mechanical assembly.** The individual STEP models in [`3D-models/`](../3D-models/) show the mechanical parts and their geometry. The full assembly video demonstrates how the PCB chassis, rear drivetrain, wheel hubs, steering linkage, camera support, sensor boards and battery mount fit together:

**[Watch our robot assembly video on YouTube](https://youtu.be/HSzWATB6nWQ).**

The assembly proceeds from the main PCB chassis to the drivetrain and differential, rear wheels, front steering hubs and steering bar, centered steering servo, battery holder, OpenMV N6 mounting and ToF boards. Before fastening the final screws, check free wheel rotation, gear engagement and the full steering range without interference. Mount the battery securely in the designated support.

**Electronic manufacturing.** The PCB manufacturing files are organized separately for the [main board](../electrical-schematics/Main%20PCB/production-files/), [IMU board](../electrical-schematics/IMU/production-files/) and [ToF boards](../electrical-schematics/TOF%20sensors/production-files/). Populate each board according to its matching schematic and board revision. Before the first connection of the battery, check the power-input polarity, absence of supply-to-ground shorts and the regulated logic voltage. Keep the wheels lifted during initial motor tests. Wiring and power architecture are described in [chapter 03](03-power-sensors.md).

### Flashing the Arduino Nano ESP32

The Nano ESP32 runs the motion-control program. We use the Arduino IDE to compile the sketch from [`src/HLR_FE/HLR_FE.ino`](../src/HLR_FE/HLR_FE.ino). The other `.ino` files in the same folder are compiled as part of the sketch; they must remain together.

1. Install [Arduino IDE 2](https://www.arduino.cc/en/software). In **Boards Manager**, install the board package supporting **Arduino Nano ESP32**.
2. Connect the Nano ESP32 by a **USB-C data cable**. Select **Tools → Board → Arduino Nano ESP32**, then select the USB port under **Tools → Port**.
3. Open `src/HLR_FE/HLR_FE.ino` and confirm that the `functions.ino`, `gyro-code.ino` and `motors-code.ino` tabs are visible.
4. Install any libraries requested by the selected sketch in **Library Manager**. The publicly available legacy sketch uses `ESP32Encoder`, `ESP32Servo`, `SoftwareSerial` and a `BMI088.h` library in addition to Arduino's standard headers. Match the BMI088 library to the source version you are compiling. You can find the same library versions we use in `../src/libraries/`
5. Set the appropriate challenge mode and inspect the steering calibration, motor direction, encoder parameters and speed values before uploading. Click **Verify** to compile.
6. With the wheels off the floor, click **Upload**. After the IDE reports a successful upload, restart the board and inspect its startup output in **Serial Monitor** at the baud rate specified by the firmware (it uses `115200`).

### Flashing the OpenMV N6

The camera performs image acquisition and color/region-of-interest processing independently of the main controller. Its source entry point is [`src/camera.py`](../src/camera.py), with the board-specific **OpenMV N6** version required for V4.1.

1. Install [OpenMV IDE](https://openmv.io/pages/download) and connect the **N6** with a USB data cable.
2. Select **Connect** in OpenMV IDE, choose the detected N6 and complete any supported firmware initialization if prompted.
3. Open the **N6-compatible** `camera.py` and click **Run**. Inspect the frame buffer while presenting red and green obstacles and track markings in front of the mounted camera.
4. Verify that the expected detection overlays appear in the correct regions of interest and that the serial messages reach the Nano ESP32. Camera transmit must connect to the correct Nano receive pin; grounds must be common and the baud rates must match.
5. Once the script operates correctly, use **Tools → Save open script to OpenMV Cam** in OpenMV IDE, so the camera starts the program from its on-device `main.py` after reboot. Restart the camera and check that the vision program starts without the IDE.

<img src="docs-media/openmv-n6-camera-view.png"
       alt="Camera view"
       width="32%">

*Figure 6.1. OpenMV N6 live camera output in OpenMV IDE. The frame buffer is useful for checking image orientation, ROI placement, color thresholds and obstacle detection before driving.*

## Running the vehicle

After both devices are programmed, disconnect the programming cables as required by the robot's power setup, secure the battery and switch on the vehicle. Allow the Nano ESP32, IMU, ToF modules and OpenMV N6 to initialize. Confirm that steering responds correctly and that the camera sends valid observations before placing the robot on the competition field. 

For configuration and diagnostics, the V4.1 robot exposes a **local Wi-Fi access point with a built-in web interface**. To use it:

1. Power the robot with its V4.1 Wi-Fi-enabled firmware installed.
2. On a computer or phone, open **Wi-Fi settings** and join the wireless network broadcast by the robot, using the network credentials configured in the controller firmware.
3. While connected to the robot's network, open **[http://192.168.4.1](http://192.168.4.1)** in a browser.
4. The dashboard shows live telemetry and the available configuration controls. The phone or computer does not need external internet access to reach this local address.

The web interface is particularly useful during bench checks and tuning because it avoids repeatedly connecting a USB cable or rebuilding firmware for every driving parameter adjustment. The robot is tested stationary first and then at reduced speed on the track.

### Evaluation in the web dashboard

The dashboard is our equivalent of a live diagnostic view: it exposes the internal sensor readings and driving parameters while the controller is operating. We use it to compare what the robot *believes* is happening against its actual behavior.

**Distance sensors.** The live ToF readouts let us check whether the front, side and rear sensors respond to walls and nearby obstacles. During testing we move an object toward and away from each sensor and compare the displayed distance to a ruler. A stuck or implausible value is easier to identify here than by observing the robot's movement alone.

**IMU.** The IMU view lets us inspect the reported orientation and angular motion. With the robot stationary, we look for large offsets or drift. When the robot is rotated manually, the heading change should follow the actual rotation with the expected sign.

<img src="docs-media/telemetry-debug.jpeg"
       alt="Robot telemetry"
       width="32%">

*Figure 6.1. IMU debugging view used to check motion readings and heading behavior.*

**Speed and tuning.** The settings interface lets us adjust the available speed parameters for driving and maneuvers without editing the source for every iteration. We begin at a conservative setting, test a repeatable route and then change only one parameter at a time. A setting shown in the web interface should be treated as the active session's configuration unless the firmware explicitly confirms that it is saved persistently.



<div align="center">
  <img src="docs-media/robot-adjust-1.jpeg"
       alt="Robot configuration controls 1"
       width="32%">
  <img src="docs-media/robot-adjust-2.jpeg"
       alt="Robot configuration controls 2"
       width="32%">
  <img src="docs-media/robot-adjust-3.jpeg"
       alt="Robot configuration controls 3"
       width="32%">
</div>



*Figures 6.5, 6.6, 6.7. Runtime configuration view used during speed tuning and subsystem checks.*


## Testing workflow

We validate each change at three levels, from the smallest subsystem to the complete challenge. The test sources are organized under [`src/tests/`](../src/tests/); system-level behavior and results are discussed in [chapter 05](05-systems.md).

**1. Independent subsystem tests.** We begin with the dedicated examples: [`motor_test`](../src/tests/motor_test/), [`servo_steering`](../src/tests/servo_steering/), [`gyro_test`](../src/tests/gyro_test/), [`move_straight_on_gyro`](../src/tests/move_straight_on_gyro/) and [`arduino_openmv_serial_communication`](../src/tests/arduino_openmv_serial_communication/). These isolate drive direction, steering response, IMU sign and drift, straight-line correction and the communication channel. The motors are first tested with wheels raised; steering is checked at center and both limits. Each sensor is checked without simultaneously changing multiple parameters.

**2. Bench checks with telemetry.** The V4.1 web interface at `192.168.4.1` provides the live ToF values and IMU data needed to compare readings with known distances and manual rotations. The OpenMV IDE frame buffer separately verifies scene acquisition and color detection. Only after these checks do we tune speed, steering limits and obstacle handling with the robot moving.

**3. Full-field driving.** We run the Open Challenge and Obstacle Challenge on a correctly sized field, with attention to lap direction, wall clearance, red/green obstacle response, recovery after maneuvers and parking. We compare runs using the same track configuration and speed settings before changing a parameter. The resulting driving videos are collected under [`video/`](../video/), and external photographs of the assembled vehicle under [`v-photos/`](../v-photos/).

The separation between these levels matters. A robot may turn correctly on the bench but still drift on the field because of wheel grip, steering asymmetry, inertia or incorrect interpretation of sensor observations. For that reason we check the mechanics, controller and visual detections together, rather than interpreting one successful lap as proof of a reliable setup.

## Versions

| Version | Role | Engineering difference |
| --- | --- | --- |
| **V4** | National winning competition robot | Previous distance-sensing hardware, previous drive motor and lighter wheel rims |
| **V4.1** | Current robot | Faster/improved ToF sensors, higher-torque drive motor and heavier wheel rims; no impeller or ESC |

The [repository commit history](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commits/main/) records changes to code, documentation and design files. Earlier mechanical models remain in [`3D-models/old-3D-models/`](../3D-models/old-3D-models/) so that previous designs can be inspected without confusing them with the current assembly. No particular commit count or release tag is claimed here without a corresponding verified GitHub reference.

*Note: For reproducibility, **the matching set of V4.1 files is essential**: main-controller firmware, N6-compatible camera code, actual PCB/sensor revision and chassis CAD must represent the same physical vehicle.*
