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

The repository preserves the filmed program, a historical modular V4.1 checkpoint and the uploaded World Final implementation. They are **different software revisions** and should not be mixed when assembling a complete system.

| Reproduction target | Fixed Git revision | Controller entry point | What it represents |
| --- | --- | --- | --- |
| **Filmed vehicle** | [`de34faf039a70443361197de540879f212e3de81`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) | [`src/HLR_FE/HLR_FE.ino`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/de34faf039a70443361197de540879f212e3de81/src/HLR_FE/HLR_FE.ino) | Source revision identified by the team as running during the [Open and Obstacle driving video](https://www.youtube.com/watch?v=bg92ulHXomw) |
| **Modular V4.1 source** | [`045c57e7388c08f9281642a864de0ba3b099c0df`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) | [`src/HLR_FE_World_Final/HLR_FE_World_Final.ino`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/045c57e7388c08f9281642a864de0ba3b099c0df/src/HLR_FE_Separated/HLR_FE_Separated.ino) | Later source revision with separate Open and Final files and four VL53L5CX interfaces |

The current uploaded World Final source is [`src/HLR_FE_World_Final/HLR_FE_World_Final.ino`](../src/HLR_FE_World_Final/HLR_FE_World_Final.ino). Record the full Git SHA of the actual flashed release rather than attributing current telemetry to the earlier modular checkpoint.

**Practical build sequence:** select one revision, fabricate its matching mechanical/electrical configuration, assemble the vehicle, install its controller and camera programs, perform [calibration](#calibration-and-setup), and follow the [testing workflow](#testing-workflow). The chassis, sensor wiring and firmware are treated as one system, not interchangeable parts from different development stages.

### Evidence from the assembled vehicle

Our documentation covers the physical vehicle, its custom PCB assemblies, the mechanical construction and autonomous-driving demonstrations. The photographs, assembly recording, source code and manufacturing files show the development of the robot from hardware design to operation on the field.

| Evidence | Recorded result | Reference |
| --- | --- | --- |
| Mechanical and electrical assembly | The built vehicle and its PCB, drivetrain, steering and sensor placement are documented in photographs and an assembly recording | [Assembly video](https://youtu.be/HSzWATB6nWQ) · [Vehicle photographs](../v-photos/) |
| Autonomous driving | Our published video documents the physical robot in both Open and Obstacle challenges; the team identifies the filming software as commit `de34faf` | [Driving video](https://www.youtube.com/watch?v=bg92ulHXomw) · [Filming firmware commit `de34faf`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) |
| Camera perception | An RT1062 frame captures red and green detections on the actual vision interface | [Captured camera view](docs-media/v41-openmv-RT1062-camera-view.png) |
| Electronic and mechanical release | The source repository includes STEP models, PCB manufacturing files, the Arduino V4.1 sketch and the RT1062 script | [Mechanical CAD](../3D-models/) · [PCB files](../electrical-schematics/) · [Controller source](../src/HLR_FE_World_Final/) · [Camera source](../src/camera.py) |

**Firmware used during filming:** according to the team, the driving video was recorded with Git commit [`de34faf039a70443361197de540879f212e3de81`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81). The associated Arduino program is [`src/HLR_FE/HLR_FE.ino`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/de34faf039a70443361197de540879f212e3de81/src/HLR_FE/HLR_FE.ino). The newer [`src/HLR_FE_World_Final/`](../src/HLR_FE_World_Final/) firmware is documented separately because its software architecture differs from the filmed version.

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
| [`src/HLR_FE_World_Final/`](../src/HLR_FE_World_Final/) | V4.1 Arduino Nano ESP32 Open/Final firmware and supporting `.ino` files |
| [`src/camera.py`](../src/camera.py) | OpenMV RT1062 camera-side Python source |
| [`src/HLR_FE_Separated/README.txt`](../src/README.md) | Firmware setup and mode-selection reference |
| [`src/tests/`](../src/tests/) | Motor, steering, IMU, straight-driving and communication tests |
| [`v-photos/`](../v-photos/) | Vehicle photographs from all sides |
| [`t-photos/`](../t-photos/) | Team photograph |
| [`video/`](../video/) | Driving and challenge recordings |
| [`other/`](../other/) | Additional development resources |

The Arduino sketch is divided into multiple tabs, which the Arduino IDE compiles together. Keep them in the same `HLR_FE_World_Final` directory.

| File | Responsibility |
| --- | --- |
| [`HLR_FE_World_Final.ino`](../src/HLR_FE_World_Final/HLR_FE_World_Final.ino) | Main initialization, V4.1 mode selection, hardware pins, startup and dispatch |
| [`functions.ino`](../src/HLR_FE_World_Final/functions.ino) | Shared movement and floor-marker routines |
| [`gyro.ino`](../src/HLR_FE_World_Final/gyro.ino) | BMI088 acquisition and heading calculations |
| [`motors.ino`](../src/HLR_FE_World_Final/motors.ino) | Motor, encoder and steering control |
| [`tof.ino`](../src/HLR_FE_World_Final/tof.ino) | Four VL53L5CX devices and address sequencing |
| [`Open.ino`](../src/HLR_FE_World_Final/Open.ino) | Open Challenge navigation and transitions |
| [`Final.ino`](../src/HLR_FE_World_Final/Final.ino) | Obstacle Challenge navigation, colored pillars and parking |

The independent subsystem examples in [`src/tests/`](../src/tests/) cover individual functions; the complete robot program is in `src/HLR_FE_World_Final/`.

## Parts and manufacturing

### Main components

The main V4.1 assemblies are listed below. Component variants and manufacturing details are documented in the [electrical schematics](../electrical-schematics/), [mechanical CAD](../3D-models/) and [power and sensor chapter](03-power-sensors.md).

| Assembly | Component or source | Reproduction reference |
| --- | --- | --- |
| Main controller | Arduino Nano ESP32 | [Controller firmware](../src/HLR_FE_World_Final/) |
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

### Ordering custom PCBs and assembly with JLCPCB

The following step-by-step procedure is part of our V4.1 reproduction guide. It uses the three manufacturing packages already provided in this repository. It explains how another team can submit the files to JLCPCB, choose fabrication or factory assembly, and inspect the delivered electronics. **The presence of production files and an ordering procedure is not evidence that every board, factory setting, or electrical check has been independently verified.**

#### 1. Download the correct manufacturing files

Three distinct boards are used, with **four identical ToF sensor PCBs** installed on the robot. Manufacture each board type separately. All the available exported packages are dated November 7, 2026.

| Board | Installed quantity | Gerber ZIP | Bill of materials (BOM) | Pick-and-Place (CPL) |
|---|---:|---|---|---|
| **Main PCB / structural chassis** | 1 | [Gerber_MainPCB_2026-11-07.zip](../electrical-schematics/Main%20PCB/production-files/Gerber_MainPCB_2026-11-07.zip) | [BOM_MainPCB_2026-11-07.xlsx](../electrical-schematics/Main%20PCB/production-files/BOM_MainPCB_2026-11-07.xlsx) | [PickAndPlace_MainPCB_2026_11_07.xlsx](../electrical-schematics/Main%20PCB/production-files/PickAndPlace_MainPCB_2026_11_07.xlsx) |
| **BMI088 IMU board** | 1 | [Gerber_IMU_2026-11-07.zip](../electrical-schematics/IMU/production-files/Gerber_IMU_2026-11-07.zip) | [BOM_IMU_2026-11-07.xlsx](../electrical-schematics/IMU/production-files/BOM_IMU_2026-11-07.xlsx) | [PickAndPlace_IMU_2026_11_07.xlsx](../electrical-schematics/IMU/production-files/PickAndPlace_IMU_2026_11_07.xlsx) |
| **VL53L5CX ToF sensor board** | 4 | [Gerber_TOF-sensor_2026-11-07.zip](../electrical-schematics/TOF%20sensors/production-files/Gerber_TOF-sensor_2026-11-07.zip) | [BOM_Fast TOF-sensor_2026-11-07.xlsx](../electrical-schematics/TOF%20sensors/production-files/BOM_Fast%20TOF-sensor_2026-11-07.xlsx) | [PickAndPlace_TOF-sensor_2026_11_07.xlsx](../electrical-schematics/TOF%20sensors/production-files/PickAndPlace_TOF-sensor_2026_11_07.xlsx) |

What the files mean:

- **Gerber ZIP:** board outline, copper layers, mask, silkscreen and drill information used by the PCB factory.
- **BOM:** component types, reference designators, packages and part numbers used for assembly.
- **CPL (component placement list):** the position, rotation and side of each placed component.

Use **all three files from the same board and revision** when ordering assembled PCBs. Do not pair the main-board Gerber with the IMU BOM or use old placement coordinates with a newer Gerber. A bare-board order needs only the Gerber, whereas PCBA needs all three.

For source traceability, first clone the repo and save the current commit ID:

```bash
git clone https://github.com/andreipopescufilimon/WRO2026_Future_Engineers.git
cd WRO2026_Future_Engineers
git rev-parse HEAD
```

The published files are manufacturing exports. Any change to the PCB design requires new, matching Gerber, BOM and CPL exports. Ensure the exported board is electrically and mechanically compatible with the selected firmware and chassis revision.

#### 2. Choose PCB fabrication or factory assembly

##### Option A: Bare PCBs

JLCPCB manufactures empty boards. Components must be purchased and soldered afterward, or assembled by another electronics service. **You do not upload BOM and CPL** for fabrication-only orders. The small BMI088 and VL53L5CX packages may be difficult to solder by hand without appropriate solder paste, microscope and reflow equipment.

##### Option B: Assembled PCBs (PCBA)

JLCPCB manufactures the board and mounts the components selected in its assembly system. Select **PCB Assembly** during the quotation process, before paying for the bare boards. Upload the matching BOM and CPL, match components, and review orientation in the assembly preview. Components that are unsupported, out of stock or not selected may remain unpopulated and need manual soldering.

Factory assembly is preferable for the IMU and ToF sensors if the intended exact packages are offered by the assembly service. Do not substitute a similar-looking sensor package, voltage regulator, connector or motor driver without checking electrical compatibility.

#### 3. Place the PCB order on JLCPCB

Perform these steps **three times**, once for each board design:

1. Go to **[jlcpcb.com](https://jlcpcb.com/)** and sign in. Select **Instant Quote** or the current PCB quotation page.
2. Upload the correct **Gerber ZIP** from the table. Keep the original ZIP intact.
3. Inspect the generated PCB preview: physical outline, drilled and mounting holes, openings, copper, masks, silkscreen, and front/back orientation.
4. **Select the correct copper layer count.** The **Main PCB is four-layer**, as shown by top, inner 1, inner 2 and bottom copper in [Chapter 03](03-power-sensors.md). Do not accept a two-layer default for that board. Determine the IMU and ToF layer counts from their own Gerbers.
5. Check **board thickness, dimensions, copper weight, finish, solder-mask color, panelization and any additional options** against the intended design. The repository does not give a complete verified production stack-up for each board. **Do not assume that JLCPCB's default 1.6 mm thickness or 1 oz copper is necessarily the correct HyperLine specification.**
6. On the **main chassis PCB**, pay particular attention to the structural outline, mounting holes, and interface with 3D-printed parts. The board doubles as the robot's frame, so a mechanically wrong PCB can prevent assembly even if the electrical nets are correct.
7. Select the desired manufacturing quantity. The robot needs **1 main + 1 IMU + 4 ToF boards**, but the minimum PCB order quantity may result in extras.
8. Review the quote and JLCPCB preview before continuing. Correct any Gerber or parameter mismatch before production.
9. For **bare boards**, add the reviewed item to the cart. For **PCBA**, continue to Step 4 before checkout.

**Do not order a main chassis PCB as a two-layer board.** The inner copper routing is part of the electrical design.

#### 4. Optional JLCPCB PCBA procedure

JLCPCB's ordering instructions describe a Gerber upload, PCB Assembly selection, BOM/CPL upload, component matching and placement review. The interface text may change over time.

1. In the PCB quote, enable **PCB Assembly**. Select the assembly side, number of assembled PCBs and available service options. Do this **before payment**.
2. Upload the **matching BOM and Pick-and-Place XLSX files** from Section 1. If the website asks for another supported file format, convert the spreadsheet carefully, preserving its column headers, reference designators, values, positions and rotations. JLCPCB supplies standard BOM/CPL formats in its help center.
3. Select **Process BOM & CPL**.
4. Review every matched component. Check the **designator**, exact value/part number, **footprint**, available stock and assembly side. Check the exact LCSC/JLCPCB catalog part number when listed.
5. Review all **unmatched, unavailable, optional and not-selected** components. Parts not selected in the final matching list will not be mounted automatically. Keep a separate list of components to solder by hand.
6. Inspect the 2D/3D **component placement preview**, not just the green status indicators. Pay close attention to:
   - **BMI088 orientation** and pin-1 marking.
   - **VL53L5CX orientation**, clear optical aperture and sensor package.
   - Polarized capacitors and LEDs.
   - **IFX9201SG** motor driver and thermal/contact orientation.
   - Connectors, 5 V regulator interface and cable direction.
7. If an item appears **rotated or mirrored**, confirm the correct location and rotation against the design. Correct the placement data and re-upload it when necessary.
8. Confirm the assembled side(s), then review final **fabrication costs, assembly fees, parts, shipping and estimated delivery**. These values are not fixed in the repository.
9. Only place the order when the preview and confirmed component list agree with the design. Keep the approved quote and manufacturing exports as evidence.

##### Mistakes worth checking before payment

| Problem | What to check |
|---|---|
| PCB is wrong shape or mounting holes are missing | Review Gerber outline and drill files; compare with CAD |
| Main PCB is quoted as two layers | Select four layers and confirm all copper layers exist |
| A part is absent in PCBA | Confirm it was selected and is available in JLCPCB's component list |
| Sensor is rotated or placed across an opening | Compare placement preview with footprint, pin 1 and optical clearance |
| BOM and CPL do not match | Compare designators, board revision and component coordinates |
| Some connectors or modules are not populated | Plan manual soldering and record the missing parts |

Official JLCPCB references:

- [How to place a PCBA order](https://jlcpcb.com/help/article/how-do-i-place-a-pcba-order)
- [PCB Assembly FAQs](https://jlcpcb.com/help/article/pcb-assembly-faqs)
- [Recommendations for BOM/CPL file preparation](https://jlcpcb.com/help/article/advice-for-bom-and-cpl-files-preparation)
- [Component matching guidelines](https://jlcpcb.com/help/article/component-matching-guidelines-for-pcba-orders)
- [Step-by-step PCB ordering guide](https://jlcpcb.com/help/article/instructions-for-ordering)

#### 5. If the production exports need revision

The three PCB manufacturing file sets can be downloaded from this repository. These are **production outputs**, not proof that the corresponding editable EasyEDA source projects have been committed here.

If you change a footprint, connector, circuit trace, outline or mounting hole in EasyEDA, regenerate **all relevant production files**, including Gerbers and the matching BOM/CPL. Preview the new board and component positions again on JLCPCB.

For a more complete reproduction package, the team should also publish:

- Editable **EasyEDA PCB and schematic source exports**, explicitly linked to the production release.
- An actual **PCB stack-up** for each board, including thickness and copper weight.
- A **PCBA assembly map** indicating any components the factory does not place and any parts soldered manually.
- Screenshots or notes from an **approved manufacturing preview**, if available.

#### 6. Assemble and check the delivered boards

**Before applying power:**

1. Inspect the PCB under good lighting or magnification for solder bridges, missing components, damaged pads, polarity and incorrect orientation.
2. Dry-fit the **structural main PCB** against the 3D-printed hardware and compare the mounting positions with the [assembly CAD](../3D-models/).
3. With all power sources disconnected, use a multimeter to check for unintended shorts between supply rails and ground.
4. Check connector polarity and verify that all external controller and sensor modules will see their allowed supply voltage.
5. Where possible, start with a **current-limited bench supply**. Check the regulated supply output before connecting sensitive electronics. Do not directly connect the battery to an unverified power circuit.
6. Power and test subsystems in stages: regulator, controller, IMU, each ToF module individually, floor color sensor, UART camera, motor driver and steering. Keep motors mechanically unloaded during initial driver tests.
7. The **four VL53L5CX boards** have the same factory I²C address. Follow the documented sequential startup/readdressing procedure in [Chapter 03](03-power-sensors.md) and [firmware](../src/HLR_FE_World_Final/) rather than expecting four identical boards to work immediately on one I²C bus.
8. Only then perform calibration and track testing from [the calibration and testing workflow below](#calibration-and-setup).

If any test fails, turn off power and diagnose the circuit before connecting additional modules.

#### 7. Record what was actually manufactured and verified

The repository contains manufacturing files; an ordering tutorial alone is **not proof that a particular batch was assembled successfully**. Keep separate records of actual orders, inspections and measurements.

| Evidence field | Fill in after the real event |
|---|---|
| Board and revision | |
| Git commit SHA for the production files | |
| Exact Gerber, BOM and CPL filenames | |
| JLCPCB order date, quantity and assembly selection | |
| Actual layer count, board thickness and copper weight | |
| Missing / manually populated components | |
| Visual inspection result | |
| Measured initial supply and regulated voltages | |
| Initial I²C / UART / motor checks | |
| Logs or photo evidence links | |
| Test date, hardware revision and firmware SHA | |

Use the existing [physical validation directory](validation/physical/) for real observations. Do not enter estimated values as though they were bench measurements.

Before applying battery power, inspect solder joints, power polarity, connector orientation and continuity. Check that there is no unintended short between supply rails and ground. Confirm the expected regulated voltages before connecting sensitive modules. Disconnect power before modifying wiring.

---

### Five-minute reviewer walkthrough

The following reading order lets another team locate the essentials without looking for extra chapters:

1. Inspect the [project overview](01-overview.md), [photos](../v-photos/) and dated historical [driving demonstration](https://www.youtube.com/watch?v=bg92ulHXomw).
2. Open [mobility](02-mobility.md) and the [STEP models](../3D-models/) to identify the structural board, rear differential and parallel steering assembly.
3. Open [power and sensors](03-power-sensors.md) and the [PCB manufacturing packages](../electrical-schematics/) for the power tree, physical sensor connections and exact fabrication inputs.
4. Open [software](04-software.md) together with [src/](../src/) to compare the Open and Obstacle strategy to the actual program in the chosen Git revision.
5. Read [systems engineering](05-systems.md) for the reasons behind the component and algorithm changes, including their limits.
6. Use the JLCPCB instructions above, the build/upload steps below and the [physical validation status](validation/physical/SUMMARY.md) to distinguish what is reproducible from what still needs a field measurement.

The repository's public Git history is the proper source for checking the three required commit deadlines. A downloaded ZIP does not contain commit timestamps. Refer to [version-specific commits](#version-specific-github-commits) and verify that each required snapshot predates the applicable competition deadline. Never recreate or backdate commits to imply compliance.

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

### 2. Assemble the mechanics and electronics

Use the eight current STEP models, the structural main PCB, the matching IMU and ToF PCBs, and the [assembly video](https://youtu.be/HSzWATB6nWQ). Disconnect the battery throughout assembly. The video remains the reference for exact fastener locations; do not force parts or invent dimensions not specified in CAD.

#### 2.1 Prepare and 3D-print the parts

Import the current STEP models in millimetres into CAD or a compatible slicer. Export meshes at sufficient resolution to preserve holes and bearing seats. Do not use the archived V3/V4.0 impeller or wheel models for the V4.1 build.

| Slicer parameter | Starting profile |
|---|---|
| Layer height | **0.08 mm**, team-specified |
| Nozzle | 0.4 mm, recommended starting point |
| First layer | 0.20 mm, recommended |
| Line width | 0.42 mm, recommended |
| Perimeters | 4, recommended |
| Top/bottom | At least 0.8 mm solid thickness (10 layers at 0.08 mm) |
| Infill | 35–50% gyroid for mounts; increase locally for loaded hubs as needed |
| Print speed | 35–50 mm/s; slower external walls for dimensional accuracy |
| Material, nozzle/bed temperature, cooling | Use the actual filament manufacturer's profile and record the material and settings |
| Supports | Only where needed; avoid support contact on bearing seats and optical openings |
| Adhesion | Brim when small contact area requires it |
| Scaling | 100% in millimetres; compensate only after measuring a test fit |

Only the 0.08 mm layer height is supplied as a team setting; the other numeric values are a suggested starting profile, not a verified historical print recipe. Save the actual slicer profile and record printer, material, orientation and any dimensional compensation. Remove support material, inspect holes, and dry-fit each part before installing hardware. A smooth printed surface does not guarantee correct fit or adequate strength.

**Evidence to add:** `docs/docs-media/world-final/printing-slicer-profile.png`

![Actual slicer profile showing 0.08 mm layer height, material and process settings](docs-media/world-final/printing-slicer-profile.png)

*Actual slicer profile showing 0.08 mm layer height, material and process settings (capture pending).*

**Evidence to add:** `docs/docs-media/world-final/assembly-01-printed-parts.jpg`

![Printed mounts, hubs, steering bar and wheels laid out before assembly](docs-media/world-final/assembly-01-printed-parts.jpg)

*Printed mounts, hubs, steering bar and wheels laid out before assembly (capture pending).*

#### 2.2 Hardware assembly sequence

| Step | Procedure | Acceptance check |
|---|---|---|
| 1. Main PCB chassis | Inspect both sides of the manufactured PCB; check connectors, solder joints, mounting holes and isolation before fitting mechanical supports. | No visible bridges, damage or unintended contact with fasteners. |
| 2. Rear drive support | Fit `drive-system-mount.step` hardware onto the chassis. Install motor, transmission, bearings and differential in the orientation shown in the CAD/video. | Rotate by hand; gears mesh freely without rubbing or excessive play. |
| 3. Rear wheels | Fit rear wheel assemblies and silicone contact surfaces. Secure the wheels using the documented mechanical interfaces. | Both wheels turn freely and the differential permits unequal wheel speeds. |
| 4. Front support | Attach the steering/camera mounting structure. Fit left/right hubs in their correct handed positions, front wheels and steering bar. | Free motion throughout the steering travel; wheels approximately parallel at neutral. |
| 5. Servo | With drive disabled, power the servo using the steering test; command neutral before attaching the horn/linkage. Disconnect power before tightening or adjusting linkage. | Both steering directions work without binding or hitting hard stops. |
| 6. Battery mount | Install battery bracket and retention method; keep battery disconnected. | Battery cannot slide into moving mechanisms; connector accessible. |
| 7. Sensors | Mount four ToF boards facing front, left, right and back, install IMU rigidly, place floor-marker sensor toward the track and fit camera on its support. | Optical windows unobstructed, sensor direction and camera ROI checked. |
| 8. Wiring | Connect the motor, servo, encoder, sensors, camera UART and supply connections according to the PCB schematics and controller pin definitions. Secure wiring clear of wheels and linkage. | Verify connector polarity, common ground, continuity and correct supply voltages before connecting devices. |
| 9. First power | With wheels raised and drive disabled, verify power rails and serial initialization. Confirm all four ToF addresses and stationary IMU calibration before starting motors. | No unexpected heating, resets or movement; stop and investigate faults. |
| 10. Calibration | Follow the steering, encoder, camera, ToF and IMU checks below, then perform controlled low-speed driving. | Record measured results with the actual firmware SHA. |

**Evidence to add:** `docs/docs-media/world-final/assembly-02-pcb-chassis.jpg`

![Structural PCB and mechanical mounting points before drivetrain installation](docs-media/world-final/assembly-02-pcb-chassis.jpg)

*Structural PCB and mechanical mounting points before drivetrain installation (capture pending).*

**Evidence to add:** `docs/docs-media/world-final/assembly-03-rear-drivetrain.jpg`

![Rear motor, gear transmission, differential and wheels installed](docs-media/world-final/assembly-03-rear-drivetrain.jpg)

*Rear motor, gear transmission, differential and wheels installed (capture pending).*

**Evidence to add:** `docs/docs-media/world-final/assembly-04-front-steering.jpg`

![Front hubs, servo and parallel steering linkage installed](docs-media/world-final/assembly-04-front-steering.jpg)

*Front hubs, servo and parallel steering linkage installed (capture pending).*

**Evidence to add:** `docs/docs-media/world-final/assembly-05-sensor-wiring.jpg`

![Camera, IMU, ToF modules and secured electrical connections](docs-media/world-final/assembly-05-sensor-wiring.jpg)

*Camera, IMU, ToF modules and secured electrical connections (capture pending).*

**Evidence to add:** `docs/docs-media/world-final/assembly-06-complete-robot.jpg`

![Complete assembled robot with battery retained and cable routing visible](docs-media/world-final/assembly-06-complete-robot.jpg)

*Complete assembled robot with battery retained and cable routing visible (capture pending).*

See [Mobility](02-mobility.md), [technical drawings](technical-drawings/) and [electrical schematics](../electrical-schematics/) for geometry and wiring references.

### 3. Prepare the software environment

The Arduino Nano ESP32 runs the vehicle controller; the OpenMV RT1062 runs the separate camera script. The source files are part of the selected project snapshot. The later V4.1 controller imports the following libraries and interfaces:

| Device or dependency | Required interface | Installation or source reference |
| --- | --- | --- |
| Arduino Nano ESP32 board package | Nano digital/analog pin names, `Serial0`, `Wire`, GPIO and PWM | Install the **Arduino Nano ESP32** board platform through Arduino IDE Boards Manager |
| `ESP32Encoder` | Encoder library included by the controller | Install through Arduino IDE Library Manager |
| `ESP32Servo` | `Servo.attach()` and `Servo.write()` for front-wheel steering | Install through Arduino IDE Library Manager |
| SparkFun VL53L5CX Arduino Library | `SparkFun_VL53L5CX` methods used by [`tof.ino`](../src/HLR_FE_World_Final/tof.ino) | Install the SparkFun VL53L5CX Arduino Library |
| BMI088 library | `BMI088.h` exposing `setAccScaleRange`, `setAccPoweMode`, `setGyroOutputDataRate` and `getGyroscopeZ` | Use a BMI088 implementation with the **same API as [`gyro.ino`](../src/HLR_FE_World_Final/gyro.ino)**; libraries with the same header name are not necessarily interchangeable |
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
| V4.1 controller source | `src/HLR_FE_Separated/HLR_FE_Separated.ino` with `Open.ino`, `Final.ino` and the hardware-control tabs | [Controller firmware](../src/HLR_FE_World_Final/) |
| RT1062 image-processing demonstration | Captured camera frame shows red and green object detections | [Camera view](docs-media/v41-openmv-RT1062-camera-view.png) |
| Physical autonomous driving | We published our real-world driving demonstration for both Open and Obstacle modes | [Driving video](https://www.youtube.com/watch?v=bg92ulHXomw) |

The separate V4.1 sketch tabs compile together as one Arduino program. The board package and supporting libraries are installed through Arduino IDE before uploading.

### 4. Flash the Arduino Nano ESP32

The Arduino Nano ESP32 executes the vehicle control software. Its entry point is [`src/HLR_FE_World_Final/HLR_FE_World_Final.ino`](../src/HLR_FE_World_Final/HLR_FE_World_Final.ino).

1. Install Arduino IDE and the **Arduino Nano ESP32** board support package through **Boards Manager**.
2. Connect the controller using a **USB-C data cable**.
3. Open `src/HLR_FE_World_Final/HLR_FE_World_Final.ino`. Confirm that `functions.ino`, `gyro.ino`, `motors.ino`, `tof.ino`, `Open.ino`, `Final.ino` and `Telemetry.ino` open as tabs in the same sketch.
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
| 1 | **Steering neutral and stops** | [`HLR_FE_World_Final.ino`](../src/HLR_FE_World_Final/HLR_FE_World_Final.ino) sets `STEERING_CENTER = 85`, `STEERING_LEFT = 39`, `STEERING_RIGHT = 131`, and parking endpoints `40` / `138`. Center the linkage before fixing the servo horn; inspect both wheel clearances with the wheels raised. | The wheels are aligned at neutral; commanded travel does not force the links into the chassis or steering stops. Servo command values are not physical wheel angles. |
| 2 | **Motor direction and encoder** | Rear motor PWM on D7, direction on D8; encoder A on D5 (rising-edge interrupt), B on D4 (direction). The source defines `WHEEL_DIAM = 29.0` mm and `MM_PER_TICK = 0.858` mm for its distance helper. Drive a measured straight distance on the actual surface and compare to `read_cm()`. | Positive wheel travel has the expected encoder direction and measured displacement agrees with the selected distance conversion. |
| 3 | **BMI088 heading** | [`gyro.ino`](../src/HLR_FE_World_Final/gyro.ino) estimates Z-axis bias over `DRIFT_TEST_TIME = 10` seconds at boot, with ±2000°/s gyro and ±6 g accelerometer settings. Keep the robot still during this interval, then rotate it manually both ways. | Heading increases/decreases consistently with the sign expected by the steering controller, without visible stationary jumps. |
| 4 | **Four ToF matrices** | [`tof.ino`](../src/HLR_FE_World_Final/tof.ino) releases one LPn pin at a time and assigns front A3=`0x30`, left A2=`0x31`, right A1=`0x32`, rear A0=`0x33`; each is 4×4, configured at 30 Hz. Test all four optical directions with a flat target and check that mount/wheels do not occlude the field of view. | All four sensors initialize with unique addresses and provide valid ranges for targets in their respective directions. |
| 5 | **Front corner zones** | The Open controller uses front ToF zones `(2,3)` and `(3,3)` (1-based row/column) for a corner below 700 mm (current World Final source), with rearming once both are at least 800 mm. Data older than 150 ms is rejected. Check which physical side corresponds to those zones after mounting. | The correct wall and zones trigger a single turn, without mirrored indexing or repeated corner events. |
| 6 | **Floor colors** | The TCS34725 uses separate I²C pins A6/A7. [`functions.ino`](../src/HLR_FE_World_Final/functions.ino) reads normalized RGB channels; the source selects 16× gain and a short exposure. Present white, blue and red/orange field markings at the installed floor clearance. | White re-arms detection and colored markers set or confirm the intended travel direction. |
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
| Initial `robot_speed` command | 250 (first-marker logic temporarily uses 80) | 50 |
| `kp` | 0.028 | 0.038 |
| `ki` | 0 | 0 |
| `kd` | 0.080 | 0.070 |
| `delayTurns` | 1000 ms | 1500 ms |
| Additional behavior | ToF-driven turns; corner PWM command 80 | Colored-pillar interaction and parking; `park_speed = 32` in main sketch |

Motor speed values are controller PWM commands on an 8-bit scale, **not km/h or motor terminal voltage**. The nominal 6 V motor is intentionally operated from the robot's 2S LiHV drive rail as documented in [Chapter 03](03-power-sensors.md); avoid interpreting the PWM fraction as an electrical or thermal protection guarantee.

---

### World Final release verification

The 24 November 2026 rolling-parking upload is the current controller release. Open `src/HLR_FE_World_Final/HLR_FE_World_Final.ino` with all eight tabs in the same directory. The [source manifest](../src/HLR_FE_World_Final/SOURCE_MANIFEST.json) records original and documented SHA-256 hashes; the added file comments do not alter executable source. This does not constitute a successful Arduino build or a physical run. Record the actual board package, compile output, flashed commit and measured results before identifying this revision as tested.

| Test | Evidence to save |
|---|---|
| Compile and upload | `docs/docs-media/world-final/firmware-build-log.txt` |
| Encoder calibration, both rounds | Measured distance versus 0.858 mm/tick |
| Open corner and braking | Three fresh agreeing pairs, speed ramp, re-arm distance and time |
| Final cube recovery | Camera commands, phase transitions, lateral estimate and return behavior |
| Rolling parking | Front 400 mm alignment, two side posts, 45 mm offset and final stop |
| Parking/color diagnostics | `docs/docs-media/world-final/parking-side-log.csv`, `docs/docs-media/world-final/color-log.csv` |

Keep prior revision datasets separate from new results. The existing hardware assembly and 0.08 mm print profile remain applicable subject to actual hardware confirmation.

## Running the vehicle

After flashing both devices, disconnect programming cables as appropriate for the power setup and secure the LiHV battery. Select the mode with **one** 3.3 V HIGH input: D12 for Open or D11 for Final. The sketch latches that selection when powered on. During initialization, keep the vehicle stationary for the BMI088's 10-second bias calibration; the start input is D10 (active LOW). The controller then waits for a start command before driving. The [calibration and setup sequence](#calibration-and-setup) summarizes the checks that connect the code settings to the assembled geometry.

### Connect to the World Final telemetry dashboard

The uploaded `src/HLR_FE_World_Final/` firmware integrates a read-only Wi-Fi dashboard in [`Telemetry.ino`](../src/HLR_FE_World_Final/Telemetry.ino). Unlike the historical configuration interface, this dashboard cannot change driving parameters or send motor commands. It starts only in Final mode.

1. Flash the complete `HLR_FE_World_Final` sketch with its matching hardware and camera code. Select Final mode before boot.
2. Keep the wheels raised or the robot securely stationary during connection and initial checks. Allow IMU initialization to complete.
3. Join SSID `HyperLine` using password `HyperLine-FE`, then open [http://192.168.4.1](http://192.168.4.1). Internet access is unnecessary.
4. Check the Live indicator and the robot state. Observe yaw, target, heading error, angular rate, gyro bias, acceleration, pitch/roll, PWM, encoder counts, camera commands and selected ToF cells.
5. Capture the actual dashboard during stationary tests, controlled movement and parking. Record the flashed SHA, date and scenario with each image.


**Evidence to add:** `docs/docs-media/world-final/telemetry-01-idle.png`

![World Final dashboard at rest after IMU calibration](docs-media/world-final/telemetry-01-idle.png)

*World Final dashboard at rest after IMU calibration (capture pending).*

**Evidence to add:** `docs/docs-media/world-final/telemetry-02-heading-control.png`

![Live actual and target yaw history during a controlled turn](docs-media/world-final/telemetry-02-heading-control.png)

*Live actual and target yaw history during a controlled turn (capture pending).*

**Evidence to add:** `docs/docs-media/world-final/telemetry-03-cube-avoidance.png`

![Cube navigation state, camera command, steering and heading response](docs-media/world-final/telemetry-03-cube-avoidance.png)

*Cube navigation state, camera command, steering and heading response (capture pending).*

**Evidence to add:** `docs/docs-media/world-final/telemetry-04-parking.png`

![Parking stage, selected ToF distances, heading and stop/event status](docs-media/world-final/telemetry-04-parking.png)

*Parking stage, selected ToF distances, heading and stop/event status (capture pending).*

For the dashboard implementation and limitations see [Software](04-software.md#world-final-read-only-telemetry). For complete 4×4 matrix capture and zone validation see [Power and sensors](03-power-sensors.md#world-final-tof-matrix-evidence). The historic dashboard pictures remain archived in `docs-media/`; they are not evidence that the current dashboard supports remote tuning.

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
| ToF sensor checks | [`tof.ino`](../src/HLR_FE_World_Final/tof.ino) and optional [diagnostic dashboard](#live-distance-sensor-checks) | Each installed sensor responds to a target moving through its field of view |
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
| Controller firmware and source | [V4.1 sketch](../src/HLR_FE_World_Final/) | **Available:** published source modules for the controller and both challenge modes |
| Four VL53L5CX modules | [Sensor implementation](../src/HLR_FE_World_Final/tof.ino), [sensor calibration section](03-power-sensors.md#51-distance-sensor-calibration-matrix) | **Implemented in source:** four sensor addresses and 4×4 ranging configuration |
| BMI088 and steering/drive | [Gyro implementation](../src/HLR_FE_World_Final/gyro.ino), [motor implementation](../src/HLR_FE_World_Final/motors.ino) | **Implemented in source:** IMU acquisition, steering and motor-control routines |
| Battery and regulated-power reliability | [Power and sensor architecture](03-power-sensors.md) | **Documented:** 2S LiHV supply, regulator, power distribution and protection considerations |

The videos and camera frame document the assembled robot and its operation. The historical controller and camera source correspond to the filmed revision identified above. The separated V4.1 source is provided as the newer modular implementation.

The assembly recording, challenge footage and RT1062 frame buffer provide the practical examples accompanying this reproduction guide. The testing examples above describe how each subsystem is inspected before autonomous field operation.


### V4.1 hardware and firmware release: status and verification

**Compilation status (team report, 17 November 2026):** The team confirms that its Arduino controller sketch compiles successfully on its development laptop. No compiler log or exact toolchain output has been supplied in this archive. The missing log is a missing *retained verification artifact*, not evidence of a failed build. The separate OpenMV camera firmware still needs to be tested on the target camera. See [team-reported build status](validation/TEAM_REPORTED_COMPILATION.md) for the precise evidence boundary.

This table identifies the V4.1 configuration to reproduce and the existing records that establish which parts have been verified. A historical Git snapshot may identify a former source baseline, but this uploaded ZIP has **no `.git` metadata**, so the snapshot cannot be assumed to match the archive byte-for-byte. The build log, upload record and physical run must identify the **same actual flashed revision** before the current release can be described as fully verified. The candidate snapshot below is not, by itself, evidence that the robot was operated with that software.

| Release item | Identifiable configuration or evidence record |
| --- | --- |
| Hardware revision | V4.1, as documented in the [assembly procedure](#2-assemble-the-mechanics), [PCB manufacturing packages](../electrical-schematics/) and [vehicle photographs](../v-photos/) |
| Main controller | Arduino Nano ESP32 |
| Vision processor | OpenMV RT1062 |
| Fixed candidate Git snapshot | [`4e5da95c8b87787be97cb1e5f976c625865338bb`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/4e5da95c8b87787be97cb1e5f976c625865338bb), repository checkpoint on 10 November 2026; record the actual flashed SHA if different |
| Controller firmware | [`src/HLR_FE_World_Final/`](../src/HLR_FE_World_Final/), including `HLR_FE_Separated.ino`, `Open.ino`, `Final.ino` and the shared hardware tabs |
| Camera firmware source | [`src/camera.py`](../src/camera.py); use the version from the same fixed Git snapshot as the controller and record the camera firmware/IDE version |
| Bundled Arduino libraries | [BMI088 1.0.0](../src/libraries/BMI088/library.properties), [ESP32Encoder 5.0.0](../src/libraries/ESP32Encoder/library.properties), [ESP32Servo 3.0.3](../src/libraries/ESP32Servo/library.properties), [SparkFun VL53L5CX 1.0.3](../src/libraries/SparkFun_VL53L5CX_Arduino_Library/library.properties). [Adafruit TCS34725 1.4.4](../src/libraries/Adafruit_TCS34725/library.properties) is also bundled; the autonomous sketch accesses this sensor directly over I²C |
| Board-core and installed build environment | Record the Arduino CLI version, installed board platforms and libraries with `arduino-cli version`, `arduino-cli core list` and `arduino-cli lib list`; retain the build output from the GitHub Actions workflow or local compilation |
| Compilation status and retained original output | **Team-reported successful Arduino compile on laptop (17 November 2026).** [`build_verification.csv`](validation/physical/build_verification.csv) still has no logged exit code or attached compiler output. A dated `exit code 0` and actual compiler log would strengthen independent reproducibility; the rubric does not explicitly make a stored log mandatory for 6 points. See [compilation status](validation/TEAM_REPORTED_COMPILATION.md). |
| Controller upload | Record date, flashed source SHA, upload command or IDE confirmation, selected board/port and the robot's startup serial output alongside the build evidence |
| Physical subsystem checks | [`functional_checks.csv`](validation/physical/functional_checks.csv) and [`subsystem_test_record.csv`](validation/physical/subsystem_test_record.csv), using dated observations and the actual flashed SHA |
| Autonomous driving verification | [`challenge_runs.csv`](validation/physical/challenge_runs.csv), containing mode, course direction, observed outcome, source SHA and a matching recording |
| Historical driving demonstration | [Published Open and Obstacle footage](https://www.youtube.com/watch?v=bg92ulHXomw), associated by the team with historical commit [`de34faf039a70443361197de540879f212e3de81`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81), not automatically with this modular source snapshot |

**Firmware verification.** After checking out the intended release SHA with a clean working tree, compile the modular sketch using Arduino CLI or the [GitHub Actions build workflow](../.github/workflows/verify-v41.yml). Keep the full compiler output and actual exit status, plus the installed board-platform and library versions. Enter the observed result in [`build_verification.csv`](validation/physical/build_verification.csv). A successful compilation shows that this combination of source and dependencies builds; it does not by itself establish a successful upload or autonomous operation.

**Physical verification.** Upload the compiled sketch to the assembled Arduino Nano ESP32, save the IDE or CLI upload confirmation, and test the motor, steering, four ToF sensors, BMI088 heading, RT1062 UART and encoder. Link the original readings or recordings in the [functional test ledger](validation/physical/functional_checks.csv). The table and test IDs already exist; no additional testing framework is required.

**Autonomous operation.** Record at least one Open or Obstacle run with the same flashed firmware revision, including its date, course direction, outcome and video link in [`challenge_runs.csv`](validation/physical/challenge_runs.csv). The source SHA in that row should match the build and physical-check records. Once these three evidence stages are connected, the documented V4.1 source becomes a traceable **built and operated release**, not just an identifiable repository revision.

### 4. Verifying one complete modular V4.1 revision

The modular implementation must be checked as a **single hardware, controller and camera combination**. The earlier `de34faf` footage demonstrates the historical vehicle; it is not substituted for an execution record of `src/HLR_FE_World_Final/`. The `045c57e` revision above identifies an earlier modular source checkpoint. When reproducing the latest Open/Final code, capture the **full Git SHA of the actual release being flashed** so the controller, camera code, bundled libraries and manufacturing resources can be traced to the same repository state.

From a clean clone of the intended modular release, record its identity before installing or compiling:

```bash
git rev-parse HEAD
git status --short
python scripts/repro_preflight.py
```

The preflight checks required files, firmware declarations and source integrity; it also creates a `docs/validation/evidence/source-sha256.json` and a `docs/validation/evidence/static-preflight-results.md` when run from the repository root. These checks establish which files were inspected. They do **not** substitute for compilation or a physical driving test. Record any local firmware modifications alongside the SHA, since uncommitted changes are not represented by `git rev-parse HEAD` alone.

For an **actual Nano ESP32 compilation** on Windows, Linux or macOS, install Arduino CLI and the selected board platform, then run these commands from the repository root in a terminal:

```bash
arduino-cli version
arduino-cli core list
arduino-cli lib list
arduino-cli compile --fqbn arduino:esp32:nano_nora --libraries src/libraries --warnings all --verbose src/HLR_FE_World_Final
```

Retain the full compiler output and its actual success or failure status, alongside the exact source revision and library/platform versions. The GitHub Actions workflow described below collects these files automatically and provides a downloadable build-evidence artifact. Running the commands above is a build attempt, not a claim that compilation succeeded. If a local build fails, keep its error log and resolve the environment before marking the result as verified.

**Build without a local development computer:** The [GitHub Actions V4.1 compilation workflow](../.github/workflows/verify-v41.yml) runs the same modular sketch in a GitHub-hosted runner. From GitHub, open **Actions → V4.1 modular firmware build → Run workflow**. The run page identifies the exact source SHA, and its downloadable `v41-build-evidence-*` artifact contains the actual compiler log, installed core version and library inventory. Only a successful workflow proves that the sketch compiled in that environment. It does not prove controller upload or driving performance. A **successful build record** includes an actual exit code `0` and the matching compiler log (the workflow artifact uses `exit-code.txt` and `compile.log`). If the compiler reports a missing library, ambiguous `BMI088.h`, or another error, fix the environment and rerun the capture. The recorded result is more useful than a claim that a sketch *should* compile. The library copies in `src/libraries/` identify the exact supplied implementations; the build log is also needed to identify the versions actually selected by the compiler.

For the RT1062, open the matching `src/camera.py` with OpenMV IDE, record the camera firmware version shown by the connected camera, save the script to the camera and retain one captured frame and one UART message log. The controller and camera must agree on **19,200 baud**; the image demonstration and any future height-classification results must identify the **separate, later camera script revision** that actually implements those features. The current bundled camera source does not classify height.

| Evidence item | Source or output location | Minimum information for reproduction |
| --- | --- | --- |
| Reproduced repository release | `git rev-parse HEAD` and source manifest | Full Git commit SHA, local changes if any, matching controller and RT1062 scripts |
| Arduino compilation | [GitHub Actions V4.1 build workflow](../.github/workflows/verify-v41.yml) or local `arduino-cli compile` | Actual compile command, installed dependency versions, exit code and full compiler log |
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

For camera-height tests, the previously proposed mapping is **50 mm and 150 mm: invert the visible red/green command; 100 mm: retain it**. That mapping and the associated tracking and partial-visibility handling are **not present in the included camera source** and cannot be represented as implemented or tested. Record both correctly classified and misclassified trials; a threshold written in software is not a measured classification success rate.

Each video or run record for the modular release should include the full **source SHA actually flashed**, the camera revision, hardware version, challenge direction and outcome. This is separate from the real driving video for historical commit `de34faf`. A short continuous run with clearly identifiable firmware is more convincing evidence for the newer release than an unversioned performance claim. Keep failed or partial runs in the log as well as successful ones so another team can understand the operating limits.

### Optional recovered camera candidates

Two earlier user-authored files are archived in [`src/camera_candidates/`](../src/camera_candidates/) and can help close the 2026 Surprise Rule implementation gap. The v11 camera candidate's pure logic passes [23/23 reproducible tests](validation/v11_camera_candidate_unit_tests.csv). These tests **do not certify that the candidate was run on an actual RT1062**. Until flashed, calibrated and recorded under the same software release as the Nano controller, the primary `src/camera.py` remains the source of the published repository behavior.

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
| Camera Surprise Rule | The uploaded source lacks sign-height classification; retain this as an open implementation and physical-verification requirement |
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

The filmed robot used the earlier `src/HLR_FE/` sketch. The later V4.1 source separates the Open and Obstacle Challenge routines in `src/HLR_FE_World_Final/`, reflecting the changes in sensor handling and program structure.

### Filming firmware and modular revision

The team identifies [`de34faf039a70443361197de540879f212e3de81`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) as the code revision running in the [real Open and Obstacle Challenge footage](https://www.youtube.com/watch?v=bg92ulHXomw). That snapshot contains `src/HLR_FE/HLR_FE.ino`. Its source, camera program and mechanical/electrical documentation are accessible as a fixed historical Git state.

The later source snapshot [`045c57e7388c08f9281642a864de0ba3b099c0df`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) contains `src/HLR_FE_World_Final/`, which moves Open and Final behavior into their respective files and uses the four-matrix ToF interface. It is the basis of the modular configuration and calibration references in this chapter. This distinction keeps the historical demonstration and newer implementation individually traceable.

| Release resource | Reference |
| --- | --- |
| Filmed source and matching historical files | [Commit `de34faf`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/de34faf039a70443361197de540879f212e3de81) |
| Modular V4.1 controller and project snapshot | [Commit `045c57e`](https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/commit/045c57e7388c08f9281642a864de0ba3b099c0df) |
| Assembled vehicle | [Assembly video](https://youtu.be/HSzWATB6nWQ) and [robot photographs](../v-photos/) |
| Real-world autonomous operation | [Open and Obstacle driving video](https://www.youtube.com/watch?v=bg92ulHXomw) |
| Physical sensor and control references | [Power and sensors](03-power-sensors.md), [mobility](02-mobility.md) and [systems](05-systems.md) |
