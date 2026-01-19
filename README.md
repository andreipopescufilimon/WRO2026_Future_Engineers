# WRO2025 Future Engineers – Nerdvana Taurus Team

---

Repository of Nerdvana Taurus Team competing in the **World Robot Olympiad (WRO) 2025**, Future Engineers category. 

### 🔗 Links

<p align="left">
  <a href="https://hyperlinerobotics.com/" target="_blank">
    <img src="https://img.shields.io/badge/Website-000?style=for-the-badge&logo=google-chrome&logoColor=white" />
  </a>

  <a href="https://www.youtube.com/@HyperLineRobotics_Team" target="_blank">
    <img src="https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white" />
  </a>

  <a href="https://www.instagram.com/hyperlinerobotics_team/" target="_blank">
    <img src="https://img.shields.io/badge/Instagram-E4405F?style=for-the-badge&logo=instagram&logoColor=white" />
  </a>

  <a href="https://x.com/HLR_Team" target="_blank">
    <img src="https://img.shields.io/badge/X-000000?style=for-the-badge&logo=x&logoColor=white" />
  </a>

  <a href="https://www.facebook.com/hyperlineroboticsteam" target="_blank">
    <img src="https://img.shields.io/badge/Facebook-1877F2?style=for-the-badge&logo=facebook&logoColor=white" />
  </a>

  <a href="https://github.com/andreipopescufilimon" target="_blank">
    <img src="https://img.shields.io/badge/GitHub-181717?style=for-the-badge&logo=github&logoColor=white" />
  </a>

  <a href="https://oshwlab.com/bicicleta11/works" target="_blank">
    <img src="https://img.shields.io/badge/OSHWLab-0A7DFF?style=for-the-badge&logo=opensourcehardware&logoColor=white" />
  </a>
</p>

## ❤️ Sponsors and Partners

<p align="center">
  <img src="https://hyperlinerobotics.com/assets/images/sponsors/openmv.avif" height="60" />
  <img src="https://hyperlinerobotics.com/assets/images/sponsors/EasyEDA_logo.png" height="60" />
  <img src="https://hyperlinerobotics.com/assets/images/sponsors/OSHWLab.png" height="60" />
  <img src="https://static.lcsc.com/feassets/pc/images/headIcons/logo-s.png" height="60" />
  <img src="https://hyperlinerobotics.com/assets/images/sponsors/ING.png" height="60" />
  <img src="https://hyperlinerobotics.com/assets/images/sponsors/FundatiaComunitaraBucuresti.png" height="60" />
  <img src="https://hyperlinerobotics.com/assets/images/sponsors/SoBotz.png" height="60" />
  <img src="https://jsumo.com/Data/EditorFiles/jsumo-robot-parts-logo.png" height="60" />
  <img src="https://hyperlinerobotics.com/assets/images/sponsors/evergreen.png" height="60" />
  <img src="https://fae-drones.com/wp-content/uploads/2022/08/logo-vertical-ro.png" height="60" />
</p>


---

## 📚 Table of Contents

