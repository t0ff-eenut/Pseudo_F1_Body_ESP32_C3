# Pseudo F1 RC Car Body Control Unit (BCU) Firmware

## 1. 프로젝트 개요
이 프로젝트는 **ESP32-C3 Super Mini**를 기반으로 한 1/10 스케일 F1 RC카의 바디 제어 유닛(BCU) 펌웨어입니다. 라즈베리파이(Raspberry Pi)와 UART로 통신하며, 수동 조종 및 자율 주행 명령을 수행합니다.

### 주요 기능
*   **DC 모터 제어**: L298N 드라이버를 이용한 후륜 구동 (속도/방향 제어, 브레이크, 코스트)
*   **서보 모터 제어**: 조향 장치 제어 (각도 제한 -45° ~ +45°)
*   **UART 통신**: 라즈베리파이와 115200bps 통신, 패킷 기반 명령 처리
*   **모드 관리**: 대기(IDLE), 수동(MANUAL), 자율(AUTO) 모드 전환 및 상태 관리
*   **상태 표시**: LED Strip을 이용한 현재 상태 및 모드 시각화

---

## 2. 시스템 아키텍처
이 펌웨어는 유지보수와 확장성을 위해 **3계층(Layered) 아키텍처**를 따릅니다.

| 계층 (Layer) | 폴더명 | 역할 | 주요 모듈 |
| :--- | :--- | :--- | :--- |
| **APP Level** | `app_level` | 애플리케이션 로직, 프로토콜 처리, 모드 관리 | `RCcar_control`, `RCcar_mode`, `RCcar_protocol` |
| **HAL Level** | `hal_level` | 하드웨어 추상화, 기능 단위 제어 | `custom_esp_motor`, `custom_esp_servo`, `custom_esp_uart` |
| **HW Level** | `hw_level` | MCU 페리페럴 직접 제어 (GPIO, PWM, NVS) | `custom_esp_gpio`, `custom_esp_nvs` |

---

## 3. 하드웨어 구성 및 핀맵 (Pin Map)

### 3.1. ESP32-C3 Super Mini Pinout
| 기능 | 핀 번호 (GPIO) | 설명 |
| :--- | :--- | :--- |
| **Motor A (Left)** | **GPIO 2** | ENA (PWM 속도) |
| | **GPIO 3** | IN1 (방향 1) |
| | **GPIO 4** | IN2 (방향 2) |
| **Motor B (Right)** | **GPIO 10** | ENB (PWM 속도) |
| | **GPIO 5** | IN3 (방향 1) |
| | **GPIO 6** | IN4 (방향 2) |
| **Servo Motor** | **GPIO 7** | PWM (조향 제어) |
| **UART (R-Pi)** | **GPIO 20** | TXD (ESP32 -> RPi) |
| | **GPIO 21** | RXD (RPi -> ESP32) |
| **LED Strip** | **GPIO 8** | WS2812 상태 표시 LED |
| **NVS Reset** | **GPIO 9** | BOOT 버튼 (설정 초기화) |

### 3.2. L298N 모터 드라이버 설정
*   **전원**: 7.4V ~ 12V (LiPo 2S/3S 권장)
*   **점퍼 설정**: ENA, ENB 점퍼 캡을 **제거**하고 PWM 핀에 연결해야 속도 제어가 가능합니다.

---

## 4. 통신 프로토콜 (Communication Protocol)
라즈베리파이와 ESP32 간의 통신은 **UART (115200bps, 8N1)**를 사용하며, 고정 길이 패킷을 주고받습니다.

### 4.1. 패킷 구조 (5 Bytes)
| Byte 0 | Byte 1 | Byte 2 | Byte 3 | Byte 4 |
| :---: | :---: | :---: | :---: | :---: |
| **HEADER** | **CMD** | **THROTTLE** | **STEERING** | **CHECKSUM** |
| `0xAA` | 명령어 | -100 ~ +100 | -45 ~ +45 | XOR Checksum |

