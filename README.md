# ESP32-CAM Automated Positioning System

**Application of image processing algorithms for real-time device positioning**

This project implements a firmware for the **ESP32-CAM** microcontroller, enabling real-time image processing and automated device positioning using **FreeRTOS** and **ESP-IDF**. The system leverages computer vision algorithms to detect and track objects suspicious objects: Sun drones and controls servos for precise camera alignment. The project is developed using **PlatformIO** in **VS Code**.

---

## Features

- **Wi-Fi Connectivity**: Connects to Wi-Fi with a prioritized list of access points and supports mDNS for easy access via `http://esp32.local`.
- **Real-Time Image Processing**:
  - **Brightness Analysis**: Detects the brightest points (e.g., the Sun), calculates their center of mass, and determines angles for camera alignment.
  - **FAST-9 Algorithm**: Identifies key points (contour corners) for object detection, optimized for low-resource environments.
  - **DBSCAN Clustering**: Groups key points to detect objects like drones, filtering noise and identifying cluster centers.
- **Web Interface**: Hosts a web server to display images, computed data (e.g., key point coordinates), and supports interactive zooming via rectangular selection.
- **Servo Control**: Drives **SG-90** servos for precise camera positioning based on computed angles, using PWM signals.
- **FreeRTOS Multitasking**: Manages concurrent tasks for image capture, processing, servo control, and web server operations.

---

## How It Works

### Edge Detection Mode (FAST-9 + DBSCAN)
Uses the **FAST-9** algorithm to detect key points and **DBSCAN** for clustering them into meaningful groups (e.g., drones). The highest cluster center is used for camera positioning.

![FAST-9 Demo](https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/fast9_early.gif)

*localy on laptop*

<img src="https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/fast9_from_air.png" alt="demo/fast9_from_air" width="700">

## pure FAST9 by esp32-cam
<img src="https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/esp32cam/fast9_desk.png" alt="demo/esp32cam/fast9_desk" width="350" height="350"> <img src="https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/esp32cam/fast9_empty_window.png" alt="demo/esp32cam/fast9_empty_window" width="350" height="350">

## DBSCAN
<img src="https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/esp32cam/fast9_dbscan_highest.png" alt="demo/esp32cam/fast9_dbscan_highest" width="600" height="600">

## Brightness Analysis Mode
Detects the brightest pixels in an image, filters them, and calculates the center of mass. The system then computes horizontal and vertical angles to align the camera with the target (e.g., the Sun).

<!-- ![Brightness Analysis](https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/look4sun.png) -->

<img src="https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/esp32cam/light_in_window.png" alt="demo/esp32cam/light_in_window" width="350" height="350"> <img src="https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/esp32cam/window_glowing_inside.png" alt="demo/esp32cam/window_glowing_inside" width="350" height="350">

<img src="https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/esp32cam/light_inside.png" alt="demo/esp32cam/window_glowing_inside" width="500" height="500">

---

## Project Structure

The project is organized into modular components for maintainability and scalability:

- **frontend/**: Web interface files (`index.html`, `index_src.html`) for displaying images and data.
- **include/**: Header files with function declarations and macros.
  - `img_processing/`: Camera, DBSCAN, FAST-9, and brightness analysis functions.
  - `server/`: Web server, Wi-Fi, and mDNS configurations.
  - `my_servos.h`, `my_vector.h`: Servo control and dynamic array utilities.
- **src/**: Source files implementing core functionality.
  - `img_processing/`: Camera initialization, image processing, and algorithm implementations.
  - `server/`: Web server, Wi-Fi, and mDNS logic.
  - `servos.c`, `my_vector.c`: Servo management and dynamic array operations.
  - `main.c`: Program entry point, initializing tasks and modules.
- **Configuration Files**:
  - `CMakeLists.txt`: Build configuration.
  - `Kconfig.projbuild`: Project settings (e.g., Wi-Fi credentials).
  - `sdkconfig.esp32cam`: ESP-IDF hardware settings.

---

## Technical Details

### Hardware
- **ESP32-CAM**: 32-bit dual-core processor (240 MHz, 600 DMIPS), 520 KB SRAM, 4 MB PSRAM, 2 MP OV2640 camera, Wi-Fi/Bluetooth, and microSD support.
- **SG-90 Servos**: Two servos for pan and tilt, controlled via PWM (GPIO 14, 15).
- **Power**: 5V supply for easy integration.

### Software
- **FreeRTOS**: Real-time operating system for multitasking, managing image capture, processing, servo control, and web server tasks.
- **ESP-IDF**: Framework for hardware initialization, camera control, and communication.
- **Image Processing**:
  - **Brightness Analysis**: Optimized for GRAYSCALE, limits pixel count to 16 for efficiency.
  - **FAST-9**: Adapted from OpenCV, uses a decision tree for fast corner detection.
  - **DBSCAN**: Clusters key points with ε=20 pixels and min_points=3, optimized for low memory usage.
- **Memory Management**: Custom `vector_t` structure for dynamic arrays, minimal use of PSRAM (1.45%), and high utilization of DMA-compatible DRAM (82.81%).

### Challenges Overcome
- **Limited Resources**: Optimized algorithms for 520 KB SRAM, using GRAYSCALE and minimal PSRAM.
- **Multithreading**: Resolved deadlocks in FreeRTOS by careful mutex management and task synchronization.
- **Algorithm Stability**: Dynamic parameter tuning for FAST-9 and DBSCAN to handle varying lighting conditions.
- **Debugging**: Used `ESP_LOGI`, `ESP_LOGE`, and custom `log_memory` for tracking memory usage and errors.
- **Low FPS**: Implemented `ANGLE_DIVIDER` for smoother servo movements despite low frame rates.

---

## Device Appearance

![Device](https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/device/look.jpg)

## pinout

![UART pinout](https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/device/UART_pinout.jpg)
![servo pinout](https://github.com/vidkryvashka/esp32-cam-positioning/blob/main/demo/device/servo_pinout.jpg)

---