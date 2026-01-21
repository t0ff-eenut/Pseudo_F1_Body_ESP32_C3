# Pseudo F1 RC Car Body Control Unit (BCU) Firmware

## 1. Project Overview
This project provides firmware for a 1/10 scale RC car Body Control Unit (BCU) based on **ESP32-C3 Super Mini**. It communicates with a Raspberry Pi via UART to execute manual control and autonomous driving commands.

### Key Features
*   **DC Motor Control**: 2-pin control for Front and Rear motors (PWM on IN pins).
*   **Servo Motor Control**: Steering control via PWM.
*   **UART Communication**: Protocol-based command processing (default 115200bps).
*   **Status Indication**: LED Strip support.

---

## 2. Hardware Configuration (Pin Map)

### ESP32-C3 Super Mini Pinout (Updated)
| Function | GPIO | Description |
| :--- | :--- | :--- |
| **Motor Front** | **GPIO 6** | IN1 (PWM/Dir) |
| | **GPIO 10** | IN2 (PWM/Dir) |
| **Motor Rear** | **GPIO 4** | IN1 (PWM/Dir) |
| | **GPIO 5** | IN2 (PWM/Dir) |
| **Steering** | **GPIO 7** | Servo PWM |
| **UART** | **GPIO 20** | TXD |
| | **GPIO 21** | RXD |
| **LED Strip** | **GPIO 8** | WS2812 |
| **BOOT** | **GPIO 9** | Reset/Config |

> **Note**: GPIO 2 (Strapping Pin) is avoided to prevent boot issues.

---

## 3. Communication Protocol
Packet Structure (6 Bytes):
`[STX(0xAA)] [CMD] [PARAM1] [PARAM2] [CHECKSUM] [ETX(0x55)]`

*   **CMD 0x01 (Control)**:
    *   Param1: Throttle (-100 to 100)
    *   Param2: Steering (-100 to 100 or Angle)

---

## 4. Build Instructions
Requires ESP-IDF.
```bash
idf.py build
idf.py flash monitor
```
