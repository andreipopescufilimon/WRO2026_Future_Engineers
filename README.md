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
    <img src="https://hyperlinerobotics.com/assets/images/sponsors/SoBotz.png" height="55">
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
**Age:** 17 <br>

**High School:** International Computer High School Bucharest (ICHB)  

**Description:**  
Hi! I’m Andrei from Romania, and this is my sixth WRO season. This is my second season in Future Engineers, as before I competed in the Robomission category. I am passionate about robotics, especially electronics and the latest algorithms and tech. Over the years, I have worked on multiple robotics projects, including line followers, sumo bots, and air quality modules. I also founded HyperLine Robotics, where I build and develop advanced robotics projects with my team. Apart from robotics, I also enjoy cybersecurity, programming, and cycling.

---

### **Horia Simion**
**Age:** 16 <br>
**High School:** National College "Mihai Viteazul" (CNMV)

**Description:**  
Hi! I’m Horia from Romania, and this is my second WRO season competing alongside Andrei. I have participated in RoboMission multiple times, gaining valuable experience in solving various problems that may arise while building a robot. I have a strong interest in technology and robotics and am always eager to learn and experiment with new ideas.

---

### **Eduard Alexandru Vasile**
**Age:** 15 <br>
**High School:** ...

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


---

## 🚗 Mobility Management <a id="mobility-management"></a>

The robot's mobility is controlled through **a fully PCB chassis, a servo-based steering system, and a drivetrain featuring a custom differential and axes**. These components work together to ensure smooth, **precise movement with optimized traction, stability, and efficient power management**.

## ⚙️ **Drivebase** <a id="drivebase"></a>

### 🔧 **Drivetrain** <a id="drivetrain"></a>

The drivetrain uses a custom micro-RC differential at the rear, driven by a **Pololu 30:1 HPCB micro gearmotor** through a **3D printed gear** → **differential input gear**. The motor is held in a 3D-printed support with the battery mounted above, keeping the center of mass centered and low. Rear outputs rotate in bearings seated inside a printed part that contains 4 bearings for the wheel axles, this attachment is mounted using M2 screws to the PCB chassis, minimizing friction and parts count. 
At the front, the wheels are fully 3D-printed and each wheel runs on two bearings (inner + outer) for a rigid, wobble-free hub that steers precisely.

<img src="" width="600">
**to add images*

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

**to add images*

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

**to be completed*


---


## 🔄 **Steering** <a id="steering"></a>

The **steering system** is a critical part of the robot, ensuring precise maneuverability and smooth turns. Our design is based on a **parallelogram steering mechanism**, where both front wheels turn at the same angle through a single servo-controlled linkage. This setup provides predictable and stable steering, making it easy to use for an autonomous vehicle. Instead of using an **Ackermann steering system**, which requires more complex calculations and linkages, we opted for a **simpler and more lightweight solution** that offers consistent control. Our steering system allows for a maximum turning angle of 80 degrees in both the left and right directions. This range provides precise maneuverability, enabling the robot to navigate sharp turns efficiently while maintaining stability.

**add steering gif*

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

| <img src="https://image.easyeda.com/oshwhub/pullImage/1db8f1f53c984bd58fc7691a401918ca.png" width="300"> | **Specifications** |
|------------------------------|------------------------------|
| **Gyroscope Range:** ±2000°/s | **Accelerometer Range:** ±24g |
| **Interface:** I2C / SPI | **Supply Voltage:** 3.0V – 3.6V |
| **Current Draw:** ~3.2mA | **Weight:** ~1g |
| 🔗 [View Open Source Project Here](https://oshwlab.com/bicicleta11/bmi088-gyro-module) | **Function:** Tracks orientation & motion |

---

### **📷 OpenMV RT1062 Camera – Vision Processing** <a id="openmv-h7-camera">
**to be completed*


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
**to be completed*


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


