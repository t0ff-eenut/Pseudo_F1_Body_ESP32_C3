# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.2.1] - 2026-04-27

### Fixed

#### COM 포트 연결/해제 시 ESP32 의도치 않은 동작 (`pc_uart_controller.py`)
- **연결 시 모터 순간 구동 버그**
  - 원인: `serial.Serial(port, ...)` 생성자가 DTR=True 상태로 포트를 열고, 이후 `dtr=False`를 설정하면서 DTR 펄스가 발생 → ESP32 자동 리셋 트리거
  - 수정: `Serial()` 객체 생성 후 `dtr=False`, `rts=False` 설정 → `open()` 호출 순서로 변경하여 포트 열기 시점부터 DTR 없이 동작
- **연결 해제 시 모터 순간 구동 버그**
  - 원인: `ser.close()` 호출 시 OS가 DTR을 토글 → ESP32 리셋 → 부팅 중 PWM 초기화 직전 순간 모터 반응
  - 수정: `close()` 전에 `dtr=False`, `rts=False` 명시적 설정 후 50ms 대기
  - 추가: 포트 닫기 전 긴급정지 패킷(0xFF) 전송 및 재전송 스레드 `join()` 대기 → 레이스 컨디션 방지

---

## [1.2.0] - 2026-04-27

### Added

#### PC UART 제어 UI (`pc_uart_controller.py`)
- Python tkinter 기반 PC 제어 UI 신규 작성
- 스로틀(-127~+127) / 조향(-127~+127) 슬라이더로 실시간 제어
- 브레이크(전/후륜 독립) 및 긴급정지(0xFF) 버튼
- WASD 키보드 단축키 지원 (W/S=전진·후진, A/D=조향, Space=전체 정지)
- 하트비트 자동 전송 체크박스
- **Failsafe 방지용 150ms 주기 자동 재전송 루프** (슬라이더 고정 시에도 ESP 감속 방지)
- 전송 로그 패널 (HH:MM:SS + 패킷 HEX 표시)
- COM 포트 자동 탐색 및 새로고침

#### USB Serial/JTAG 드라이버 기반 수신 (`hal_level/custom_esp_uart`)
- UART_NUM_1 (GPIO 20/21) 방식 → `usb_serial_jtag_driver` 방식으로 교체
- 플래싱에 사용하는 USB 포트(동일 COM 포트)로 PC ↔ ESP32 양방향 통신 가능
- `usb_serial_jtag_vfs_use_driver()` 호출로 printf 콘솔과 드라이버 충돌 방지
  - 기존: 기본 콘솔 VFS와 드라이버가 동일 RX 인터럽트를 경쟁 → PC 패킷 미수신
  - 수정: VFS 경로를 드라이버로 통일 → 정상 수신

#### `.gitignore`
- `build/`, `managed_components/`, `sdkconfig.old`, Python 캐시, `.vscode/` 제외 설정 추가

### Changed

#### MOTOR_TEST 비활성화 (`main/project_top.h`)
- `MOTOR_TEST true` → `false` (일반 UART 제어 모드로 전환)

### Fixed

#### ESP-IDF v6.0 헤더 경로 분리 대응 (`main/CMakeLists.txt`)
- `driver/gpio.h` → `esp_driver_gpio` 컴포넌트로 분리됨 → `REQUIRES`에 추가
- `driver/uart.h` → `esp_driver_uart` 컴포넌트로 분리됨 → `REQUIRES`에 추가
- `driver/usb_serial_jtag_vfs.h` 의존성 → `esp_driver_usb_serial_jtag`, `vfs` 추가

#### 빌드 경고·에러 수정
- `custom_esp_motor.c`: `int8_t`는 최대 127이므로 `> 127` 비교 항상 false → 해당 줄 제거 (2곳)
- `custom_esp_motor.c`: 미사용 `custom_esp_motor_TAG` 변수 제거
- `custom_esp_nvs.c` / `.h`: `const char*` 배열 반환 함수의 반환 타입 `char*` → `const char*` 수정

---

## [1.1.0] - 2026-04-24

### Added

