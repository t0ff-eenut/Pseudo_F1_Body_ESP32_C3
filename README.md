# Pseudo F1 RC Car Body Control Unit (BCU) 펌웨어

## 1. 프로젝트 개요
이 프로젝트는 **ESP32-C3 Super Mini**를 기반으로 한 1/10 스케일 F1 RC카의 바디 제어 유닛(BCU) 펌웨어입니다. 라즈베리파이(Raspberry Pi)와 UART로 통신하며, 수동 조종 및 자율 주행 명령을 수행합니다.

### 주요 기능
*   **DC 모터 제어**: 전륜(Front) 및 후륜(Rear) 모터 제어 (각 모터당 2핀 PWM 제어 방식).
*   **서보 모터 제어**: PWM을 이용한 조향 제어.
*   **UART 통신**: 패킷 기반의 명령어 처리 (기본 115200bps).
*   **상태 표시**: LED 스트립(WS2812) 지원.

---

## 2. 하드웨어 구성 (핀맵)

### ESP32-C3 Super Mini 핀맵 (업데이트됨)
| 기능 | GPIO | 설명 |
| :--- | :--- | :--- |
| **전륜 모터 (Front)** | **GPIO 6** | IN1 (PWM/방향) |
| | **GPIO 10** | IN2 (PWM/방향) |
| **후륜 모터 (Rear)** | **GPIO 4** | IN1 (PWM/방향) |
| | **GPIO 5** | IN2 (PWM/방향) |
| **조향 서보** | **GPIO 7** | 서보 PWM |
| **UART** | **GPIO 20** | TXD |
| | **GPIO 21** | RXD |
| **LED 스트립** | **GPIO 8** | WS2812 |
| **BOOT** | **GPIO 9** | 리셋/설정 |

> **참고**: 부팅 문제 방지를 위해 스트래핑 핀인 GPIO 2 사용을 피했습니다.

---

## 3. 통신 프로토콜
패킷 구조 (6 바이트):
`[STX(0xAA)] [CMD] [PARAM1] [PARAM2] [CHECKSUM] [ETX(0x55)]`

*   **CMD 0x01 (제어 명령)**:
    *   Param1: 스로틀/속도 (-100 ~ 100)
    *   Param2: 조향 각도 (-100 ~ 100 또는 각도)

---

## 4. 빌드 방법
ESP-IDF 환경이 필요합니다.
```bash
idf.py build
idf.py flash monitor
```
