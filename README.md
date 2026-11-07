
# WRO2026 Future Engineers – HyperLine Robotics Team
---


Repository of HyperLine Robotics Team competing in the **World Robot Olympiad (WRO) 2026**, Future Engineers category.

### Links

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

---

## Table of Contents

- [ The Team](#the-team)
- [ Challenge Overview](#challenge-overview)
- [ Our Robot](#our-robot)
- [ Performance Metrics](#performance-metrics)
- [ Engineering Trade-Offs](#engineering-tradeoffs)
- [ Testing & Iterations](#testing-iterations)
- [ Mobility Management](#mobility-management)
- [ Power and Sense Management](#power-and-sense-management)
- [ Components coding](#components-coding)
- [ Obstacle Management](#obstacle-management)
- [ Edge Cases and Failure Handling](#edge-cases)
- [ Performance Video](#performance-video)
- [ Cost Analysis](#cost-analysis)
- [ Randomizer](#randomizer)
- [ License](#license)

## Folder Structure


This repository is organized as follows:


```
 WRO2026_Future_Engineers
├──  3D-models             # Contains 3D design files for the robot's components
│   ├──  old-3D-models     # Previous versions of 3D models
├──  electrical-schematics # Circuit diagrams and wiring
├──  github-commits        # Commit logs and change tracking details for this repository
├──  media                 # Images and videos
│   ├──  robot-photos      # Photos of the robot
│   ├──  team-photos       # Pictures of team members and teamwork
│   └──  video             # Recorded testing
├──  other                 # Contains files that do not fit into other categories
├──  src                   # Main source code for the robot
├──  technical-draws       # Technical drawings and mechanical blueprints
├──  video                 # Videos of our robot
├──  LICENSE               # MIT License for the project
└──  README.md             # Main documentation for the project
```

---

## The Team <a id="the-team"></a>

<img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/42d6e5a1cd52bcbd6b493d360703382f0b427a0a/media/team-photos/team.jpeg" width="450">

### **Popescu Filimon Andrei Cosmin** (middle)
**Age:** 18 <br>
**High School:** International Computer High School Bucharest (ICHB)
**View my portofolio**: [https://andreipopescufilimon.com/](https://andreipopescufilimon.com/)

**Description:**
Hi! I’m Andrei from Romania, and this is my sixth WRO season. This is my second season in Future Engineers, as before I competed in the Robomission category. I am passionate about robotics, especially electronics and the latest algorithms and tech. Over the years, I have worked on multiple robotics projects, including line followers, sumo bots, and air quality modules. I am the founder of HyperLine Robotics Team, where I build and develop advanced robotics projects with my team. Apart from robotics, I also enjoy cybersecurity, programming, and cycling.

---

### **Horia Simion** (left)
**Age:** 17 <br>
**High School:** National College "Mihai Viteazul" (CNMV)

**Description:**
Hi! I’m Horia from Romania, and this is my third WRO season competing alongside Andrei. I have participated in RoboMission multiple times, gaining valuable experience in solving various problems that may arise while building a robot. I have a strong interest in technology and robotics and am always eager to learn and experiment with new ideas!

---

### **Eduard Alexandru Vasile** (right)
**Age:** 16 <br>
**High School:** Liceul Teoretic Alexandru Ioan Cuza

**Description:**
Hi! I’m Eduard from Romania. I’m passionate about robotics, especially building and testing competitive robots. I have worked on sumo robots, and recently I started developing line follower robots as well. I also enjoy driving and flight simulators, where I like exploring realistic control, precision, and performance.

Apart from robotics, I love MTB, video games, and cybersecurity. I’m always interested in learning new technologies, improving my skills, and working on projects that challenge me both technically and creatively.


---

## Challenge Overview <a id="challenge-overview"></a>

The **WRO 2026 Future Engineers** challenge pushes teams to develop a **fully autonomous vehicle** capable of navigating a **dynamic and randomized racetrack** using **sensors, computer vision, and advanced control algorithms**. The goal is to complete **multiple laps** while adapting to randomized obstacles, following **strict driving rules**, and successfully executing a **parallel parking maneuver** at the end of the course.

### Competition Format

- **Open Challenge**: The vehicle must complete **three (3) laps** on a track with **randomly placed inside walls**.

- **Obstacle Challenge**: The vehicle must complete **three (3) laps** while detecting and responding to **randomly placed red and green traffic signs**:
  -  **Red markers** ➜ The vehicle must stay on the **right side of the lane**.
  -  **Green markers** ➜ The vehicle must stay on the **left side of the lane**.

  After completing the three laps, the vehicle must **locate the designated parking zone** and perform a **precise parallel parking maneuver** within a limited space, adding an extra layer of difficulty.

- **Documentation**: Each team must maintain a **public GitHub repository** showcasing their **engineering process, vehicle design, and source code**.

### Scoring & Evaluation
Scoring is based on **accuracy, technical documentation and speed**, rewarding teams that balance **efficiency, adaptability, and innovation**. This challenge not only tests **robotics and programming skills** but also promotes **problem-solving, teamwork, and engineering creativity**.

 **Find out more about the challenge [here](https://wro-association.org/wp-content/uploads/WRO-2026-Future-Engineers-Self-Driving-Cars-General-Rules.pdf).**

---

## Our Robot <a id="our-robot"></a>
| <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/5f4072d596f9771e1168e6d9f6c43632f88357e8/media/robot-photos/top.png" width="300">         | <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/5f4072d596f9771e1168e6d9f6c43632f88357e8/media/robot-photos/bottom.png" width="300">            |
|----------------------------------|-------------------------------------|
| <p align="center"><b>Top</b></p> | <p align="center"><b>Bottom</b></p> |

| <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/5f4072d596f9771e1168e6d9f6c43632f88357e8/media/robot-photos/left.png" width="300">          | <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/5f4072d596f9771e1168e6d9f6c43632f88357e8/media/robot-photos/right.png" width="300">           |
|-----------------------------------|------------------------------------|
| <p align="center"><b>Left</b></p> | <p align="center"><b>Right</b></p> |

| <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/5f4072d596f9771e1168e6d9f6c43632f88357e8/media/robot-photos/front.png" width="300">           | <img src="https://github.com/andreipopescufilimon/WRO2026_Future_Engineers/blob/5f4072d596f9771e1168e6d9f6c43632f88357e8/media/robot-photos/back.png" width="300">          |
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

## Performance Metrics <a id="performance-metrics"></a>

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

## Engineering Trade-Offs <a id="engineering-tradeoffs"></a>

Throughout the development process, every major design choice had to balance speed, stability, reliability, weight, and ease of debugging. The goal was not only to build a fast robot, but to create a compact autonomous vehicle that can perform consistently across different WRO field layouts, lighting conditions, obstacle positions, and parking scenarios.

| Decision | Chosen Solution | Reason |
|---|---|---|
| Main controller | Arduino Nano ESP32 | Compact board with enough processing power for real-time control, integrated wireless features for debugging, and good compatibility with our sensors and actuators |
| Vision system | OpenMV RT1062 | Lower latency, lower power consumption, and easier real-time color processing compared to a Raspberry Pi-based system |
| Chassis structure | PCB chassis | Reduces wiring, improves rigidity, saves space, and makes the robot easier to assemble and reproduce |
| Steering system | Parallel steering | Simpler, lighter, and more compact than Ackermann steering, while still giving predictable steering response |
| Navigation | Gyro-based PD control | More stable heading correction than relying only on encoder movement, especially during long straights and after obstacle avoidance |
| Tires | Cast silicone tires | Provide high grip and predictable handling on smooth WRO surfaces |
| Obstacle detection | Camera-based detection | Allows flexible detection of randomized red and green traffic signs, track lines, and parking elements |

The final robot design focuses on reliable autonomous behavior in a compact and lightweight structure. Instead of optimizing only one area, such as maximum speed, we designed the mechanical, electrical, and software systems to support each other. The chassis keeps the robot rigid, the tires and impeller improve grip, the camera provides high-level field information, and the gyro helps maintain stable heading control.
