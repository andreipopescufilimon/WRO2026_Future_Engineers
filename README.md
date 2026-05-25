# WRO2026 Future Engineers – HyperLine Robotics Team
---

Repository of HyperLine Robotics Team competing in the **World Robot Olympiad (WRO) 2026**, Future Engineers category. 

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

<table align="center" cellspacing="12">
<tr>
  <td bgcolor="#ffffff" align="center" width="200">
    <img src="https://hyperlinerobotics.com/assets/images/sponsors/openmv.avif" height="55">
  </td>
  <td bgcolor="#ffffff" align="center" width="200">
    <img src="https://hyperlinerobotics.com/assets/images/sponsors/EasyEDA_logo.png" height="55">
  </td>
  <td bgcolor="#ffffff" align="center" width="200">
    <img src="https://hyperlinerobotics.com/assets/images/sponsors/OSHWLab.png" height="55">
  </td>
  <td bgcolor="#ffffff" align="center" width="200">
    <img src="https://static.lcsc.com/feassets/pc/images/headIcons/logo-s.png" height="55">
  </td>
</tr>

<tr>
  <td bgcolor="#ffffff" align="center">
    <img src="https://hyperlinerobotics.com/assets/images/sponsors/ING.png" height="45">
  </td>
  <td bgcolor="#ffffff" align="center">
    <img src="https://hyperlinerobotics.com/assets/images/sponsors/FundatiaComunitaraBucuresti.png" height="55">
  </td>
  <td bgcolor="#ffffff" align="center">
    <img src="https://hyperlinerobotics.com/assets/images/sponsors/SoBotz-white.png" height="55">
  </td>
  <td bgcolor="#ffffff" align="center">
    <img src="https://jsumo.com/Data/EditorFiles/jsumo-robot-parts-logo.png" height="55">
  </td>
</tr>

<tr>
  <td bgcolor="#ffffff" align="center">
    <img src="https://hyperlinerobotics.com/assets/images/sponsors/evergreen.png" height="50">
  </td>
  <td bgcolor="#ffffff" align="center">
    <img src="https://fae-drones.com/wp-content/uploads/2022/08/logo-vertical-ro.png" height="55">
  </td>
  <td></td>
  <td></td>
</tr>
</table>


---

## 📚 Table of Contents

