# Gesture-Controlled Robot Arm

## Project Description
A robotic arm controlled entirely by hand gestures using Raspberry Pi and computer vision.

## Table of Contents
- [Problem Statement](#problem-statement)
- [Components](#components)
- [Challenges Faced](#challenges-faced)
- [How It Works](#how-it-works)
- [Flow Diagram](#flow-diagram)

---

### Problem Statement
Traditional robotic arms are usually controlled with buttons, joysticks, keyboards, or pre-programmed commands. While effective, these methods can feel unnatural, slow, and difficult for beginners.  

This project proposes a gesture-controlled robotic arm using a Raspberry Pi 5 and a camera or gesture sensor. Real-time image processing recognizes hand gestures and translates them into precise arm movements such as rotation, lifting, gripping, and pick-and-place tasks.  

The system demonstrates natural human-machine interaction through embedded control and computer vision, with applications in industrial automation, medical assistance, and smart robotics.

---

### Components

**Mechanical Components**
- Base platform  
- Rotating turntable/base joint  
- Lower arm link  
- Upper arm link  
- Gripper/claw mechanism  
- Acrylic structural plates  
- Bearings and joint brackets  
- Suction cup feet (×4)  

**Electronic Components**
- Arduino-compatible control board  
- OLED display module (0.96")  
- Servo motors (×6 AD002 servos)  
- Potentiometer control knobs  
- Power distribution circuitry  
- Battery holder (2×18650 batteries)  
- Micro USB interface  

**Wiring & Accessories**
- Servo extension cables  
- Jumper wires/connectors  
- Screws, nuts, copper standoffs, nylon spacers  
- Wire sleeve/winding pipe  

**Tools Included**
- Cross socket wrench  
- 2 mm screwdriver  
- 3 mm screwdriver  

**Functional Parts**
- Gripper holding a blue ball/object  
- Multi-axis servo joints: base rotation, shoulder, elbow, wrist, gripper open/close  

**Power/Programming**
- Compatible with Arduino IDE  
- USB programmable  
- Supports manual and PC control modes  

---

### Challenges Faced
- **Lack of proper guide** – Documentation was scattered, requiring trial and error.  
- **Raspberry Pi setup** – Flashing the OS and installing drivers was complex and error-prone.  
- **Python & Arduino integration** – Synchronizing gesture recognition (Python) with servo control (Arduino) was difficult.  
- **Gesture detection accuracy** – Fine-tuning image processing algorithms was necessary to avoid jerky or incorrect outputs.  

---

### How It Works
1. **Hand Movement** – A webcam captures the gesture.  
2. **Processing on Raspberry Pi** – The Pi receives the image and prepares it for analysis.  
3. **Display Input** – The detected gesture is shown on a monitor for confirmation.  
4. **Python Analysis** – Python scripts translate gestures into commands (e.g., fist = close, palm = open).  
5. **Command Transmission** – Commands are sent to the robotic arm’s servos.  
6. **Output Action** – The robotic arm performs the movement in real time, mirroring the user’s gesture.  

---

### Flow Diagram
![Alt Text](https://github.com/Josiah-Wami/Gesture-Control-Robot-Arm./blob/4e5cb1f38fa6c4cac9b94171a5f4a3670720d487/visily-gesture-controlled-robot-arm-workflow.png)
