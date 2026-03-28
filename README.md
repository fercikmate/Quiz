# QUIZZER: Real-Time Buzzing System
A high-performance buzzing system utilizing an ESP32 microcontroller running FreeRTOS and a Python Tkinter GUI. This project demonstrates real-time event handling, multi-core processing, and bidirectional serial communication.

## Features
* Real-Time Button Polling: ESP32 uses a dedicated FreeRTOS task pinned to Core 1 for microsecond-accurate edge detection.

* Producer-Consumer Architecture: Decoupled firmware logic using Task Notifications for non-blocking serial transmission.

* Dynamic GUI: Windows-based dashboard with customizable team names, colors, and millisecond-accurate timing.

* Hardware Lockout: Prevents multiple button registrations until the system is reset by the host.

* Bidirectional Communication: Reset hardware states via Serial commands sent from the Python UI.

# Technical Specifications
* Microcontroller: ESP32 (ESP-IDF / C)

* Operating System: FreeRTOS

* GUI Framework: Python 3.13 + Tkinter

* Communication: UART / Serial (115200 Baud)

* Build Tool: PlatformIO & PyInstaller

# Firmware Architecture (FreeRTOS)
The ESP32 firmware is designed as a Multicore Real-Time System. By utilizing an RTOS (Real-Time Operating System), the project separates physical hardware detection (core 1) from serial communication (core 0), ensuring high responsiveness and system stability.

## Task Distribution
The workload is distributed across the ESP32's dual-core processor to prevent I/O blocking and task starvation:

### Listener Task (Core 1 / Priority 1):
Dedicated to high-speed polling of GPIO pins for Teams 1-5. Pinning this to Core 1 ensures that background system activities on Core 0 do not interfere with buzzer timing.

### Sender Task (Core 0 / Priority 1):
Handles UART/Serial transmission. It remains in a "Blocked" state until notified by the Listener, saving CPU cycles.

### Receiver Task (Core 0 / Priority 1):
Asynchronously listens for 'Reset' commands sent from the Python UI to clear the hardware lockout state.

# Hardware Configuration
Connect buttons to the following ESP32 GPIO pins (Pull-Down mode):

Team 1: GPIO 27

Team 2: GPIO 26

Team 3: GPIO 25

Team 4: GPIO 33

Team 5: GPIO 32

Status LED: GPIO 4


# Usage Instructions
* Connect the ESP32 to the PC via USB.

* Launch the QUIZZER application.

* In the Setup window, enter the correct COM Port (e.g., COM4) and configure team settings.

* Click "Start Quiz" to open the main dashboard.

* When a team buzzes, their timestamp and order will be displayed.

* Use the "Reset" button to clear the board and prepare the ESP32 for the next question.