#### Failsafe: 점진적 감속 및 서보 중앙 정렬 (`app_level/RCcar/RCcar_control`)
- 연결 끊김 감지 시 점진적으로 속도를 낮추고 서보를 직진으로 정렬하는 Failsafe 기능 추가
- `FAILSAFE_TIMEOUT_MS (500)`: 이 시간(ms) 동안 패킷 미수신 시 페일세이프 진입
- `FAILSAFE_STEP_INTERVAL_MS (50)`: 감속·정렬 단계 적용 주기 (ms)
- `FAILSAFE_MOTOR_STEP (10)`: 매 주기마다 줄어드는 속도량 (0~127 단위)
- `FAILSAFE_SERVO_STEP (5)`: 매 주기마다 중앙으로 이동하는 서보 각도 (도)
- 4개 파라미터 모두 `RCcar_control.h` 상단에서 사용자가 직접 수정 가능
- 헤더 내 시뮬레이션 주석 포함 (최대속도→정지 약 700ms, 최대조향→직진 약 900ms)

#### CMD_HEARTBEAT 구현
- 패킷 수신 시 Failsafe 타이머(`s_last_packet_ms`) 갱신
- `CMD_CONTROL`, `CMD_BRAKE`, `CMD_EMERGENCY_STOP` 수신 시에도 타이머 동시 갱신

#### 패킷 수신 상태 추적 변수
- `s_last_packet_ms`: 마지막 유효 패킷 수신 시각 (ms)
- `s_last_step_ms`: 마지막 페일세이프 스텝 적용 시각 (ms)
- `s_current_speed`: 현재 속도 (점진적 감속 추적용, -127~+127)
- `s_current_angle`: 현재 서보 각도 (점진적 정렬 추적용, -90~+90°)

### Changed

#### 프로토콜 정리 (`app_level/RCcar/RCcar_protocol`)
- 불필요 커맨드 제거: `CMD_SET_MODE(0x02)`, `CMD_CONTROL_RAW(0x10)`, `CMD_SERVO_RAW(0x11)`
- 지원 커맨드 정리: `CMD_HEARTBEAT(0x00)`, `CMD_CONTROL(0x01)`, `CMD_BRAKE(0x03)`, `CMD_EMERGENCY_STOP(0xFF)`

#### 속도/조향 해상도 개선 (`hal_level/custom_esp_motor`, `app_level/RCcar/RCcar_control`)
- 속도 범위: -100~+100 퍼센트 → **-127~+127 풀 8-bit 레인지**로 변경
- `speed_to_duty()` 기준값: `100 → 127` (`|speed| * 1023 / 127`)
- `CMD_CONTROL`: `param1`(속도), `param2`(조향) 모두 -127~+127 직접 전달

#### 서보 단위 변환 버그 수정 (`app_level/RCcar/RCcar_control`)
- `param2`를 각도로 직접 사용하던 버그 수정
- 수정 후: `servo_angle = (int16_t)param2 * 90 / 127` (-127~+127 → -90°~+90°)

### Added (이전 세션 반영분)

#### 브레이크 기능 (`hal_level/custom_esp_motor`, `app_level/RCcar/`)
- `CMD_BRAKE(0x03)`: 전륜(`param1`) / 후륜(`param2`) 독립 브레이크 강도 0~127
- `custom_motor_brake_front()`, `custom_motor_brake_rear()`, `custom_motor_brake_all()` 추가
- 쇼트 브레이크 방식: IN1=IN2=동일 PWM duty

#### RPI_UART_PROTOCOL.md
- 라즈베리파이 연동용 UART 프로토콜 참조 문서 작성
- 패킷 구조, 커맨드별 파라미터 범위, Python 예제(`rpi_pwm_control_example.py`) 포함

### Fixed

- `CMD_CONTROL_RAW` 후진 불가 문제 → 커맨드 자체 제거로 해결

### Known Issues / 미구현 항목

- UART 응답 패킷: `custom_uart_send_data` 준비되어 있으나 호출 없음
- NVS: 초기화만 구현, 서보 오프셋/모터 캘리브레이션 저장·로드 미구현
- `Kconfig.projbuild`: blink 예제 기반 항목 잔존 (`BLINK_LED`, `BLINK_PERIOD` 등)
- `MOTOR_TEST`: `project_top.h`에서 수동으로 `false`로 변경 필요

