#include "RCcar_control.h"

#define CONTROL_DEBUG DEBUG
static const char *TAG = "[@]RCcar_control";

bool rccar_control_init(void) {
    #if CONTROL_DEBUG
    printf("[%s] [Start] rccar_control_init()\n", custom_getRuntimeString());
    #endif

    rccar_protocol_init();

    // Ensure motors are stopped
    custom_motor_stop_all();
    custom_servo_center();

    #if CONTROL_DEBUG
    printf("[%s] [Done] rccar_control_init()\n", custom_getRuntimeString());
    #endif
    return true;
}

void rccar_control_process_byte(uint8_t byte) {
    rc_packet_t packet;
    if (rccar_protocol_parse_byte(byte, &packet)) {
        // Valid packet received
        switch (packet.cmd) {
            case CMD_CONTROL:
                // Param1: Speed (Throttle)
                // Param2: Angle (Steering)
                custom_motor_set_both(packet.param1, packet.param1);
                custom_servo_set_angle(packet.param2);
                break;

            case CMD_SET_MODE:
                // Handle mode change if implemented
                break;

            case CMD_EMERGENCY_STOP:
                custom_motor_stop_all();
                break;

            case CMD_HEARTBEAT:
                // Reset watchdog if implemented
                break;

            default:
                break;
        }
    }
}

void rccar_control_task(void) {
    // Process all available bytes in the UART buffer
    uint8_t data;
    while (custom_uart_read_byte(&data)) {
        rccar_control_process_byte(data);
    }
}
