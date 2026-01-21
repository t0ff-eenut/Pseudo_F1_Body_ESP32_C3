#include "RCcar_control.h"

#define CONTROL_DEBUG DEBUG
static const char *TAG = "[@]RCcar_control";

bool rccar_control_init(void) {
    #if CONTROL_DEBUG
    printf("[%s] [시작] rccar_control_init()\n", custom_getRuntimeString());
    #endif

    rccar_protocol_init();

    // 모터 정지 및 서보 중앙 정렬 확인
    custom_motor_stop_all();
    custom_servo_center();

    #if CONTROL_DEBUG
    printf("[%s] [완료] rccar_control_init()\n", custom_getRuntimeString());
    #endif
    return true;
}

void rccar_control_process_byte(uint8_t byte) {
    rc_packet_t packet;
    if (rccar_protocol_parse_byte(byte, &packet)) {
        // 유효한 패킷 수신됨
        switch (packet.cmd) {
            case CMD_CONTROL:
                // Param1: 속도 (Throttle)
                // Param2: 각도 (Steering)
                custom_motor_set_both(packet.param1, packet.param1);
                custom_servo_set_angle(packet.param2);
                break;

            case CMD_SET_MODE:
                // 모드 변경 구현 시 사용
                break;

            case CMD_EMERGENCY_STOP:
                custom_motor_stop_all();
                break;

            case CMD_HEARTBEAT:
                // 워치독 리셋 등 구현 시 사용
                break;

            default:
                break;
        }
    }
}

void rccar_control_task(void) {
    // UART 버퍼의 모든 바이트를 처리
    uint8_t data;
    while (custom_uart_read_byte(&data)) {
        rccar_control_process_byte(data);
    }
}