---

## [1.0.0] - 2025-11-21

### Added

#### 프로젝트 초기 구성
- ESP32-C3 Super Mini 기반 F1 RC카 BCU(Body Control Unit) 펌웨어 최초 구현
- ESP-IDF 기반 CMake 빌드 시스템 구성
- 3-tier 레이어 구조 적용: `hw_level` → `hal_level` → `app_level`

#### HW Level (`hw_level/`)
- **`custom_esp_gpio`**: GPIO 초기화 모듈
  - WS2812 LED 스트립 초기화 및 색상 제어 (RMT 드라이버, GPIO 8)
  - BOOT 버튼(GPIO 9) 입력 설정 (NVS 리셋용)
  - ESP32-C3 / 기타 ESP32 모델 분기 핀맵 지원
- **`custom_esp_nvs`**: NVS(비휘발성 저장소) 래퍼 모듈
  - `status` / `config` / `data` 3개 네임스페이스 관리
  - BOOT 버튼 또는 NVS 오류 시 자동 초기화 로직

#### HAL Level (`hal_level/`)
- **`custom_esp_motor`**: DC 모터 제어 모듈
  - 전륜(GPIO 6, 10) / 후륜(GPIO 4, 5) 독립 제어
  - LEDC 10-bit PWM (20kHz), 전진/후진/정지 로직
  - 속도 -100 ~ +100 퍼센트 인터페이스
- **`custom_esp_servo`**: 서보 모터 제어 모듈
  - LEDC 13-bit PWM (50Hz), GPIO 7
  - 각도 -90° ~ +90° 인터페이스, 중앙 정렬 함수
- **`custom_esp_uart`**: UART 통신 모듈
  - UART1, 115200bps, GPIO 20(TX) / 21(RX)
  - 비동기 큐 기반 드라이버 설치
  - `read_byte` (비차단) / `send_data` 인터페이스

#### App Level (`app_level/RCcar/`)
- **`RCcar_protocol`**: 6바이트 패킷 프로토콜 구현
  - 패킷 구조: `[STX(0xAA)] [CMD] [P1] [P2] [XOR-Checksum] [ETX(0x55)]`
  - 상태머신 기반 바이트 단위 파싱
  - XOR 체크섬 검증 및 패킷 생성 함수
  - 지원 커맨드: `CMD_HEARTBEAT(0x00)`, `CMD_CONTROL(0x01)`, `CMD_SET_MODE(0x02)`, `CMD_CONTROL_RAW(0x10)`, `CMD_SERVO_RAW(0x11)`, `CMD_EMERGENCY_STOP(0xFF)`
- **`RCcar_control`**: RC카 제어 로직
  - `CMD_CONTROL`: 속도/조향 퍼센트 제어
  - `CMD_CONTROL_RAW`: 10-bit 모터 PWM 직접 제어
  - `CMD_SERVO_RAW`: 13-bit 서보 PWM 직접 제어
  - `CMD_EMERGENCY_STOP`: 전체 모터 즉시 정지

#### main.c
- 초기화 순서 관리 (GPIO → NVS → Motor → Servo → UART → Control)
- 모터 테스트 모드 (`#if MOTOR_TEST`) / 일반 제어 루프 분기
- 10ms 주기 메인 제어 루프 (`rccar_control_task`)
- 컬러 시리얼 로그 출력

---

### Known Issues / 미구현 항목

- `CMD_SET_MODE (0x02)`: 수신만 되고 모드 전환 로직 없음 (스텁)
- `CMD_HEARTBEAT (0x00)`: ACK 응답 및 워치독 연동 미구현
- `CMD_CONTROL_RAW`: PWM → 속도 변환 시 방향(음수) 손실 (양수만 처리)
- UART 응답 패킷: `custom_uart_send_data` 준비되어 있으나 호출 없음
- NVS: 초기화만 구현, 서보 오프셋/모터 캘리브레이션 저장·로드 미구현
- `Kconfig.projbuild`: blink 예제 기반 항목 잔존 (`BLINK_LED`, `BLINK_PERIOD` 등)