- [👥 The Team](#the-team)
- [🎯 Challenge Overview](#challenge-overview)
- [🤖 Our Robot](#our-robot)
- [📊 Performance Metrics](#performance-metrics)
- [⚖️ Engineering Trade-Offs](#engineering-tradeoffs)
- [🔬 Testing & Iterations](#testing-iterations)
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
  - [📷 OpenMV RT1062 Camera](#openmv-rt1062-camera)
  - [⚙️ Drive Motor](#drive-motor)
  - [⚙️ Motor Driver IFX9201SG](#motor-driver)
  - [🔄 Steering Servo MG90S](#steering-servo)
  - [⚙️ Impeller](#impeller)
  - [📏 Pololu PWM Distance Sensor](#distance-sensors)
  - [🔌 Voltage Regulator D24V50F5](#voltage-regulator)
  - [🛠️ PCB Design](#pcb-design)
  - [⚡ Power Consumption](#power-consumption)
- [💻 Components coding](#components-coding)
  - [🧠 Software Architecture](#software-architecture)
  - [🔄 Robot State Machine](#state-machine)
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
- [📂 Resources](#resources)
- [📜 License](#license)

## 📂 Folder Structure


This repository is organized as follows:


```
📦 WRO2026_Future_Engineers
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

### **Popescu Filimon Andrei Cosmin**
**Age:** 18 <br>

**High School:** International Computer High School Bucharest (ICHB)  

**Description:**  
Hi! I’m Andrei from Romania, and this is my sixth WRO season. This is my second season in Future Engineers, as before I competed in the Robomission category. I am passionate about robotics, especially electronics and the latest algorithms and tech. Over the years, I have worked on multiple robotics projects, including line followers, sumo bots, and air quality modules. I am the founder of HyperLine Robotics Team, where I build and develop advanced robotics projects with my team. Apart from robotics, I also enjoy cybersecurity, programming, and cycling.

---

### **Horia Simion**
**Age:** 17 <br>
**High School:** National College "Mihai Viteazul" (CNMV)

**Description:**  
Hi! I’m Horia from Romania, and this is my second WRO season competing alongside Andrei. I have participated in RoboMission multiple times, gaining valuable experience in solving various problems that may arise while building a robot. I have a strong interest in technology and robotics and am always eager to learn and experiment with new ideas.

---

### **Eduard Alexandru Vasile**
**Age:** 16 <br>
**High School:** Liceul Teoretic Alexandru Ioan Cuza

**Description:**  
**to be completed*

---

## 🎯 Challenge Overview <a id="challenge-overview"></a>

The **WRO 2026 Future Engineers** challenge pushes teams to develop a **fully autonomous vehicle** capable of navigating a **dynamic and randomized racetrack** using **sensors, computer vision, and advanced control algorithms**. The goal is to complete **multiple laps** while adapting to randomized obstacles, following **strict driving rules**, and successfully executing a **parallel parking maneuver** at the end of the course.

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


Robot Specifications:
| Parameter | Value |
|---|---|
| Robot Length | ~145 mm |
| Robot Width | ~65 mm |
| Robot Height | ~55 mm |
| Total Weight | ~200 g |
| Wheelbase | ~65 mm |
| Ground Clearance | ~6 mm |
| Main Controller | Arduino Nano ESP32 |
| Vision System | OpenMV RT1062 |
| Drive System | Rear differential drive |
| Steering System | Parallel steering |
| Drive Motor | Pololu 30:1 HPCB Gearmotor |
| Steering Servo | MG90S |
| Downforce System | 10000KV BLDC Impeller |
| Battery | 2S Li-Po 300mAh |
| Maximum Stable Speed | ~3 m/s |
| Camera Processing Speed | ~40 FPS |
| Maximum Steering Angle | ~85° |
| Peak Current Consumption | ~7 A |
| Communication Protocols | UART, I2C, PWM |
| Chassis Type | Custom PCB chassis |
| Wheel Material | Cast silicone tires |

The robot was designed to prioritize stability, fast steering response, and compact packaging. A lightweight PCB chassis combined with silicone tires and an active downforce system allows the robot to maintain traction and consistency during high-speed autonomous runs.

---

## 📊 Performance Metrics <a id="performance-metrics"></a>

The robot was tested extensively on full-scale WRO-inspired tracks to evaluate stability, obstacle handling, and parking consistency. Multiple runs were performed under different lighting conditions and track layouts to validate reliability and repeatability.

| Metric | Result |
|---|---:|
| Maximum stable speed | 3 m/s |
| Average lap time | 11-12 s |
| Parking success rate | 90% |
| Obstacle avoidance success rate | 95% |
| Average steering correction delay | <10 ms |
| Camera processing speed | ~40 FPS |
| Maximum steering angle | 85° |
| Full system peak current | ~7 A |

The final tuning focused on balancing speed and stability rather than maximizing raw speed alone. Particular attention was given to repeatable obstacle avoidance and consistent parking performance.

---

## ⚖️ Engineering Trade-Offs <a id="engineering-tradeoffs"></a>

Throughout development, several engineering decisions had to balance performance, reliability, complexity, and weight.

| Decision | Chosen Solution | Reason |
|---|---|---|
| Main controller | Arduino Nano ESP32 | Compact size, high performance, integrated wireless features, used for debuging |
| Vision system | OpenMV RT1062 | Lower latency and lower power consumption compared to Raspberry Pi |
| Chassis structure | PCB chassis | Reduced wiring, improved rigidity, better space optimization |
| Steering system | Parallel steering | Simpler and lighter than Ackermann steering |
| Navigation | Gyro-based PD control | More stable heading correction compared to encoder-only control |
| Tires | Silicone tires | Better grip and more predictable handling |
| Downforce system | Impeller | Increased traction without adding weight |
| Obstacle detection | Camera-based detection | Allowed flexible handling of randomized obstacles |

The final robot design focused on achieving reliable autonomous behavior while maintaining a lightweight and compact structure optimized for high-speed operation.

---

## 🔬 Testing & Iterations <a id="testing-iterations"></a>

During development, multiple robot revisions were designed and tested to improve steering precision, reliability, and overall track performance.

| Version | Problem Identified | Improvement Made |
|---|---|---|
| V1 | Fully 3D-printed chassis was too large, steering precision was poor, and power management was unstable | Redesigned the entire robot structure and moved to a custom PCB chassis |
| V2 | Robot was reliable but too long for tight turns and obstacle sections | Redesigned steering geometry and optimized wheelbase dimensions |
| V3 | Current Robot | - |

Each iteration was tested on the track field. Testing focused on steering consistency, obstacle handling, parking precision, and stability during high-speed runs.

---

## 🚗 Mobility Management <a id="mobility-management"></a>

The robot's mobility is controlled through **a fully PCB chassis, a servo-based steering system, and a drivetrain featuring a custom differential and axes**. These components work together to ensure smooth, **precise movement with optimized traction, stability, and efficient power management**.

## ⚙️ **Drivebase** <a id="drivebase"></a>

### 🔧 **Drivetrain** <a id="drivetrain"></a>

The drivetrain uses a custom micro-RC differential at the rear, driven by a **Pololu 30:1 HPCB micro gearmotor** through a **3D printed gear** → **differential input gear**. The motor is held in a 3D-printed support with the battery mounted above, keeping the center of mass centered and low. Rear outputs rotate in bearings seated inside a printed part that contains 4 bearings for the wheel axles, this attachment is mounted using M2 screws to the PCB chassis, minimizing friction and parts count. 
At the front, the wheels are fully 3D-printed and each wheel runs on two bearings (inner + outer) for a rigid, wobble-free hub that steers precisely.

<img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/bd5e65719f69d3f668c65852b8c334ef36fecc77/other/differential.jpeg" width="600">

To maximize grip on the track without adding mass, we use a **downforce impeller (10000KV BLDC motor)** that pulls air from under the robot, increasing the normal force. The impeller is PWM-controlled via an **EMAX Bullet 20A ESC**. The drive motor is controlled by an **IFX9201SG driver (PWM + DIR)** with an active brake pulse for precise stopping, while the encoder on the gearmotor provides odometry for short, accurate moves (e.g., avoidance hops, parking nudges).

### ⚙️ **Motor – 30:1 Micro Metal Pololu Gearmotor HPCB** <a id="motor"></a>

Following past testing, we selected **a high-power 30:1 Micro Metal Gearmotor (6V)** for the drive system. This motor provides an **optimal balance of speed and torque**, allowing the robot to maintain stability while navigating turns.

| <img src="https://a.pololu-files.com/picture/0J12418.220.jpg?8f026fe1675b1109ea574290d3d26081" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Model:** 30:1 HPCB | **Voltage:** 6V |
| **No-load Speed:** 1000 RPM | **No-load Current:** 120mA |
| **Stall Torque:** ~0.45 kg·cm | **Stall Current:** 1.6A |
| 🔗 **[Buy Here](https://www.pololu.com/product/5184)** | **Function:** Drives the robot |

**Why We Chose This Motor?**  
**- Gear ratio provides sufficient torque** without sacrificing efficiency.  
**- Compact and lightweight design**, allowing integration into a lightweight robot.

### 🛞 Wheels & Tires (Silicone) <a id="wheels"></a>

Our robot uses cast silicone tires on 3D-printed hubs. Silicone provides high, repeatable static friction on painted boards and vinyl, which pairs perfectly with the rear differential and downforce impeller—more grip when we need it, without adding mass.

**Why silicone?**
- Grip & consistency: Strong adhesion on smooth surfaces → faster exits and shorter braking distances.
- Low wear on the map: Softer compound grips without scratching.
- Stable handling: Predictable breakaway, so PD/PID steering stays smooth.

**Construction:**
- Rims: 3D-printed hubs with dual bearings (inner + outer) for a rigid, wobble-free wheel.
- Tire: Cast silicone ring fitted onto a mechanical bead on the rim (no harsh solvents needed).
- Fit: Rear wheels mount directly to the diff axle outputs; fronts ride on steering hubs for low friction.


### 🔌 **IFX9201SG Motor Driver** <a id="motor-driver"></a>

The **IFX9201SG** motor driver is used to control the robot’s high-performance drive or impeller motor with precision and efficiency. It supports PWM-based speed control and direction control while integrating advanced protection features, making it ideal for demanding robotics applications. The IFX9201SG is directly integrated into our PCB, ensuring compact design and reliable communication with the Arduino Nano ESP32.

| <img src="https://assets.lcsc.com/images/lcsc/900x900/20230316_Infineon-Technologies-IFX9201SG_C112633_front.jpg" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Model:** IFX9201SG | **Operating Voltage:** 5.5V – 45V |
| **Logic Voltage:** 3.3V / 5V compatible | **PWM Frequency:** Up to 20 kHz |
| **Max Continuous Current:** 5A | **Max Peak Current:** 8A per channel |
| **Control Interface:** PWM + Direction pins | **Built-in Protections:** Overtemperature, Overcurrent, Undervoltage, Short-to-GND/Battery |
| 🔗 **[Buy Here](https://www.lcsc.com/product-image/C112633.html)** | **Function:** Controls drive motors |

### **⚙️ Impeller for downforce** <a id="impeller">

The **impeller** generates downforce to improve the robot’s grip on the track at high speeds. Powered by a **1020 coreless DC motor**, it delivers extremely high RPM with minimal weight, making it ideal for competitive line follower and robotracer builds. Its low rotor inertia ensures instant acceleration, while the compact size allows for easy integration.

| <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/bd5e65719f69d3f668c65852b8c334ef36fecc77/other/impeller.jpeg" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Type:** BLDC Motor | **Model:** 10000KV |
| **Voltage:** 2–3S | **Shaft Diameter:** 1.0mm |
| **No-Load Speed:** ~85,000 RPM @ 3.7V	 | **	Weight:** ~4.5g |
| **Current Draw (Avg):** ~1A @ 3.7V	 | **Peak Current:** ~2.5A |
| 🔗 **[Buy Here](https://www.aliexpress.com/item/1005010313915123.html)** | **Function:** Drives the downforce impeller |

---


## 🔄 **Steering** <a id="steering"></a>

The **steering system** is a critical part of the robot, ensuring precise maneuverability and smooth turns. Our design is based on a **parallelogram steering mechanism**, where both front wheels turn at the same angle through a single servo-controlled linkage. This setup provides predictable and stable steering, making it easy to use for an autonomous vehicle. Instead of using an **Ackermann steering system**, which requires more complex calculations and linkages, we opted for a **simpler and more lightweight solution** that offers consistent control. Our steering system allows for a maximum turning angle of 80 degrees in both the left and right directions. This range provides precise maneuverability, enabling the robot to navigate sharp turns efficiently while maintaining stability.

<img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/7127f1dabd068adfad747dd9a046f9f305c5eda3/other/steering.gif" width="500">

Our **steering arm is directly connected to the servo**, which moves the two front wheels simultaneously. This ensures that the turning response is immediate and proportional to the servo's motion. The **wheels are mounted on special mounts hubs**, allowing for smooth and precise movement without excessive friction. To ensure **structural integrity and long-term reliability**, the steering system is **assembled using M2 screws on which the hubs can turn**. 

**to add images*

---

### **🔄 Steering Servo – MG90S** <a id="servo-motor"></a>

To control the steering system, we use an **MG90S micro servo**, known for its high torque and fast response. This servo enables quick and precise adjustments while maintaining a compact and lightweight design. Featuring **metal gears**, it ensures durability and reliability over extended use. The servo is securely mounted onto the chassis with two screws, and the steering arm is directly attached to its output shaft, providing smooth and efficient motion transfer for accurate steering.

| <img src="https://static.optimusdigital.ro/20565-large_default/mg90s-servomotor.jpg" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Model:** MG90S | **Voltage:** 5V |
| **Torque:** 2.2kg/cm | **Signal Type:** PWM |
| **Current Draw (Avg):** 120mA | **Peak Current:** 500mA |
| **Weight:** ~13.4g | **Gears:** Plastic |
| 🔗 **[Buy Here](https://www.optimusdigital.ro/ro/motoare-servomotoare/271-servomotor-mg90s.html?srsltid=AfmBOooTrDsx2UoJ3Px8J26kkCbcuYhlpKYmuIYkivK_5ZSzPJx0ZNo8)** | **Function:** Controls steering |

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

The robot's **power and sensing system** is designed for **efficiency, precision, and adaptability**. It consists of an **Arduino Nano ESP32, a Li-Po battery, custom BMI088 IMU, OpenMV RT1062 Camera, motors(drive motor and servo motor), and a voltage regulator**, all working together to ensure **stable operation and accurate navigation**.  

Each component has been carefully selected to provide **optimal performance**, minimize power consumption, and ensure **reliability in competition environments**.

---

### **🔋 Li-Po Battery – 2S 300mAh** <a id="li-po-battery"></a>

The **Li-Po battery** provides a **compact, lightweight, and high-discharge** power source, making it ideal for our robot. The **7.4V output** is suitable for running our **motors and voltage regulator**, ensuring stable power delivery.

| <img src="https://img-va.myshopline.com/image/store/2000408386/1640672930478/GNB3002S60AHV-(4)_1800x.jpeg?w=1000&h=1000" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Model:** 2S Li-Po | **Capacity:** 300mAh |
| **Voltage:** 7.4V | **Discharge Rate:** 60C |
| **Weight:** 12g | **Size:** 13.8 x 12 x 52.5mm |
| **Output Current:** Varies by load | **Connector Type:** JST |
| 🔗 **[Buy Here](https://www.gaoneng.shop/products/gaoneng-gnb-lihv-2s-7.6v-300mah-60c-jst-lipo-battery)** | **Function:** Powers the entire robot |


---

### **🖥️ Arduino Nano ESP32 – Main Controller** <a id="arduino-nano-esp32">

The **Arduino Nano ESP32** provides **high-speed processing, built-in Wi-Fi and Bluetooth, and extensive GPIO capabilities** in a **compact form**. This allows it to handle **sensor data, motor control, and vision processing efficiently**.

| <img src="https://store.arduino.cc/cdn/shop/files/ABX00092_01.iso_804x603.jpg?v=1727101612" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Microcontroller:** ESP32 | **Flash Memory:** 4MB |
| **SRAM:** 520KB | **Frequency:** 240MHz |
| **Pins:** 22 | **Input Voltage:** 5V |
| **Current Draw (Avg):** 200mA | **Peak Current:** 500mA |
| 🔗 **[Buy Here](https://store.arduino.cc/en-ro/products/nano-esp32)** | **Function:** Controls all robot components |

---

### 🧭 Custom IMU Sensor – BMI088 <a id="imu-sensor-bmi088"></a>

The **Custom BMI088 IMU** is used to **measure the robot's angular velocity and acceleration**, helping it maintain **stability and precise movement control**. It is essential for **calculating turns and avoiding drift**. We decided to customize it to be able to make it more compact.

| <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/2f218ef03b6aa341f2fc0c9e151cf439fa750abd/other/IMU.png" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Gyroscope Range:** ±2000°/s | **Accelerometer Range:** ±24g |
| **Interface:** I2C / SPI | **Supply Voltage:** 3.0V – 3.6V |
| **Current Draw:** ~3.2mA | **Weight:** ~1g |
| 🔗 [View Open Source Project Here](https://oshwlab.com/bicicleta11/bmi088-gyro-module) | **Function:** Tracks orientation & motion |

---

### **📷 OpenMV RT1062 Camera – Vision Processing** <a id="openmv-rt1062-camera">

The **OpenMV Cam RT1062** handles **traffic sign recognition, lane detection, color tracking, and other visual cues** in real time. It includes its own high-performance microcontroller, so the Arduino can focus mainly on movement control while the camera processes images independently.

| <img src="https://openmv.io/cdn/shop/files/cam-v5-front-hero-2-web_1000x.jpg?v=1699323324" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Processor:** NXP i.MX RT1062 Cortex-M7 | **Frequency:** 600 MHz |
| **RAM:** 32 MB SDRAM + 1 MB SRAM | **Flash Memory:** 16 MB QSPI Flash |
| **Camera Sensor:** OV5640 | **Resolution:** 2592 × 1944, 5 MP |
| **Typical Vision Speed:** ~40 FPS at QVGA | **Interfaces:** UART, I2C, SPI, CAN, USB-C |
| **Connectivity:** Wi-Fi, Bluetooth 5.1, Ethernet | **I/O Voltage:** 3.3 V only |
| 🔗 **[Buy Here](https://openmv.io/collections/cameras/products/openmv-cam-rt?variant=42269003743326)** | **Function:** Detects traffic signs, lanes, and visual markers |

---

### **⚙️ Drive Motor – 30:1 Micro Metal Gearmotor HPCB 6V with Encoder** <a id="drive-motor">

The **drive motor is responsible for propelling the robot forward**. The **30:1 gearbox** provides an excellent **balance of speed and torque**, while the **built-in encoder** allows for **precise speed control**.

| <img src="https://a.pololu-files.com/picture/0J12418.220.jpg?8f026fe1675b1109ea574290d3d26081" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Model:** 30:1 HPCB | **Voltage:** 6V |
| **No-load Speed:** 1000 RPM | **No-load Current:** 120mA |
| **Stall Torque:** ~0.45 kg·cm | **Stall Current:** 1.6A |
| 🔗 **[Buy Here](https://www.pololu.com/product/5184)** | **Function:** Drives the robot |

---

### **⚙️ IFX9201SG Motor Driver** <a id="motor-driver"></a>

The **IFX9201SG** motor driver is used to control the robot’s high-performance drive or impeller motor with precision and efficiency. It supports PWM-based speed control and direction control while integrating advanced protection features, making it ideal for demanding robotics applications. The IFX9201SG is directly integrated into our PCB, ensuring compact design and reliable communication with the Arduino Nano ESP32.

| <img src="https://assets.lcsc.com/images/lcsc/900x900/20230316_Infineon-Technologies-IFX9201SG_C112633_front.jpg" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Model:** IFX9201SG | **Operating Voltage:** 5.5V – 45V |
| **Logic Voltage:** 3.3V / 5V compatible | **PWM Frequency:** Up to 20 kHz |
| **Max Continuous Current:** 5A | **Max Peak Current:** 8A per channel |
| **Control Interface:** PWM + Direction pins | **Built-in Protections:** Overtemperature, Overcurrent, Undervoltage, Short-to-GND/Battery |
| 🔗 **[Buy Here](https://www.lcsc.com/product-image/C112633.html)** | **Function:** Controls drive motors |

---

### **🔄 Steering Servo – MG90S** <a id="steering-servo">

The **MG90S servo is used for precise steering control**, enabling the robot to **navigate turns with accuracy**. It provides **high torque output in a compact size**.

| <img src="https://static.optimusdigital.ro/20565-large_default/mg90s-servomotor.jpg" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Model:** MG90S | **Voltage:** 5V |
| **Torque:** 2.2kg/cm | **Signal Type:** PWM |
| **Current Draw (Avg):** 120mA | **Peak Current:** 500mA |
| **Weight:** ~13.4g | **Gears:** Plastic |
| 🔗 **[Buy Here](https://www.optimusdigital.ro/ro/motoare-servomotoare/271-servomotor-mg90s.html?srsltid=AfmBOooTrDsx2UoJ3Px8J26kkCbcuYhlpKYmuIYkivK_5ZSzPJx0ZNo8)** | **Function:** Controls steering |

---

### **⚙️ Impeller for downforce** <a id="impeller">

The **impeller** generates downforce to improve the robot’s grip on the track at high speeds. Powered by a **1020 coreless DC motor**, it delivers extremely high RPM with minimal weight, making it ideal for competitive line follower and robotracer builds. Its low rotor inertia ensures instant acceleration, while the compact size allows for easy integration.

| <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/bd5e65719f69d3f668c65852b8c334ef36fecc77/other/impeller.jpeg" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Type:** BLDC Motor | **Model:** 10000KV |
| **Voltage:** 2–3S | **Shaft Diameter:** 1.0mm |
| **No-Load Speed:** ~85,000 RPM @ 3.7V	 | **	Weight:** ~4.5g |
| **Current Draw (Avg):** ~1A @ 3.7V	 | **Peak Current:** ~2.5A |
| 🔗 **[Buy Here](https://www.aliexpress.com/item/1005010313915123.html)** | **Function:** Drives the downforce impeller |

---

### 📏 Distance Sensor – Pololu PWM Distance Sensor <a id="distance-sensors"></a>

The **Pololu Digital Distance Sensor (PW output, 300 cm max)** uses a short-range lidar module and reports distance as the width of a digital pulse (similar to a hobby-servo signal). It’s ideal for reliable, fast obstacle detection and gives you an actual distance reading (3 mm resolution), perfect for lap direction detection at start and for close-range wall sensing.

| <img src="https://a.pololu-files.com/picture/0J11135.1200.jpg?1910ced553e34153046a4c95021a93b3" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Detection Range:** 300 cm (depends on surface) | **Type:** Digital pulse width (HIGH-time encodes distance) |
| **Voltage Supply:** 3.0V – 5.5V | **Current Draw:** ~30 mA enabled, ~0.4 mA disabled (EN low) |
| **Resolution:** 3 mm (≈ 4 µs per 3 mm)	 | **Update Rate:** ~50–110 Hz (period ~20–9 ms) |
| **Dimensions:** 21.6mm × 8.9mm × 3.1mm | **Weight:** 1g |
| 🔗 [Buy Here](https://www.pololu.com/product/4064) | **Use Case:** Start-line lap direction & close-range obstacle sensing for parking |

---

### **🔌 D24V50F5 Voltage Regulator – Power Management** <a id="voltage-regulator"> 

The **D24V50F5** regulates the **11.1V Li-Po battery output** to a **stable 5V**, ensuring **safe power delivery** to the **Arduino, sensors, and camera**. It prevents **overvoltage damage** and includes **thermal & short-circuit protection** for reliability.

| <img src="https://gomagcdn.ro/domains/robofun.ro/files/product/large/pololu-5v-5a-step-down-voltage-regulator-d24v50f5-633511-832-442353.jpg" width="300">  | **Specifications** |
|------------------------------|------------------------------|
| **Model:** D24V50F5 | **Input Voltage:** 6V – 38V |
| **Output Voltage:** 5V | **Output Current:** 5A |
| **Efficiency:** Linear Regulator | **Dropout Voltage:** ~2V |
| **Protection:** Short-circuit & thermal shutdown | **Mounting Type:** Pins |
| 🔗 **[Buy Here](https://www.pololu.com/product/2851)** | **Function:** Converts battery voltage to 5V |

---

### **🛠️ PCB Design & Electrical Architecture** <a id="pcb-design"></a>

| **Electrical Schematics** | **PCB Copper Traces** | **Final PCB Chassis** |
|---------------------------|---------------------------|---------------------------|
| <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/c1dc83879f053fdf2dd824ed671141c196b5ff17/other/SCH_Schematic1_1-P1_2026-05-25.png" width="400"> | <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/c1dc83879f053fdf2dd824ed671141c196b5ff17/other/PCB%20View.png" width="400"> | <img src="" width="400"> |

```txt
                2S Li-Po Battery
                        │
        ┌───────────────┴───────────────┐
        │                               │
   Drive Motor                    Impeller ESC
        │                               │
   IFX9201SG                      BLDC Motor
        │
   D24V50F5 Regulator
        │
 ┌──────┼────────┬────────┬────────┐
 │      │        │        │        │
ESP32  OpenMV   IMU    Servo   Distance Sensors
```

The robot uses a fully custom PCB chassis designed to combine both the electrical system and the structural frame into a single compact platform. This approach reduced weight, improved rigidity, and increased overall reliability during high-speed autonomous runs.

### **Advantages of the Custom PCB Chassis**
- Cleaner construction with fewer wires and connectors  
- More stable power delivery for motors and sensors  
- Better reliability during high-speed runs  
- Compact design with improved weight distribution  
- Reduced electrical interference on communication lines  
- Stronger and more rigid compared to multi-part printed structures

### **🔧 Engineering Decisions**
The PCB was designed with separated high-current and logic-power areas to reduce electrical noise generated by the motors and impeller system. Large copper pours were also used to improve current handling and passive heat dissipation.

Several PCB revisions were tested before the final version, improving:
- Motor power routing
- Sensor stability
- Weight distribution
- Connector accessibility
- Mechanical resistance

---

### **⚡ Power Consumption Summary** <a id="power-consumption"></a>

| Item / Designators | Part | Supply | Typical Current | Peak Current | Notes |
|------------------------------------|-------------------------------|----------------|------------------|----------------|---------------------------------------------|
| Main Controller | Arduino Nano ESP32 | 5V | ~200mA | ~500mA | Main processing unit |
| Vision Processing | OpenMV Cam RT1062 | 5V | ~300mA | ~500mA | Traffic sign & lane detection |
| Gyroscope / IMU | Custom BMI088 Module | 3.3V | ~3.2mA | ~5mA | Orientation and angular velocity |
| Drive Motor | Pololu 30:1 HPCB Gearmotor | 6V | ~120mA | ~1.6A | Peak occurs during acceleration/stall |
| Steering System | MG90S Servo | 5V | ~120mA | ~500mA | High peaks during fast steering |
| Downforce System | 10000KV BLDC Impeller + ESC | 7.4V | ~1.5A | ~4A | Generates aerodynamic downforce |
| Distance Sensor #1 | Pololu PWM Distance Sensor | 5V | ~30mA | ~35mA | Front obstacle detection |
| Distance Sensor #2 | Pololu PWM Distance Sensor | 5V | ~30mA | ~35mA | Side obstacle detection |
| Motor Driver | IFX9201SG | 5V Logic / VBAT | ~10mA | ~20mA | Logic current only |
| Voltage Regulator | D24V50F5 | 7.4V → 5V | Depends on load | Depends on load | Powers 5V electronics |
| Total Estimated Consumption | Entire Robot | 7.4V Battery | ~2.3A | ~7A | Worst-case peaks during acceleration |

The robot was designed with power efficiency and peak-current stability in mind. High-current systems such as the impeller and drive motor are electrically separated from sensitive logic components to reduce electrical noise and voltage drops during acceleration. Large copper pours and short power paths on the PCB help improve current handling and thermal dissipation.

---

## 💻 Components coding <a id="components-coding"></a>

### 🧠 Software Architecture <a id="software-architecture"></a>

This diagram below, shows how the main software modules interact during a run. The OpenMV RT1062 handles image processing and sends compact commands through UART, while the Arduino Nano ESP32 manages motion control, state logic, steering, motor speed, and parking.

The software is split into independent modules so each subsystem can be tested separately. Vision data is used by the obstacle and parking logic, while the gyro PID keeps the robot stable between decisions. Final movement commands are sent to the steering servo and IFX9201SG motor driver.

```text
          OpenMV RT1062
        (Vision Processing)
                 │ UART
                 ▼
        Arduino Nano ESP32
   ┌─────────────┼─────────────┐
   │             │             │
 Gyro PID   Obstacle Logic   Parking Logic
   │             │             │
   └──────► Steering Control ◄─┘
                    │
             MG90S Servo
                    │
             Drive Motor PID
                    │
              IFX9201SG
```

### 🔄 Robot State Machine <a id="state-machine"></a>

The robot software is organized as a state machine. Each state has a specific role, and transitions happen only when a clear sensor event or timing condition is detected. This makes the robot easier to debug, safer during runs, and more reliable in the randomized WRO track.

```text
WAIT_START
    ↓ start button pressed
PID_DRIVE
    ├── cube detected far away ─────► FOLLOW_CUBE
    ├── black wall / turn point ────► TURN_90
    ├── 12 turns completed ─────────► PARKING
    ↓
FOLLOW_CUBE
    ├── cube close enough ──────────► AVOID_CUBE
    └── cube lost timeout ──────────► PID_DRIVE
    ↓
AVOID_CUBE
    ↓ avoidance movement completed
AFTER_CUBE
    ↓ robot realigned
PID_DRIVE
    ↓ final lap completed
PARKING
    ↓ robot fully inside parking area
STOP
```

| State | Purpose | Main Inputs | Output |
|---|---|---|---|
| `WAIT_START` | Keeps the robot stopped before the round begins | Start button | Starts autonomous run |
| `PID_DRIVE` | Drives straight using gyro-based PD correction | Gyro, camera line/wall signals | Steering + motor speed |
| `FOLLOW_CUBE` | Follows the detected red/green cube before avoidance | OpenMV `S±value` command | Servo correction toward cube |
| `AVOID_CUBE` | Passes the cube on the correct side based on color | OpenMV `RED` / `GREEN` command | Fixed avoidance maneuver |
| `AFTER_CUBE` | Realigns the robot after avoiding a cube | Gyro angle, timeout | Returns to stable driving |
| `TURN_90` | Performs a 90° turn when a corner is detected | Black wall signal, turn direction | Updates target gyro angle |
| `PARKING` | Executes the final parallel parking routine | Distance sensors, gyro | Parks inside the parking area |
| `STOP` | Ends the run safely | Parking complete | Motor off, steering centered |


Using a state machine prevents different behaviors from interfering with each other. For example, while the robot is parking, camera commands are ignored, and while it is avoiding a cube, the normal driving PID is temporarily paused. This makes the behavior more predictable and easier to test during repeated runs.

### ⚡ Drive motor <a id="drive-motor-coding"></a>

The motor driver is based on the **Infineon IFX9201SG**, which allows us to directly manage the motor with only two control signals: a **PWM** pin that regulates speed, and a direction pin that selects forward or reverse rotation. Thanks to this chip’s integrated design, no external library was required for motor control.

We implemented a set of functions within our control system: one to initialize the driver, one to control motor velocity, and another to stop it effectively using an active braking routine. The function **move(int speed)** takes an input in the range **−255** to **+255**. The absolute value of the input is written to the **PWM channel**, while the sign determines the motor’s rotation direction. A dedicated **stop_motor()** function applies a short reverse torque pulse before setting the PWM duty cycle to zero, ensuring the robot stops quickly without uncontrolled coasting.

Since the robot is powered by an **ESP32**, **PWM** signals must be generated using the **LEDC** hardware utility, which provides stable and precise duty cycles.

```cpp
void motor_driver_setup() {
  pinMode(PWMA, OUTPUT);
  pinMode(DIRA, OUTPUT);
  ledcSetup(PWM_MOTOR_CHANNEL, PWM_MOTOR_FREQ, PWM_MOTOR_RESOLUTION);
  ledcAttachPin(PWMA, PWM_MOTOR_CHANNEL);
  move(0);
}

void move(int speed) {
  ledcWrite(PWM_MOTOR_CHANNEL, abs(speed));
  digitalWrite(DIRA, speed > 0 ? LOW : HIGH);
}

void stop_motor() {
  move(-3);
  delay(100);
  move(0);
}
```

For odometry, the motor includes an encoder. We implemented an interrupt-based approach to count ticks in real time, avoiding the need for a dedicated library. The encoder setup attaches an interrupt on the rising edge of channel A, and the distance traveled is computed from tick counts.

In this configuration, the encoder provides 12 counts per revolution, and by applying the gear ratio, wheel diameter, and π, we obtain a conversion factor of `MM_PER_TICK = 1.8326`. This enables the robot to measure traveled distance with centimeter-level accuracy, which is crucial for precise maneuvers during navigation.

```cpp
void encoder_setup() {
  // Enable pull-ups so A/B never float
  pinMode(ENCODER_A, INPUT_PULLUP);
  pinMode(ENCODER_B, INPUT_PULLUP);
  // Attach only A’s rising edge:
  attachInterrupt(digitalPinToInterrupt(ENCODER_A), encoderISR, RISING);
  encoder_ticks = 0;
}

float read_cm() {
  noInterrupts();
  int32_t t = encoder_ticks;
  interrupts();

  float dist_mm = t * MM_PER_TICK;
  float dist_cm = dist_mm / 10.0f;
  //Serial.printf("ticks=%ld  dist_cm=%.2f\n", t, dist_cm);
  return dist_cm;
}
```

### 🌪️ Impeller <a id="impeller-coding"></a>

The impeller is responsible for generating downforce, which increases stability and traction when the robot operates at high speed. To control it, we use an **RFR3411 MOSFET**, wired as a low-side switch. This **MOSFET** can safely handle the high current required by the impeller, while allowing speed control via **PWM** modulation.

Only a single PWM pin from the ESP32 is needed to drive the MOSFET gate. The PWM duty cycle determines how much power is applied to the impeller:

```
0 = off
255 = full speed
```

Two functions were implemented for simplicity: one for initializing the PWM channel and one for setting the impeller speed.

```cpp
void impeller_setup() {
  pinMode(PWM_IMPELLER, OUTPUT);
  ledcSetup(PWM_IMPELLER_CHANNEL, PWM_IMPELLER_FREQ, PWM_IMPELLER_RESOLUTION);
  ledcAttachPin(PWM_IMPELLER, PWM_IMPELLER_CHANNEL);
}

void setImpeller(int _pwm) {
  ledcWrite(PWM_IMPELLER_CHANNEL, constrain(_pwm, 0, 255));
}
```

### 🎮 Servo motor <a id="servo-motor-coding"></a>

The steering system is controlled by a **MG90S servo**, which adjusts the angle of the front wheels. This enables the robot to follow precise paths while maintaining stability during turns.

The servo is connected directly to the **ESP32** and driven using the `ESP32Servo.h` library interface. To ensure consistent and safe motion, we defined three reference positions:

```
STEERING_LEFT = 140
STEERING_CENTER = 85
STEERING_RIGHT = 40
```

These values were experimentally calibrated to match the geometry of the steering mechanism.

The function `steering_servo_setup()` attaches the servo to its pin, performs a left–right sweep to verify operation, and finally centers it. This ensures that the robot always starts in a straight-line configuration.

The function `steer(double steering_angle)` accepts an input in the range `[-1, 1]`, representing normalized steering:

```
-1 = maximum right
0 = centered
+1 = maximum left
```

The input is first clamped to stay within the allowed range, then mapped to the calibrated servo values using **map_double**. This abstraction allows higher-level controllers (such as PID) to provide normalized outputs without directly worrying about servo limits.

```cpp
void steering_servo_setup() {
  steeringServo.attach(STEERING_SERVO);
  delay(100);

  steeringServo.write(STEERING_LEFT);
  delay(300);
  steeringServo.write(STEERING_RIGHT);
  delay(300);
  steeringServo.write(STEERING_CENTER);
  delay(300);
}

void steer(double steering_angle) {
  // Clamp input between -1 and 1
  if (steering_angle > 1)  steering_angle = 1;
  if (steering_angle < -1) steering_angle = -1;

  // Map to servo range
  steering_angle = map_double(steering_angle, -1, 1, STEERING_LEFT, STEERING_RIGHT);
  steeringServo.write(steering_angle);
}
```

### 🧭 IMU <a id="imu-coding"></a>

To keep the robot on a straight path and execute precise turns, we rely on the **Bosch BMI088 Inertial Measurement Unit (IMU)**. This sensor combines a high-resolution accelerometer and gyroscope, and in our case we primarily use the **Z-axis** gyroscope for yaw angle estimation.

During initialization, the function `gyro_setup()` configures the BMI088 via **I²C** communication. The accelerometer is set to a **±6 g** range at **200 Hz**, while the gyroscope is set to **±2000 °/s** at **400 Hz** with a **47 Hz bandwidth**. These parameters provide a good balance between responsiveness and noise filtering.

A key challenge with gyroscopes is drift — the tendency of small measurement errors to accumulate over time. To correct this, the setup function performs a calibration routine: for a predefined duration **(DRIFT_TEST_TIME)**, the sensor’s output is averaged while the robot is stationary. The resulting bias **(drifts_z)** is subtracted from all subsequent readings.

```cpp
void gyro_setup() {
  // 1) Wire + basic init
  Wire.begin();
  imu.initialize();

  imu.setAccScaleRange(RANGE_6G);
  imu.setAccOutputDataRate(ODR_200);       // 200 Hz accel
  imu.setAccPoweMode(ACC_ACTIVE);

  imu.setGyroScaleRange(RANGE_2000);
  imu.setGyroOutputDataRate(ODR_400_BW_47); // 400 Hz, BW=47 Hz
  imu.setGyroPoweMode(GYRO_NORMAL);

  if (!imu.isConnection()) {
    if (debugGyro) {
      Serial.println("BMI088 connection failed!");
    }
    return;
  }

  if (debugGyro) Serial.println("Starting gyro drift calculation...");
  double start = millis();
  gyro_last_read_time = start;
  gz = 0;

  while (millis() - start < DRIFT_TEST_TIME * 1000) {
    double now = millis();
    double dt = (now - gyro_last_read_time) * 0.001; // s
    float rate = imu.getGyroscopeZ();                // °/s
    gz += rate * dt;                                 // accumulate degrees
    gyro_last_read_time = now;
  }

  drifts_z = gz / DRIFT_TEST_TIME;  // average °/s
  if (debugGyro) {
    Serial.print("Drift test done! drifts_z = ");
    Serial.print(drifts_z, 6);
    Serial.println(" °/s");
  }

  // reset integration
  gz = 0;
  gyro_last_read_time = millis();
} 
```

The function `read_gyro_data()` must be called continuously in the control loop. It computes the elapsed time **dt**, reads the current angular rate from the **BMI088**, subtracts the drift offset, and integrates the result into **gz**, which represents the robot’s absolute yaw angle in degrees. This value is used directly in the **PID controller** for heading correction.

```cpp
void read_gyro_data() {
  // call this frequently (e.g. every loop)
  double now = millis();
  double dt  = (now - gyro_last_read_time) * 0.001; // s          
  float rate = imu.getGyroscopeZ();              // °/s
  double corrected = rate - drifts_z;               // drift-compensated
  gz += corrected * dt;                             // integrate to degrees
  gyro_last_read_time = now;

  if (debugGyro) {
    Serial.print("Gyro angle (°): ");
    Serial.println(gz, 4);
  }
}
```


### 📡 Distance sensors <a id="distance-sensors-coding"></a>

The robot integrates four **Pololu PWM distance sensors**, placed on the **front, back, left, and right sides** of the chassis. Each sensor outputs a pulse width signal, where the duration is proportional to the measured distance. This gives us precise range data without requiring complex communication protocols.

The function `distanceSensorPin(DistanceDir d)` maps each logical direction (FRONT_DIR, LEFT_DIR, RIGHT_DIR, BACK_DIR) to its corresponding analog pin. Using `pulseIn()`, we measure the pulse width, and the helper function `pulseToMM()` converts it into millimeters based on the calibration constants:

```
PW_OFFSET_US – offset at ~0 mm (typically ~1000 µs)
PW_US_PER_MM – scaling factor (1 µs per mm)
PULSE_TIMEOUT_US – maximum waiting time for a valid pulse (30 ms)
```

If no valid pulse is received, the function returns **-1** to indicate a timeout.

```cpp
// ======= Pololu PWM Distance Sensor Settings =======
static const float PW_OFFSET_US   = 1000.0f; // microseconds offset at ~0 mm
static const float PW_US_PER_MM   = 1.0f;    // microseconds per millimeter
static const unsigned long PULSE_TIMEOUT_US = 30000UL; // 30 ms timeout

// ======= Map direction to the correct pin =======
static inline int distanceSensorPin(DistanceDir d) {
  switch (d) {
    case FRONT_DIR: return PWM_DIST_FRONT; // A0
    case LEFT_DIR:  return PWM_DIST_LEFT;  // A3
    case RIGHT_DIR: return PWM_DIST_RIGHT; // A1
    case BACK_DIR:  return PWM_DIST_BACK;  // A2
  }
  return PWM_DIST_FRONT;
}

// ======= Read pulse in microseconds =======
static inline unsigned long readPulseUS(int pin) {
  pinMode(pin, INPUT);
  return pulseIn(pin, HIGH, PULSE_TIMEOUT_US);
}

// ======= Convert pulse width to millimeters =======
static inline float pulseToMM(unsigned long pw_us) {
  if (pw_us == 0) return -1.0f; // timeout
  float mm = (float(pw_us) - PW_OFFSET_US) / PW_US_PER_MM;
  if (mm < 0) mm = 0;
  return mm;
}
```

### 📷 Camera <a id="camera-coding"></a>

Once the robot can move and steer, it also needs to see and react to the environment. For this, we use an **OpenMV camera module**, connected via **UART protocol**. This allows the camera to handle the heavy work of image processing and only send compact messages (like colors, cubes, or angles) to the **ESP32**.

The camera is wired to the **ESP32** as follows:
- Camera TX (P4) → ESP32 RX (D0)
- Camera RX (P5) → ESP32 TX (D1)

Both devices must share the same baud rate **(19200)**. On the **ESP32** side, we use `Serial0` for communication, while on the camera side, `UART(3, 19200)` is initialized.

**Arduino code:**

```cpp
// UART to OpenMV (SoftwareSerial on D0/D1)
String receivedMessage;

void comm_setup() {
  Serial.begin(9600);
  Serial0.begin(19200);          // must match OpenMV baudrate
  receivedMessage = "";
  // (optional) small flush
  while (Serial0.available()) Serial0.read();
}

void loop() {
  // ... control loop stuff ...

  // Execute pending commands from camera
  while (Serial0.available() > 0) {
    char c = Serial0.read();
    if (c == '\n') {
      // Optionally skip commands while parking
      if (CASE != PARK) {
        execute_command(receivedMessage); // handle "S<angle>", "BLUE", "ORANGE", "RED", "GREEN", "BLACK", "1", "2"
      }
      receivedMessage = "";
    } else {
      receivedMessage += c;
    }
  }
}
```

**Camera code:**

We use `UART(3, 19200)` on **OpenMV (P4=TX, P5=RX)**. Baud must match the Arduino side.

```python
from pyb import UART
uart = UART(3, 19200)
# 3 → uses P4 (TX) and P5 (RX)
# 19200 → baud rate, must match Arduino
```

Arduino listens and executes messages line-by-line (ending with `\n`). Camera only sends.

We configure the camera for `RGB565, QVGA (320×240)`, flipped to match mounting, and fixed exposure/gain/WB for consistent LAB thresholds.

```python
import sensor
import time
from machine import UART, LED

# -------- DEBUG FLAG --------
DEBUG = False

# -------- Camera & Sensor Setup --------
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)       # QVGA: 320x240

sensor.set_vflip(True)
sensor.set_hmirror(True)

# Disable auto settings for stable color tracking.
# RT1062 uses the OV5640 sensor, so you may need to retune thresholds/exposure.
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False, exposure_us=16000)

sensor.skip_frames(time=2000)
clock = time.clock()
```

Why fixed exposure? It keeps color thresholds stable across frames and lighting.

Color thresholds **(LAB)** for red/green cubes, blue/orange lines, black walls.
We also define ROIs to look only where each object should appear (faster + fewer false positives).

```python
# ---- LAB thresholds ----
red_threshold    = [(32, 54, 40, 67, 17, 63)]
green_threshold  = [(36, 69, -56, -21, -19, 32)]
blue_threshold   = [(9, 76, -45, 27, -57, -8)]
orange_threshold = [(62, 91, -3, 43, 5, 69)]
pink_threshold   = [(30, 70, 10, 60, -15, 15)]
black_threshold  = [(0, 37, -26, 7, -17, 11)]

# ---- Regions of Interest ----
img_h, img_w = sensor.height(), sensor.width()
cubes_roi      = (0, int(img_h * 0.4),  img_w,        int(img_h * 0.6))  # bottom area
lines_roi      = (5, int(img_h * 0.5),  img_w - 10,   int(img_h * 0.4))  # lower band
wall_roi       = (50, int(img_h * 0.5 - 18), img_w - 100, int(img_h * 0.2 - 10))
final_wall_roi = (30, 80, img_w - 60, int(img_h - 60))
```

Minimum areas to reject noise and a simple **PD** setup for cube following.

```python
min_cube_size       = 300
min_line_size       = 800
min_area            = 10
min_valid_cube_area = 450
black_wall_min_area = 7000
final_black_wall_min_area = 9000
min_black_height    = 39

kp_cube = 0.21
kd_cube = 2.4
pid_error = 0.0
pid_last_error = 0.0
follow_threshold = 4100

direction = 0  # 0 = unset, 1 = LEFT(blue), 2 = RIGHT(orange)

def get_largest_blob(blobs):
    return max(blobs, key=lambda b: b.area(), default=None)
```

Every frame, we:
- Detect walls (parking / turn signal) → send `"BLACK"`
- Detect cubes (red/green) → send `S<error>` for follow, or `RED/GREEN` when very close
- Detect lines (blue/orange) → send `"BLUE"/"ORANGE"` and set direction (1/2)
- Always send the current direction **("1"/"2")** for sync

```python
while True:
    clock.tick()
    img = sensor.snapshot()
    target_x = img_w // 2

    # ---- Walls / parking ----
    black_blobs = img.find_blobs(black_threshold, roi=final_wall_roi,
                                 pixels_threshold=final_black_wall_min_area,
                                 area_threshold=final_black_wall_min_area, merge=True)
    black_blob = get_largest_blob(black_blobs)
    if black_blob and black_blob.h() >= min_black_height:
        uart.write("BLACK\n")

    # ---- Cubes (red/green) ----
    red_blobs   = img.find_blobs(red_threshold,   roi=cubes_roi,
                                 pixels_threshold=min_cube_size,
                                 area_threshold=min_cube_size, merge=True)
    green_blobs = img.find_blobs(green_threshold, roi=cubes_roi,
                                 pixels_threshold=min_cube_size,
                                 area_threshold=min_cube_size, merge=True)

    red_cube   = get_largest_blob([b for b in red_blobs   if b.area() >= min_valid_cube_area])
    green_cube = get_largest_blob([b for b in green_blobs if b.area() >= min_valid_cube_area])

    candidates = []
    if red_cube:   candidates.append(('RED',   red_cube))
    if green_cube: candidates.append(('GREEN', green_cube))

    if candidates:
        color, cube = max(candidates, key=lambda x: x[1].area())
        # normalized horizontal error in [-1..1]
        error = (cube.cx() - target_x) / float(target_x)
        pid_error = kp_cube * error + kd_cube * (error - pid_last_error)
        pid_last_error = error

        if cube.area() < follow_threshold:
            uart.write("S{:+.3f}\n".format(error))   # steer hint for FOLLOW_CUBE
        else:
            uart.write(color + "\n")                 # close: just report color

    # ---- Lines (blue/orange) ----
    blue_blobs   = img.find_blobs(blue_threshold,   roi=lines_roi,
                                  pixels_threshold=min_line_size,
                                  area_threshold=min_line_size, merge=True)
    orange_blobs = img.find_blobs(orange_threshold, roi=lines_roi,
                                  pixels_threshold=min_line_size,
                                  area_threshold=min_line_size, merge=True)

    blue_line   = get_largest_blob([b for b in blue_blobs])
    orange_line = get_largest_blob([b for b in orange_blobs])

    chosen = None
    if blue_line and orange_line:
        chosen = "BLUE" if blue_line.area() > orange_line.area() else "ORANGE"
    elif blue_line:
        chosen = "BLUE"
    elif orange_line:
        chosen = "ORANGE"

    if chosen:
        uart.write(chosen + "\n")
        if direction == 0:
            direction = 1 if chosen == "BLUE" else 2

    # ---- Always send current turn direction ----
    uart.write(str(direction) + "\n")
```

---

## 📝 Obstacle Management <a id="obstacle-management"></a>

### 🏁 Open Round <a id="open-round"></a>
During the **Open Round**, our robot follows a **straight trajectory using a PID controller based on gyro yaw**, ensuring stable movement. To determine turns, the **camera detects Orange and Blue lines** on the track:
- **Orange Line ➜ Right Turn**
- **Blue Line ➜ Left Turn**
- The turn is executed when the robot reaches an approximate **distance from the front black wall**.

#### **Camera Processing for Line Detection**
The camera captures frames in **RGB565** format and processes color blobs using **LAB thresholds** to detect relevant track elements. Below is a **snippet of the camera’s core logic** that identifies **track lines and determines turning direction**:

```python
# Detect Blobs
red_blobs = img.find_blobs(red_threshold, roi=cubes_roi, pixels_threshold=min_cube_size, area_threshold=min_cube_size, merge=True)
green_blobs = img.find_blobs(green_threshold, roi=cubes_roi, pixels_threshold=min_cube_size, area_threshold=min_cube_size, merge=True)
blue_blobs = img.find_blobs(blue_threshold, roi=lines_roi, pixels_threshold=min_line_size, area_threshold=min_line_size, merge=True)
orange_blobs = img.find_blobs(orange_threshold, roi=lines_roi, pixels_threshold=min_line_size, area_threshold=min_line_size, merge=True)
black_blobs = img.find_blobs(black_threshold, roi=cubes_roi, pixels_threshold=min_cube_size, area_threshold=min_cube_size, merge=True)

...

# Determine Direction
if direction == 0:
    if orange_line and not is_invalid_orange(orange_line, red_blobs):
        direction = 2  # Orange line first ➜ turn right
    elif blue_line:
        direction = 1  # Blue line first ➜ turn left

# Send Direction Command
uart.write(str(direction) + '\n')
```

<img src="" width="900">
* to add images

### ⚡ Final Round <a id="final-round"></a>

In the final round, we extend our open rount algorithm by adding real‐time cube detection, following, and avoidance algorithms. The OpenMV RT1062 camera handles live frames processing and sends compact UART messages to the Arduino Nano ESP32. On the Arduino, incoming UART messages drive a four‐state algorithm: in **PID**, the robot adjust to hold a straight heading by using a PD on the gyro and turns 90° whenever it receives a **BLACK** signal (lap turning point in each corner). In **FOLLOW_CUBE**, the camera’s **S<corrected_servo>** message directly sets the servo angle to chase the closest visible cube; if no follow message arrives within 250-500 ms, it returns to **PID** as the cube might have been passed or lost from the view. When a proximity trigger (**RED** or **GREEN**, **R** or **G**) arrives, it switches to **AVOID_CUBE**, executes a 37° turn plus an 8 cm clear‐away while holding that heading, then enters **AFTER_CUBE** to reallign on gyro while moving back to center section of each side of the map, and to flush the leftover commands before reverting to **PID**.

**Final Round Flowchart:**
```txt
Detect Cube
     ↓
Determine Color
     ↓
Follow Cube
     ↓
Avoid Left/Right
     ↓
Recenter Robot
     ↓
Return To PID
```
---

### Arduino Side

#### States Driving Logic

This `switch` statement runs inside `void loop()` and decides what the robot does in each of the four states.

```cpp
switch (currentState) {

  case PID:
    {
      // PID straight‐drive, maintain heading using gyro PD
      double err = current_angle_gyro - gz;
      pid_error        = (err) * kp + (pid_error - pid_last_error) * kd;
      pid_last_error   = pid_error;
      steer(pid_error);
      move(robot_speed);
      break;
    }

  case FOLLOW_CUBE:
    {
      // If no follow command arrives within 250-500ms, go back to gyro PD
      if (millis() - last_follow_cube > FOLLOW_CUBE_LOST_TIME) {
        currentState = PID;
      }
      // Otherwise, steer directly toward the cube, so we do active cube following, using a PD that runs on the camera
      steer(follow_cube_angle);
      move(robot_speed);
      break;
    }

  case AVOID_CUBE:
    {
      // Perform the hard coded avoidance maneuver
      pass_cube(cube_avoid_direction);
      break;
    }

  case AFTER_CUBE:
    {
      // Return to PID after getting to be straight again, while ignoring all commands
      while (millis() - last_cube_time > 1500) {
        read_gyro_data();
        double error = current_angle_gyro - gz;
        pid_error = (error)*kp + (pid_error - pid_last_error) * kd;
        pid_last_error = pid_error;
        if (debug == true) {
          Serial.print(current_angle_gyro);
          Serial.print("     |      ");
          Serial.print(turn_direction);
          Serial.print("     |      ");
          Serial.print(gz);
          Serial.print("     |      ");
          Serial.println(robot_speed);
        }
        steer(pid_error);
        move(robot_speed);
      }
      currentState = PID;
      Serial0.flush();
      break;
    }
}
```

**PID:**
- Uses a PD loop on the integrated gyro yaw (`gz`) vs. desired heading (`current_angle_gyro`).
- Calls `steer(pid_error)` and `move(robot_speed)` to drive straight.

**FOLLOW_CUBE:**
- If we haven’t received a new `"S…\n"` follow message for at least 250-500 ms, assume the cube is lost or passed and switch back to **PID**.
- Otherwise, use `follow_cube_angle` (sent by the camera) to set the servo and follow the cube.

**AVOID_CUBE:**
- Calls `pass_cube(cube_avoid_direction)`, which executes a turn to left or right based on the cube color. This code sequence is hard coded.
  
**AFTER_CUBE:**
- Return to **PID** after getting to be straight again, while ignoring all commands.


 #### UART Command Parser (execute_command())
 
This function is called whenever a complete line arrives over **UART**. It selects which state to enter (**PID**, **FOLLOW_CUBE**, **AVOID_CUBE**) or adjusts gyro target angle on a **BLACK** message by adding or removing 90 degrees, based on the turn direction.

```cpp
void execute_command(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  // “S…” steering to follow cube ➜ FOLLOW_CUBE
  if (cmd.startsWith("S") && RUN_MODE == 1) {
    float val = cmd.substring(1).toFloat();
    follow_cube_angle = val;
    currentState      = FOLLOW_CUBE;
    last_follow_cube  = millis();

    if (debug) {
      Serial.print("FOLLOW_CUBE angle ➜ ");
      Serial.println(follow_cube_angle);
    }
    return;
  }

  // Map color words to one letter commands and set turn_direction on first BLUE/ORANGE
  char c = cmd.charAt(0);
  if      (cmd.indexOf("BLACK")  != -1) { c = 'B'; }
  else if (cmd.indexOf("BLUE")   != -1) { c = 'L'; lastLineDetectedTime = millis(); if (turn_direction == 0) turn_direction = -1; }
  else if (cmd.indexOf("ORANGE") != -1) { c = 'O'; lastLineDetectedTime = millis(); if (turn_direction == 0) turn_direction =  1; }
  else if (cmd.indexOf("RED")    != -1) { c = 'R'; }
  else if (cmd.indexOf("GREEN")  != -1) { c = 'G'; }
  else if (cmd.indexOf("PINK")   != -1) { c = 'P'; }
  else { return; }  // Ignore any other strings

  // “BLACK” turn 90° if in PID
  if (currentState == PID) {
    if (c == 'B' || (millis() - lastLineDetectedTime > 1800 && lastLineDetectedTime > 0)) {
      if (millis() - lastTurnTime < 1000) {
        if (debug) Serial.println("Ignoring repeated 'B' command due to cooldown.");
        return;
      }
      if (debug) Serial.println("Received 'BLACK' command. Turning 90°...");
      current_angle_gyro += turn_direction * 90;
      turn_count++;
      lastTurnTime         = millis();
      lastLineDetectedTime = 0;
      return;
    }
  }

  // “RED” or “GREEN” close to a cube and need to avoid it ➜ AVOID_CUBE
  if ((c == 'R' || c == 'G') && millis() - last_cube_time >= AVOIDANCE_DRIVE_TIME) {
    cube_avoid_direction = (c == 'R') ? 'L' : 'R';  // 'R' means turn left, 'G' turn right
    currentState         = AVOID_CUBE;
    last_cube_time       = millis();
    if (debug) {
      Serial.print("AVOID_CUBE dir ➜ ");
      Serial.println(cube_avoid_direction);
    }
    return;
  }

  if (debugcam) {
    Serial.print("Ignored cmd ➜ ");
    Serial.println(cmd);
  }
}
```

**“S…” (e.g. `"S+0.120\n"`):**
- Parses the float after `S`, sets `follow_cube_angle` to the value that was sent after S character, and switches to **FOLLOW_CUBE**.
- Records the time to `last_follow_cube` so we know how long it has been since the camera last issued a “follow” command.

**“BLACK”:**
- Only when in **PID**, add or substract 90° based on the lap direction, by adjusting `current_angle_gyro += ±90`.
- Increment `turn_count` and enforce a 1 s cooldown to avoid repeated turning signals.

**“RED” or “GREEN”:**
- When getting to close to a cube, it sets `cube_avoid_direction` based on the cube color and enter **AVOID_CUBE**.


#### Cube Avoidance Subroutine (pass_cube())
When in **AVOID_CUBE**, the robot executes a hardcoded movement, that includes a fixed turn + move forward maneuver to avoid the cube, then transitions to **AFTER_CUBE**, that alligns the robot for doing **PID** and follow the next cube.

```cpp
void pass_cube(char cube_direction) {
  read_gyro_data();
  // Convert 'R' ➜ +1 (turn left), 'G' ➜ -1 (turn right)
  int sign = (cube_direction == 'R') ? 1 : -1;

  double start_angle  = gz;
  double target_angle = start_angle - sign * AVOIDANCE_ANGLE;

  // Turn ~37° away from cube
  move_until_angle(robot_speed, target_angle);

  // Drive ~8 cm forward while holding that heading
  move_cm_gyro(8, robot_speed, target_angle);

  // Move to AFTER_CUBE case
  last_cube_time = millis();
  currentState   = AFTER_CUBE;

  // Discard any leftover UART messages before returning to PID
  flush_messages();
}
```

### Camera Side (OpenMV RT1062)
In the OpenMV python script, we detect the largest visible red or green blob in the bottom 40% section of the frame, while computing a PD steering error, and send either a small follow command "S±pid_error\n" or an avoid trigger "RED\n"/"GREEN\n" over UART.

**Choose Closest Cube & Follows it by using a PD algorithm**

```python
# ---- Choose Closest Cube (largest red or green) ----
candidates = []
if red_cube:   candidates.append(('R', red_cube))
if green_cube: candidates.append(('G', green_cube))

if candidates:
    # Pick the cube (red or green) with the largest area
    color_char, cube = max(candidates, key=lambda x: x[1].area())
    area = cube.area()
    col  = (255,0,0) if color_char == 'R' else (0,255,0)

    # Draw rectangle and crosshair on the selected cube
    img.draw_rectangle(cube.rect(), color=col)
    img.draw_cross(cube.cx(), cube.cy(), color=col)
    img.draw_string(cube.x(), cube.y() + cube.h() - 10,
                    str(area), color=col)

    # Compute normalized horizontal error in [-1, 1]
    error = (cube.cx() - target_x) / float(target_x)
    pid_error = kp_cube * error + kd_cube * (error - pid_last_error)
    pid_last_error = error

    if area < follow_threshold:
        # Send “S±error\n” to indicate a small servo correction (➜ FOLLOW_CUBE)
        uart.write("S{:+.3f}\n".format(error))
        if DEBUG:
            print("{} FOLLOW ➜ err:{:+.3f}, pid:{:+.3f}, area:{}".format(
                  "RED" if color_char == 'R' else "GREEN",
                  error, pid_error, area))
    else:
        # At close range, send just “RED\n” or “GREEN\n” to trigger avoidance
        uart.write(("RED\n" if color_char == 'R' else "GREEN\n"))
        if DEBUG:
            print("{} CLOSE ➜ area {} >= {}".format(
                  "RED" if color_char == 'R' else "GREEN",
                  area, follow_threshold))
```

<img src="" width="900">

<img src="" width="900">
*to add images

### 🅿️ Starting from Parking <a id="start-from-parking"></a>
At the start of the round, our robot is placed in the designated parking zone (the parking zone is 1.5x robot lenght). To determine the direction of the first lap (clockwise or counterclockwise), we use the 2 distance sensors placed on left and right side of our robot, the one that sees the wall further away will be the side to exit and do the laps.

Once the direction is determined, the robot performs an initial steering exit maneuver (to left or right) using our custom PD-based turning function:

```cpp
// Start from parking
if (RUN_MODE == 1) {
  if (readDistanceMM(LEFT_DIR, 3) < readDistanceMM(RIGHT_DIR, 3)) {  // exit right
    move_until_angle_max(exit_speed, 75);

    move(65);
    delay(100);

    turn_direction = 1;
    move_until_angle_max(exit_speed, 0);

    move_straight_on_gyro(-robot_speed, 900); // allign the robot to be able to pass cubes
  } else {  // exit left
    move_until_angle_max(exit_speed, -70);

    move(65);
    delay(100);

    turn_direction = -1;
    move_until_angle_max(exit_speed, 0);

    move_straight_on_gyro(-robot_speed, 1800); // allign the robot to be able to pass cubes
  }
}
```

The robot turns ~75° toward the main track depending on the detected direction. Following this parking exit, the robot enters its standard operating state:
- If it detects a cube, it enters **FOLLOW_CUBE** mode, where it tries to center itself on the cube using a PD algorithm.
- If the cube gets **too** close, it enters **AVOID_CUBE** mode, passing the cube left or right based on its color.
- After **AVOID_CUBE**, it automatically enters **AFTER_CUBE**, where it centers itself on the center of the track to prepare for searching the next cube.
- When no cube is detected, it enters **PID** mode, in which it simply navigates straight using the gyro—just as in qualification runs.


### 🅿️ Parking <a id="parking"></a>

At the end of the run, once the robot has completed **12 turns in RUN\_MODE 1** (final run mode), it transitions into the **parallel parking sequence**. This routine is fully deterministic and relies on both **front and back distance sensors** together with the **gyro angle** to achieve a repeatable and precise result. The sequence consists of four main phases:

**1. Front-Wall Alignment**

The robot first drives forward until it reaches a **target offset from the front wall**, ensuring a consistent starting position for the parking maneuver.

* **Left course:** 470 mm from the front wall
* **Right course:** 460 mm from the front wall

This distance lock guarantees that the robot always starts the parking procedure at the same longitudinal reference point.


**2. Lane Reorientation**

Next, the robot performs a **90° snap turn** into the parking lane:

* **Heading update:** `current_angle_gyro += turn_direction * 90`
* **Correction:** On left courses, an additional `-3°` trim is applied for drift compensation.

This ensures the chassis is now parallel to the parking lane wall.


**3. Lane Squaring & Staging**

The robot then stabilizes its position inside the lane using a two-step process:

* Moves forward **200 mm** while holding the new heading (gyro-straight).
* Aligns on the **rear wall** by driving backwards until the back sensor reads **300 mm**.

Finally, it advances to a **staging distance** inside the lane, preparing for the parking maneuver:

* **Left course:** 900 mm from the front wall
* **Right course:** 1590 mm from the front wall

This places the robot in the correct position to start the 3-move parallel park.


**4. Three-Maneuver Parallel Parking**

The actual parallel parking is executed as three steering sweeps:

* **Left Course (`turn_direction = -1`)**

  1. **Reverse Arc:** Turn at `-75°` relative to prking spot.
  2. **Straighten Backwards:** Reduce angle to `-5°`.
  3. **Forward Tuck:** Correct forward sweep to `+5°`.

* **Right Course (`turn_direction = +1`)**

  1. **Reverse Arc:** Turn at `+75°` relative to parking spot.
  2. **Straighten Backwards:** Reduce angle to `+5°`.
  3. **Forward Tuck:** Correct forward sweep to `-2°`.

This sequence positions the robot in the parking zone, parallel to the wall and within the designated area.


**5. Final Stop**

Once the three-step maneuver is complete, the robot:

* Centers the steering servo.
* Cuts motor power (`move(0)`).
* Enters a **20-second idle state**, ending the run.

Code (for both lap directions):

```cpp
...
if (turn_count == 12 && RUN_MODE == 1) {
  move_straight_on_gyro(robot_speed, 500);
  if (turn_direction == -1) {
    move_to_distance(FRONT_DIR, 470.0f, 10.0f, 55, current_angle_gyro);
    current_angle_gyro += turn_direction * 90;
    current_angle_gyro -= 3;

    move_until_angle_max(60, current_angle_gyro);
    move_straight_on_gyro(robot_speed, 200);
    move_to_distance(BACK_DIR, 300.0f, 10.0f, -55, current_angle_gyro);
    move_to_distance(FRONT_DIR, 900.0f, 10.0f, 55, current_angle_gyro);

    // sensu de stanga
    move_until_angle_max(-60, (current_angle_gyro) - 75);
    move_until_angle_max(-60, (current_angle_gyro) - 5);
    move_until_angle_max(60, (current_angle_gyro) + 5);
  } else {
    move_to_distance(FRONT_DIR, 460.0f, 10.0f, 55, current_angle_gyro);
    current_angle_gyro += turn_direction * 90;

    move_until_angle_max(60, current_angle_gyro);
    move_straight_on_gyro(robot_speed, 200);
    move_to_distance(BACK_DIR, 300.0f, 10.0f, -55, current_angle_gyro);
    move_to_distance(FRONT_DIR, 1590.0f, 10.0f, 55, current_angle_gyro);

    move_until_angle_max(-63, (current_angle_gyro)+75);
    move_until_angle_max(-63, (current_angle_gyro)+5);
    move_until_angle_max(60, (current_angle_gyro)-2);
  }

  steeringServo.write(STEERING_CENTER);
  delay(300);

  move(0);
  delay(20000);
}
...
```

Parking Sequence flowchart:
```txt
Detect Parking Area
      ↓
Align To Front Wall
      ↓
Reverse With Steering
      ↓
Align Parallel
      ↓
Stop Inside Parking
```


---

## 📽️ Performance Video <a id="performance-video"></a>

🔗 **[Click here to watch the video on YouTube]()** 

---

## 💰 Cost Analysis <a id="cost-analysis"></a>

We optimized the robot for a balance between **high performance, reliability, and cost efficiency**. The total cost includes electronics, motors, sensors, PCB manufacturing, 3D printed parts, and mechanical hardware used during both development and the final competition-ready version.

### 📦 **Components Costs** <a id="components-costs"></a>

**Note:** Many of the components used in this project were provided through sponsorships and partnerships. The prices listed above represent the standard market prices without sponsor discounts, in order to reflect the real estimated value of the robot.
> 
| Component | Quantity | Unit Price ($) | Total ($) |
|---------------------------------------------|----------|----------------|-------------|
| Arduino Nano ESP32 | 1 | 21.42 | 21.42 |
| Drive Motor 6V (30:1 HPCB) | 1 | 22.45 | 22.45 |
| IFX9201SG Motor Driver | 1 | 4.62 | 4.62 |
| Steering Servo (MG90S) | 1 | 4.05 | 4.05 |
| OpenMV RT1062 Camera | 1 | 80.00 | 140.00 |
| Gyroscope (BMI088) | 1 | 8.50 | 8.50 |
| Pololu Distance Sensor | 4 | 17.95 | 71.80 |
| LiPo Battery (2S 300mAh) | 1 | 5.60 | 5.60 |
| D24V50F5 Voltage Regulator | 1 | 29.95 | 29.95 |
| Custom Silicone Wheels | 4 | 9.24 | 36.96 |
| RC Differential | 1 | 19.00 | 19.00 |
| Experimental Parts | - | 35.00 | 35.00 |
| **TOTAL COMPONENT COST** | - | **-** | **363.35** |

### 🧪 Experimental Parts & Development Iterations

During development, multiple experimental parts and prototype versions were tested before reaching the final design. These included:
- Different wheel compounds and tire designs
- Alternative PCB revisions
- Various motor ratios and steering geometries
- Prototype chassis parts and sensor mounts

These parts were mainly used for testing and optimization and are not included in the final competition robot cost.

---

### **🔌 PCB Cost (Manufactured via JLCPCB)** <a id="pcb-cost"></a>
| PCB Component                  | Quantity | Unit Price ($) | Total ($)    |
|--------------------------------|----------|----------------|--------------|
| **PCB Manufacturing (JLCPCB)** | TBD      | **TBD**        | **TBD**      |
| **PCB Assembly (JLCPCB)**      | TBD      | **TBD**        | **TBD**      |
| **TOTAL PCB COST**             | TBD      | **TBD**        | **TBD**      |

---

### **🖨️ 3D Printing Cost Breakdown** <a id="3d-printing-cost"></a>

For 3D Printing we used Anycubic Kobra S1 Printer, and as well for some high quality resing printing we used the 3DP service from JLC.

| 3D Printed Parts                          | Quantity | Unit Price ($) | Total ($)   |
|-------------------------------------------|----------|----------------|---------------|
| **1000g Anycubic PLA+ filament**       | 1       | **16.99**      | **16.99**     |
| **1000g Anycubic ABS filament**       | 1       | **21.99**      | **21.99**     |
| **TOTAL 3D PRINTING COST**                |         |            | **38.98**     |

---

### **🔩 Other Materials (Screws, Nuts, and Miscellaneous)** <a id="other-materials-cost"></a>
| Material                       | Quantity | Unit Price ($) | Total ($)   |
|--------------------------------|----------|--------------|---------------|
| **M2/M3 Screws & Nuts**       | -      | **-**      | **10.00**       |
| **Wiring & Connectors**        | -      | **-**      | **10.00**       |
| **TOTAL OTHER MATERIALS COST** |        |            | **20.00**       |

---

### **💵 TOTAL** <a id="total-cost"></a>
| Category                      | Total Cost ($) |
|-------------------------------|----------------|
| **Components**                | **363.35**     |
| **PCB (JLCPCB + Components)** | **TBD**     |
| **3D Printing**               | **38.98**      |
| **Other Materials**           | **20.00**       |
| **TOTAL PROJECT COST**        | **TBD**     |

**Prices are approximate, based on current market prices.*

---

## 📂 Resources <a id="resources"></a>

Below is a list of **external images** used in this repository.

## 📂 Resources <a id="resources"></a>

Below is a compact list of **external images** used in this repository.

| Category | Resources |
|---|---|
| Social Badges | [Website](https://img.shields.io/badge/Website-000?style=for-the-badge&logo=google-chrome&logoColor=white), [YouTube](https://img.shields.io/badge/YouTube-FF0000?style=for-the-badge&logo=youtube&logoColor=white), [Instagram](https://img.shields.io/badge/Instagram-E4405F?style=for-the-badge&logo=instagram&logoColor=white), [X](https://img.shields.io/badge/X-000000?style=for-the-badge&logo=x&logoColor=white), [Facebook](https://img.shields.io/badge/Facebook-1877F2?style=for-the-badge&logo=facebook&logoColor=white), [GitHub](https://img.shields.io/badge/GitHub-181717?style=for-the-badge&logo=github&logoColor=white), [OSHWLab](https://img.shields.io/badge/OSHWLab-0A7DFF?style=for-the-badge&logo=opensourcehardware&logoColor=white) |
| Sponsor Logos | [OpenMV](https://hyperlinerobotics.com/assets/images/sponsors/openmv.avif), [EasyEDA](https://hyperlinerobotics.com/assets/images/sponsors/EasyEDA_logo.png), [OSHWLab](https://hyperlinerobotics.com/assets/images/sponsors/OSHWLab.png), [LCSC](https://static.lcsc.com/feassets/pc/images/headIcons/logo-s.png), [ING](https://hyperlinerobotics.com/assets/images/sponsors/ING.png), [Fundația Comunitară București](https://hyperlinerobotics.com/assets/images/sponsors/FundatiaComunitaraBucuresti.png), [SoBotz](https://hyperlinerobotics.com/assets/images/sponsors/SoBotz-white.png), [JSumo](https://jsumo.com/Data/EditorFiles/jsumo-robot-parts-logo.png), [Evergreen](https://hyperlinerobotics.com/assets/images/sponsors/evergreen.png), [FAE Drones](https://fae-drones.com/wp-content/uploads/2022/08/logo-vertical-ro.png) |
| Robot Parts | [Pololu Gearmotor](https://a.pololu-files.com/picture/0J12418.220.jpg?8f026fe1675b1109ea574290d3d26081), [IFX9201SG Driver](https://assets.lcsc.com/images/lcsc/900x900/20230316_Infineon-Technologies-IFX9201SG_C112633_front.jpg), [MG90S Servo](https://static.optimusdigital.ro/20565-large_default/mg90s-servomotor.jpg), [Li-Po Battery](https://img-va.myshopline.com/image/store/2000408386/1640672930478/GNB3002S60AHV-(4)_1800x.jpeg?w=1000&h=1000), [Arduino Nano ESP32](https://store.arduino.cc/cdn/shop/files/ABX00092_01.iso_804x603.jpg?v=1727101612), [OpenMV RT1062](https://openmv.io/cdn/shop/files/cam-v5-front-hero-2-web_1000x.jpg?v=1699323324), [Pololu Distance Sensor](https://a.pololu-files.com/picture/0J11135.1200.jpg?1910ced553e34153046a4c95021a93b3), [Voltage Regulator](https://gomagcdn.ro/domains/robofun.ro/files/product/large/pololu-5v-5a-step-down-voltage-regulator-d24v50f5-633511-832-442353.jpg) |

**Note:** All external images are credited to their original owners and are used only for documentation and component reference purposes.

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


