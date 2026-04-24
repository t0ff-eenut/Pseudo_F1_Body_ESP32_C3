#ifndef RCCAR_PROTOCOL_H
#define RCCAR_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

// 프로토콜 상수 정의
#define PROTOCOL_STX            0xAA    // 시작 바이트 (Start of Text)
#define PROTOCOL_ETX            0x55    // 종료 바이트 (End of Text)
#define PROTOCOL_PACKET_SIZE    6       // 패킷 크기

// 명령어 타입 (Command Types)
typedef enum {
    CMD_HEARTBEAT       = 0x00, // 연결 확인 (Heartbeat)
    CMD_CONTROL         = 0x01, // 제어 명령 (param1: 속도 -127~+127, param2: 조향 -127~+127)
    CMD_BRAKE           = 0x03, // 브레이크 (param1: 전륜 강도 0~127, param2: 후륜 강도 0~127)
    CMD_EMERGENCY_STOP  = 0xFF  // 비상 정지
} rc_cmd_type_t;

// 패킷 구조체 정의
typedef struct {
    uint8_t stx;        // 시작 바이트
    uint8_t cmd;        // 명령어
    int8_t  param1;     // 파라미터 1 (예: 속도 -127 ~ +127)
    int8_t  param2;     // 파라미터 2 (예: 조향 -127 ~ +127)
    uint8_t checksum;   // 체크섬 (XOR 연산)
    uint8_t etx;        // 종료 바이트
} __attribute__((packed)) rc_packet_t;

/**
 * @brief 프로토콜 핸들러 초기화
 */
void rccar_protocol_init(void);

/**
 * @brief UART 수신 바이트 파싱
 *
 * @param byte 수신된 바이트
 * @param out_packet 파싱 완료된 패킷을 저장할 포인터
 * @return true 패킷 완성됨, false 진행 중 또는 실패
 */
bool rccar_protocol_parse_byte(uint8_t byte, rc_packet_t *out_packet);

/**
 * @brief 패킷 생성
 *
 * @param cmd 명령어
 * @param p1 파라미터 1
 * @param p2 파라미터 2
 * @param out_packet 생성된 패킷을 저장할 포인터
 */
void rccar_protocol_create_packet(rc_cmd_type_t cmd, int8_t p1, int8_t p2, rc_packet_t *out_packet);

#endif // RCCAR_PROTOCOL_H
