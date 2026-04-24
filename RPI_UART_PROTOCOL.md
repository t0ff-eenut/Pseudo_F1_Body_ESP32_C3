# ESP32-C3 BCU UART 제어 프로토콜 — 라즈베리파이 참조 문서

## 통신 설정

| 항목 | 값 |
|------|-----|
| 포트 | `/dev/ttyAMA0` (또는 `/dev/serial0`) |
| Baudrate | 115200 |
| Data bits | 8 |
| Parity | None |
| Stop bits | 1 |
| Flow control | None |

---

## 패킷 구조 (6바이트 고정)

```
[STX] [CMD] [P1] [P2] [CHECKSUM] [ETX]
 0xAA  1바이트 1바이트 1바이트  XOR    0x55
```

- **STX**: `0xAA` (고정)
- **CMD**: 명령어 코드
- **P1**: 파라미터 1 (`int8_t`, signed)
- **P2**: 파라미터 2 (`int8_t`, signed)
- **CHECKSUM**: `CMD ^ P1 ^ P2` (XOR)
- **ETX**: `0x55` (고정)

> Python에서 signed int8 → unsigned byte 변환:
> ```python
> p1_byte = p1 if p1 >= 0 else (256 + p1)
> ```

---

## 커맨드 목록

### `0x01` CMD_CONTROL — 일반 주행 제어 ✅ 권장

| 파라미터 | 타입 | 범위 | 의미 | 비고 |
|---------|------|------|------|------|
| P1 | int8_t | `-127 ~ +127` | 스로틀 | 음수=후진, 양수=전진, 0=코스팅 |
| P2 | int8_t | `-127 ~ +127` | 조향 | 음수=좌, 양수=우, 0=직진 |

**내부 변환:**
- P1 → 모터 PWM: `|P1| * 1023 / 127` (0~1023, 10-bit)
- P2 → 서보 각도: `P2 * 90 / 127` (-90°~+90°)
- 서보 각도 → 펄스폭: `1500 + (angle * 2000 / 180)` us (500~2500us)

**예시:**
```python
# 전진 100%, 직진
send(CMD=0x01, P1=127, P2=0)

# 전진 50%, 우측 최대 조향
send(CMD=0x01, P1=64, P2=127)

# 후진 75%, 좌측 조향 45°
send(CMD=0x01, P1=-96, P2=-64)

# 정지 (코스팅)
send(CMD=0x01, P1=0, P2=0)
```

---

### `0x03` CMD_BRAKE — 브레이크 ✅ 권장

| 파라미터 | 타입 | 범위 | 의미 | 비고 |
|---------|------|------|------|------|
| P1 | int8_t | `0 ~ 127` | 전륜 브레이크 강도 | 0=코스팅, 127=최대 제동 |
| P2 | int8_t | `0 ~ 127` | 후륜 브레이크 강도 | 0=코스팅, 127=최대 제동 |

**원리:** IN1과 IN2에 동일한 PWM duty → HIGH 구간=쇼트 브레이크, LOW 구간=코스팅
전/후륜 독립 제어 가능.

**예시:**
```python
# 전/후륜 모두 최대 제동
send(CMD=0x03, P1=127, P2=127)

# 후륜만 강하게 제동 (드리프트 진입)
send(CMD=0x03, P1=0, P2=127)

# 50% 제동
send(CMD=0x03, P1=64, P2=64)
```

---

### `0xFF` CMD_EMERGENCY_STOP — 비상 정지 ✅

| 파라미터 | 값 | 의미 |
|---------|-----|------|
| P1 | `0` | 무시 |
| P2 | `0` | 무시 |

모든 모터 즉시 코스팅 정지. P1, P2는 `0`으로 고정.

```python
send(CMD=0xFF, P1=0, P2=0)
```

---

### `0x00` CMD_HEARTBEAT — 연결 확인 (스텁)

| 파라미터 | 값 |
|---------|-----|
| P1 | `0` |
| P2 | `0` |

> ⚠️ 현재 수신만 되고 ACK 응답 없음. 추후 구현 예정.

---

### `0x02` CMD_SET_MODE — 모드 설정 (스텁)

> ⚠️ 현재 미구현. 추후 수동/자율주행 모드 전환에 사용 예정.

---

## Python 패킷 생성 예시

```python
import serial

STX = 0xAA
ETX = 0x55

def send_packet(ser, cmd, p1, p2):
    p1_b = p1 & 0xFF  # signed → unsigned
    p2_b = p2 & 0xFF
    cs   = cmd ^ p1_b ^ p2_b
    ser.write(bytes([STX, cmd, p1_b, p2_b, cs, ETX]))

ser = serial.Serial('/dev/ttyAMA0', 115200, timeout=1)

# 전진 100%, 직진
send_packet(ser, 0x01, 127, 0)

# 후륜만 브레이크 (드리프트 진입)
send_packet(ser, 0x03, 0, 127)

# 비상 정지
send_packet(ser, 0xFF, 0, 0)
```

---

## 커맨드 요약 테이블

| CMD | 코드 | P1 범위 | P2 범위 | 상태 |
|-----|------|---------|---------|------|
| CMD_HEARTBEAT | `0x00` | 0 | 0 | ⚠️ 스텁 |
| CMD_CONTROL | `0x01` | -127~+127 (스로틀) | -127~+127 (조향) | ✅ 구현 |
| CMD_BRAKE | `0x03` | 0~127 (전륜 강도) | 0~127 (후륜 강도) | ✅ 구현 |
| CMD_EMERGENCY_STOP | `0xFF` | 0 | 0 | ✅ 구현 |
