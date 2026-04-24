#ifndef RCCAR_CONTROL_H
#define RCCAR_CONTROL_H

#include "app_level_top.h"
#include "RCcar_protocol.h"

// ─── Failsafe 파라미터 (사용자 설정) ──────────────────────────────────────
//
// 동작 시나리오 (기본값 기준):
//
//   [패킷 수신 끊김]
//          │
//          ▼  FAILSAFE_TIMEOUT_MS = 500ms 경과
//   [페일세이프 진입]
//          │
//          ├─ 매 FAILSAFE_STEP_INTERVAL_MS (50ms) 마다 한 단계씩 감속/정렬
//          │
//          ▼  모터 감속
//   최대속도(127) → 0 : ceil(127 / MOTOR_STEP) × STEP_INTERVAL
//                      = ceil(127 / 10)        × 50ms  = 700ms
//
//          ▼  서보 정렬
//   최대조향(90°) → 0° : ceil(90 / SERVO_STEP) × STEP_INTERVAL
//                       = ceil(90 / 5)          × 50ms  = 900ms
//
// 파라미터 조정 가이드:
//   FAILSAFE_TIMEOUT_MS        ↑ : 연결 불안정 시 오작동 위험 ↑, 반응 느림
//                              ↓ : 일시적 지연에도 페일세이프 진입 위험
//   FAILSAFE_STEP_INTERVAL_MS  ↑ : 감속 구간 길어짐 (부드러움 ↑)
//                              ↓ : 감속 구간 짧아짐 (급정지에 가까워짐)
//   FAILSAFE_MOTOR_STEP        ↑ : 빠르게 감속 (정지 시간 ↓)
//                              ↓ : 천천히 감속 (정지 시간 ↑)
//   FAILSAFE_SERVO_STEP        ↑ : 빠르게 직진 정렬
//                              ↓ : 천천히 직진 정렬
//
// 패킷이 이 시간(ms) 동안 수신되지 않으면 페일세이프 진입
#define FAILSAFE_TIMEOUT_MS        500
// 점진적 단계 적용 주기 (ms) - 값이 작을수록 부드럽게 감속
#define FAILSAFE_STEP_INTERVAL_MS   50
// 매 주기마다 줄어드는 속도량 (0~127 단위) - 값이 클수록 빠르게 감속
#define FAILSAFE_MOTOR_STEP         10
// 매 주기마다 중앙으로 이동하는 서보 각도 (도) - 값이 클수록 빠르게 정렬
#define FAILSAFE_SERVO_STEP          5
// ──────────────────────────────────────────────────────────────────────────

/**
 * @brief RC카 제어 로직 초기화
 */
bool rccar_control_init(void);

/**
 * @brief 메인 제어 태스크 (루프에서 실행)
 */
void rccar_control_task(void);

/**
 * @brief 수신된 바이트 처리
 */
void rccar_control_process_byte(uint8_t byte);

#endif // RCCAR_CONTROL_H