*   **HEADER**: 패킷 시작을 알리는 고정 값 (`0xAA`)
*   **CMD**: 명령 종류 (아래 참조)
*   **THROTTLE**: 모터 속도 (양수: 전진, 음수: 후진, 0: 정지)
*   **STEERING**: 조향 각도 (양수: 우회전, 음수: 좌회전, 0: 중앙)
*   **CHECKSUM**: `HEADER` ^ `CMD` ^ `THROTTLE` ^ `STEERING` (XOR 연산 결과)

### 4.2. 명령어 (Command List)
| CMD 값 | 매크로 이름 | 설명 | 동작 |
| :---: | :--- | :--- | :--- |
| `0x01` | `CMD_MANUAL_CONTROL` | 수동 제어 | 수동 모드로 전환 및 모터/서보 제어 |
| `0x02` | `CMD_AUTO_CONTROL` | 자율 주행 | 자율 주행 모드로 전환 및 모터/서보 제어 |
| `0x03` | `CMD_STOP` | 비상 정지 | 모드 상관없이 즉시 정지 (브레이크) |
| `0x10` | `CMD_STATUS_REQ` | 상태 요청 | 현재 상태(모터, 서보, 모드) 응답 요청 |

---

## 5. 소프트웨어 모듈 상세

### 5.1. HAL Level
*   **`custom_esp_motor`**: L298N 드라이버를 제어합니다. `ledc` PWM을 사용하여 속도를 조절하고, GPIO로 방향을 설정합니다.
    *   `custom_motor_set_both(left, right)`: 양쪽 모터 속도 설정
    *   `custom_motor_brake()`: 역기전력을 이용한 능동 제동
    *   `custom_motor_coast()`: 전원 차단을 통한 자연 감속
*   **`custom_esp_servo`**: 조향 서보 모터를 제어합니다.
    *   `custom_servo_set_angle(angle)`: -45도 ~ +45도 범위로 각도 제한
*   **`custom_esp_uart`**: 라즈베리파이와의 통신을 담당합니다. 링 버퍼를 사용하여 비동기 수신을 처리합니다.

### 5.2. APP Level
*   **`RCcar_protocol`**: 수신된 바이트 스트림에서 패킷을 파싱하고, 체크섬을 검증합니다.
*   **`RCcar_mode`**: 차량의 현재 동작 모드를 관리합니다.
    *   **IDLE (Yellow)**: 대기 상태, 정지
    *   **MANUAL (Blue)**: 사용자 직접 제어
    *   **AUTO (Purple)**: AI 자율 주행
    *   **ERROR (Red)**: 통신 두절 또는 오류 발생 시 정지
*   **`RCcar_control`**: 메인 제어 루프입니다. UART로 수신된 명령을 해석하여 모드를 전환하고, `hal_level` 모듈을 통해 하드웨어를 제어합니다.

---

## 6. 테스트 및 사용 방법

### 6.1. 빌드 및 플래시
ESP-IDF 환경에서 다음 명령어로 빌드하고 업로드합니다.
```bash
idf.py build
idf.py -p COMx flash monitor
```

### 6.2. 라즈베리파이 연동 테스트
프로젝트 루트에 포함된 `rpi_test.py` 스크립트를 라즈베리파이에서 실행하여 통신 및 제어 기능을 테스트할 수 있습니다.
```bash
python3 rpi_test.py
```
*   **주의**: 라즈베리파이의 UART 핀(TX/RX)과 ESP32의 UART 핀이 교차 연결(TX-RX, RX-TX)되어야 합니다.

---

## 7. 개발 이력 (History)
*   **2025-12-04**: Phase 1 (기본 하드웨어 제어), Phase 2 (UART 통신), Phase 3 (앱 로직) 구현 완료.
    *   GPIO 핀맵 재정의 및 PWM 설정
    *   모터/서보 제어 모듈 구현
    *   UART 프로토콜 설계 및 구현
    *   모드 관리 및 메인 제어 로직 통합