- [👥 The Team](#the-team)
- [🎯 Challenge Overview](#challenge-overview)
- [🤖 Our Robot](#our-robot)
- [⚙️ Mobility Management](#mobility-management)
  - [🚗 Drivebase](#drivebase)
    - [🔧 Drivetrain](#drivetrain)
    - [⚙️ Motor](#motor)
    - [🛞 Wheels](#wheels)
    - [🔌 Motor Driver IFX9201SG](#motor-driver)
    - [🌪️ Impeller](#impeller)
  - [🔄 Steering](#steering)
    - [🔄 Steering Servo Motor](#servo-motor)
  - [🏎️ Chassis & Component Mounting](#chassis)
    - [✨ Key Features & Advantages](#key-features)
    - [🔧 Assembly Process](#assembly-process)
      - [🔧 Assembly Process Video](#assembly-process-video)
      - [🔧 3D print the parts](#3d-print-the-parts)
      - [🔧 Assemble the robot](#assembly)
- [🛠️ Power and Sense Management](#power-and-sense-management)
  - [🔋 Li-Po Battery](#li-po-battery)
  - [🖥️ Arduino Nano ESP32](#arduino-nano-esp32)
  - [🧭 IMU Sensor BMI088](#imu-sensor-bmi088)
  - [📷 OpenMV H7 Camera](#openmv-h7-camera)
  - [⚙️ Drive Motor](#drive-motor)
  - [⚙️ Motor Driver IFX9201SG](#motor-driver)
  - [🔄 Steering Servo MG90S](#steering-servo)
  - [⚙️ Impeller](#impeller)
  - [📏 Pololu PWM Distance Sensor](#distance-sensors)
  - [🔌 Voltage Regulator D24V50F5](#voltage-regulator)
  - [🛠️ PCB Design](#pcb-design)
  - [⚡ Power Consumption](#power-consumption)
- [💻 Components coding](#components-coding)
  - [⚡ Drive motor](#drive-motor-coding)
  - [🌪️ Impeller](#impeller-coding)
  - [🎮 Servo motor](#servo-motor-coding)
  - [🧭 IMU](#imu-coding)
  - [📡 Distance sensors](#distance-sensors-coding)
  - [📷 Camera](#camera-coding)
- [📝 Obstacle Management](#obstacle-management)
  - [🏁 Open Round](#open-round) 
  - [⚡ Final Round](#final-round)
  - [🅿️ Start from Parking](#start-from-parking)
  - [🅿️ Parking](#parking)
- [📽️ Performance Video](#performance-video)
- [💰 Cost Analysis](#cost-analysis)
  - [📦 Component Costs](#components-costs)
  - [🔌 PCB Manufacturing Cost](#pcb-cost)
  - [🖨️ 3D Printing Cost](#3d-printing-cost)
  - [🔩 Other Materials (Screws, Nuts, Miscellaneous)](#other-materials-cost)
  - [💵 Total Cost](#total-cost)
- [📂 Randomizer](#randomizer)
- [📂 Resources](#resources)
- [📜 License](#license)

## 📂 Folder Structure


This repository is organized as follows:


```
📦 WRO2025_Future_Engineers
├── 📁 3D-models             # Contains 3D design files for the robot's components
│   ├── 📁 old-3D-models     # Previous versions of 3D models
│   └── 📁 step-models       # 3D models in STEP format
├── 📁 electrical-schematics # Circuit diagrams and wiring
├── 📁 github-commits        # Commit logs and change tracking details for this repository
├── 📁 media                 # Images and videos 
│   ├── 📁 robot-photos      # Photos of the robot
│   ├── 📁 team-photos       # Pictures of team members and teamwork
│   └── 📁 video             # Recorded testing
├── 📁 other                 # Contains files that do not fit into other categories
├── 📁 src                   # Main source code for the robot
├── 📁 technical-draws       # Technical drawings and mechanical blueprints
├── 📁 video                 # Videos of our robot
├── 📄 LICENSE               # MIT License for the project
└── 📄 README.md             # Main documentation for the project
```

---

## 👥 The Team <a id="the-team"></a>

<img src="" width="450">

### **Popescu Filimon Andrei Cosmin** (Left Side)
**Age:** 17 <br>

**High School:** International Computer High School Bucharest (ICHB)  

**Description:**  
Hi! I’m Andrei from Romania, and this is my sixth WRO season. This is my second season in Future Engineers, as before I competed in the Robomission category. I am passionate about robotics, especially electronics and the latest algorithms and tech. Over the years, I have worked on multiple robotics projects, including line followers, sumo bots, and air quality modules. I also founded HyperLine Robotics, where I build and develop advanced robotics projects with my team. Apart from robotics, I also enjoy cybersecurity, programming, and cycling.

---

### **Eduard Alexandru Vasile**
**Age:** 15 <br>
**High School:** ...

**Description:**  
**to be completed*

---

## 🎯 Challenge Overview <a id="challenge-overview"></a>

The **WRO 2025 Future Engineers** challenge pushes teams to develop a **fully autonomous vehicle** capable of navigating a **dynamic and randomized racetrack** using **sensors, computer vision, and advanced control algorithms**. The goal is to complete **multiple laps** while adapting to randomized obstacles, following **strict driving rules**, and successfully executing a **parallel parking maneuver** at the end of the course.

### 📌 Competition Format 

- **🏁 Open Challenge**: The vehicle must complete **three (3) laps** on a track with **randomly placed inside walls**.

- **🚦 Obstacle Challenge**: The vehicle must complete **three (3) laps** while detecting and responding to **randomly placed red and green traffic signs**:
  - 🟥 **Red markers** ➜ The vehicle must stay on the **right side of the lane**.
  - 🟩 **Green markers** ➜ The vehicle must stay on the **left side of the lane**.
  
  After completing the three laps, the vehicle must **locate the designated parking zone** and perform a **precise parallel parking maneuver** within a limited space, adding an extra layer of difficulty.
  
- **📑 Documentation**: Each team must maintain a **public GitHub repository** showcasing their **engineering process, vehicle design, and source code**.

### 🏆 Scoring & Evaluation
Scoring is based on **accuracy, technical documentation and speed**, rewarding teams that balance **efficiency, adaptability, and innovation**. This challenge not only tests **robotics and programming skills** but also promotes **problem-solving, teamwork, and engineering creativity**.

🔗 **Find out more about the challenge [here](https://wro-association.org/wp-content/uploads/WRO-2026-Future-Engineers-Self-Driving-Cars-General-Rules.pdf).** 🚀

---

## 🤖 Our Robot <a id="our-robot"></a>
| <img src="" width="300">         | <img src="" width="300">            |
|----------------------------------|-------------------------------------|
| <p align="center"><b>Top</b></p> | <p align="center"><b>Bottom</b></p> |

| <img src="" width="300">          | <img src="" width="300">           |
|-----------------------------------|------------------------------------|
| <p align="center"><b>Left</b></p> | <p align="center"><b>Right</b></p> |

| <img src="" width="300">           | <img src="" width="300">          |
|------------------------------------|-----------------------------------|
| <p align="center"><b>Front</b></p> | <p align="center"><b>Back</b></p> |


---

## 🚗 Mobility Management <a id="mobility-management"></a>

**to be completed*


## ⚙️ **Drivebase** <a id="drivebase"></a>

### 🔧 **Drivetrain** <a id="drivetrain"></a>

**to be completed*

### ⚙️ **Motor – 30:1 Micro Metal Pololu Gearmotor HPCB** <a id="motor"></a>

**to be completed*


### 🛞 Wheels & Tires (Silicone) <a id="wheels"></a>

**to be completed*


### 🔌 **IFX9201SG Motor Driver** <a id="motor-driver"></a>

**to be completed*


### **⚙️ Impeller for downforce** <a id="impeller">

**to be completed*


---


## 🔄 **Steering** <a id="steering"></a>

**to be completed*

---

### **🔄 Steering Servo – MG90S** <a id="servo-motor"></a>

**to be completed*


---

## 🏎️ **Chassis & Component Mounting** <a id="chassis"></a>

**to be completed*


---

## 🔧 Assembly Process <a id="assembly-process"></a>

🔗 **[Click here to watch the assembly video on YouTube]()** <a id="assembly-process-video"></a>

---

### 2: Assemble the Chassis & Components <a id="assembly"></a>


#### 🔧 **Main Assembly Steps**
...
...
...
**to be completed*


---

## 🛠️ Power and Sense Management <a id="power-and-sense-management"></a>
**to be completed*


---

### **🔋 Li-Po Battery – 2S 300mAh** <a id="li-po-battery"></a>
**to be completed*


---

### **🖥️ Arduino Nano ESP32 – Main Controller** <a id="arduino-nano-esp32">
**to be completed*


---

### 🧭 IMU Sensor – BMI088 <a id="imu-sensor-bmi088"></a>
**to be completed*


---

### **📷 OpenMV ... Camera – Vision Processing** <a id="openmv-h7-camera">
**to be completed*


---

### **⚙️ Drive Motor – 30:1 Micro Metal Gearmotor HPCB 6V with Encoder** <a id="drive-motor">
**to be completed*


---

### **⚙️ IFX9201SG Motor Driver** <a id="motor-driver"></a>
**to be completed*


---

### **🔄 Steering Servo – MG90S** <a id="steering-servo">
**to be completed*


---

### **⚙️ Impeller for downforce** <a id="impeller">
**to be completed*


---

### 📏 Distance Sensor – Pololu PWM Distance Sensor <a id="distance-sensors"></a>
**to be completed*


---

### **🔌 D24V50F5 Voltage Regulator – Power Management** <a id="voltage-regulator"> 
**to be completed*


---

### **🛠️ PCB Design** <a id="pcb-design"></a>
**to be completed*


---

### **⚡ Power Consumption Summary** <a id="power-consumption"></a>

| Item / Designators                   | Part            |                    Supply |    Typical Current |    Peak Current | Notes                               |
| ------------------------------------ | --------------- | ------------------------: | -----------------: | --------------: | ----------------------------------- |
**to be completed*

---

## 💻 Components coding <a id="components-coding"></a>

### ⚡ Drive motor <a id="drive-motor-coding"></a>
**to be completed*


### 🌪️ Impeller <a id="impeller-coding"></a>
**to be completed*


### 🎮 Servo motor <a id="servo-motor-coding"></a>
**to be completed*


### 🧭 IMU <a id="imu-coding"></a>
**to be completed*


### 📡 Distance sensors <a id="distance-sensors-coding"></a>
**to be completed*


### 📷 Camera <a id="camera-coding"></a>

Once the robot can move and steer, it also needs to see and react to the environment. For this, we use an **OpenMV camera module**, connected via **UART protocol**. This allows the camera to handle the heavy work of image processing and only send compact messages (like colors, cubes, or angles) to the **ESP32**.

The camera is wired to the **ESP32** as follows:
- Camera TX (P4) → ESP32 RX (D0)
- Camera RX (P5) → ESP32 TX (D1)

Both devices must share the same baud rate **(19200)**. On the **ESP32** side, we use `SoftwareSerial` for communication, while on the camera side, `UART(3, 19200)` is initialized.

**Arduino code:**
**to be completed*

**Camera code:**
**to be completed*


---

## 📝 Obstacle Management <a id="obstacle-management"></a>

### 🏁 Open Round <a id="open-round"></a>
**to be completed*


### ⚡ Final Round <a id="final-round"></a>

**to be completed*

---

### Arduino Side

#### States Driving Logic
**to be completed*

**PID:**
**to be completed*

**FOLLOW_CUBE:**
**to be completed*

**AVOID_CUBE:**
**to be completed*
  
**AFTER_CUBE:**
**to be completed*

 #### UART Command Parser (execute_command())
**to be completed*

### Camera Side (OpenMV ...)
**to be completed*

### 🅿️ Starting from Parking <a id="start-from-parking"></a>
**to be completed*

### 🅿️ Parking <a id="parking"></a>
**to be completed*


---

## 📽️ Performance Video <a id="performance-video"></a>

🔗 **[Click here to watch the video on YouTube(without music)]()** 
🔗 **[Click here to watch the video on YouTube(with music)]()** 

---

## 💰 Cost Analysis <a id="cost-analysis"></a>
We have optimized our robot for **performance vs. cost efficiency**. The total cost includes motors, sensors, electronics, PCB, 3D printing and custom parts.

### 📦 **Components Costs** <a id="components-costs"></a>

| Component                      | Quantity | Unit Price ($) | Total ($)  |
|--------------------------------|----------|----------------|-------------|
| **TOTAL COMPONENT COST**       | -        | **-**          | **TBD**  |

**Experimental Parts**
**to be completed*

---

### **🔌 PCB Cost (Manufactured via JLCPCB)** <a id="pcb-cost"></a>
| PCB Component                  | Quantity | Unit Price ($) | Total ($)    |
|--------------------------------|----------|----------------|--------------|
| **PCB Manufacturing (JLCPCB)** | TBD      | **TBD**        | **TBD**      |
| **PCB Assembly (JLCPCB)**      | TBD      | **TBD**        | **TBD**      |
| **TOTAL PCB COST**             | TBD      | **TBD**        | **TBD**      |

---

### **🖨️ 3D Printing Cost Breakdown** <a id="3d-printing-cost"></a>
| 3D Printed Parts                          | Quantity | Unit Price ($) | Total ($)   |
|-------------------------------------------|----------|----------------|---------------|
| **1000g filament (PLA and PLA-CF)**       | TBD        | **TBD**      | **TBD**     |
| **TOTAL 3D PRINTING COST**                | TBD        | **TBD**      | **TBD**     |

---

### **🔩 Other Materials (Screws, Nuts, and Miscellaneous)** <a id="other-materials-cost"></a>
| Material                       | Quantity | Unit Price ($) | Total ($)   |
|--------------------------------|----------|--------------|---------------|
| **M3 Screws & Nuts Set**       | TBD      | **TBD**      | **TBD**       |
| **Wiring & Connectors**        | TBD      | **TBD**      | **TBD**       |
| **TOTAL OTHER MATERIALS COST** | TBD      | **TBD**      | **TBD**       |

---

### **💵 TOTAL** <a id="total-cost"></a>
| Category                      | Total Cost ($) |
|-------------------------------|----------------|
| **Components**                | **TBD**     |
| **PCB (JLCPCB + Components)** | **TBD**     |
| **3D Printing**               | **TBD**      |
| **Other Materials**           | **TBD**       |
| **TOTAL PROJECT COST**        | **TBD**     |

**Prices are approximate, based on current market prices.*

---

## 📂 Resources <a id="resources"></a>

Below is a list of **external images** used in this repository.


---

## 📜 License <a id="license"></a>
```
License – All Rights Reserved

Copyright (c) 2026 **Popescu Filimon Andrei Cosmin**
All rights reserved.

This software and all associated files are the exclusive property of the copyright holder.

* You **may not copy, modify, merge, publish, distribute, sublicense, or sell** any part of this software without prior written permission.
* The software may only be viewed from this repository for personal and educational reference purposes.
* Any unauthorized use, reproduction, or distribution is prohibited and subject to legal action.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
```
