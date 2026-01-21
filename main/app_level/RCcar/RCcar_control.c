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
                // Param1: 속도 (Throttle) -100 ~ 100
                // Param2: 각도 (Steering) -100 ~ 100
                custom_motor_set_both(packet.param1, packet.param1);
                custom_servo_set_angle(packet.param2);
                break;

            case CMD_CONTROL_RAW: {
                // 직접 PWM 제어 (모터)
                // Param1: PWM 상위 바이트 (0~3, 10-bit의 상위 2비트)
                // Param2: PWM 하위 바이트 (0~255, 10-bit의 하위 8비트)
                // 16-bit 재구성: PWM = (param1 << 8) | (param2 & 0xFF)
                uint16_t motor_pwm = ((uint16_t)(packet.param1 & 0xFF) << 8) | (uint8_t)packet.param2;
                
                // 10-bit 범위 제한 (0 ~ 1023)
                if (motor_pwm > 1023) motor_pwm = 1023;
                
                // PWM을 속도로 변환 (-100 ~ 100)
                // 양수로만 처리 (방향은 별도 명령어로 확장 가능)
                int8_t speed = (int8_t)((motor_pwm * 100) / 1023);
                custom_motor_set_both(speed, speed);
                
                #if CONTROL_DEBUG
                printf("[%s] CMD_CONTROL_RAW: PWM=%d, Speed=%d\n", 
                       custom_getRuntimeString(), motor_pwm, speed);
                #endif
                break;
            }

            case CMD_SERVO_RAW: {
                // 직접 PWM 제어 (서보)
                // Param1: PWM 상위 바이트 (0~31, 13-bit의 상위 5비트)
                // Param2: PWM 하위 바이트 (0~255, 13-bit의 하위 8비트)
                uint16_t servo_pwm = ((uint16_t)(packet.param1 & 0xFF) << 8) | (uint8_t)packet.param2;
                
                // 13-bit 범위 제한 (0 ~ 8191)
                if (servo_pwm > 8191) servo_pwm = 8191;
                
                // PWM을 각도로 역변환
                // PWM = (pulse_us * 8192) / 20000
                // pulse_us = (PWM * 20000) / 8192
                uint32_t pulse_us = (servo_pwm * 20000) / 8192;
                
                // pulse_us를 각도로 변환
                // pulse_us = 1500 + (angle * 1000 / 90)
                // angle = (pulse_us - 1500) * 90 / 1000
                int16_t angle = ((int32_t)pulse_us - 1500) * 90 / 1000;
                
                custom_servo_set_angle(angle);
                
                #if CONTROL_DEBUG
                printf("[%s] CMD_SERVO_RAW: PWM=%d, Pulse=%luus, Angle=%d\n", 
                       custom_getRuntimeString(), servo_pwm, pulse_us, angle);
                #endif
                break;
            }

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
