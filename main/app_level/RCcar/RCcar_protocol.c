/*
******************************************************************************
* File Name          : RCcar_protocol.c
* Description        : RC Car Communication Protocol Implementation
******************************************************************************
* Packet parsing and command processing for Raspberry Pi communication
******************************************************************************
* first update : 2025/12/04
******************************************************************************
*/

#include "RCcar_protocol.h"

#define PROTOCOL_DEBUG  DEBUG
static const char *RCcar_protocol_TAG = "[@]RCcar_protocol.c";

uint8_t protocol_calc_checksum(const uint8_t* p_data, uint16_t ui16_len) {
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < ui16_len; i++) {
        checksum ^= p_data[i];
    }
    return checksum;
}

bool protocol_parse_packet(const uint8_t* p_data, uint16_t ui16_len, cps* p_packet) {
    if (p_data == NULL || p_packet == NULL) {
        return false;
    }

    if (ui16_len < PROTOCOL_PACKET_SIZE) {
        #if PROTOCOL_DEBUG
        printf("[%s] "COLOR_YELLOW"[Warn]\t %s Packet too short: %d bytes\n" COLOR_RESET, 
               custom_getRuntimeString(), RCcar_protocol_TAG, ui16_len);
        #endif
        return false;
    }

    // Check header
    if (p_data[0] != PROTOCOL_HEADER) {
        #if PROTOCOL_DEBUG
        printf("[%s] "COLOR_YELLOW"[Warn]\t %s Invalid header: 0x%02X\n" COLOR_RESET, 
               custom_getRuntimeString(), RCcar_protocol_TAG, p_data[0]);
        #endif
        return false;
    }

    // Verify checksum
    uint8_t calc_checksum = protocol_calc_checksum(p_data, PROTOCOL_PACKET_SIZE - 1);
    if (calc_checksum != p_data[PROTOCOL_PACKET_SIZE - 1]) {
        #if PROTOCOL_DEBUG
        printf("[%s] "COLOR_RED"[Error]\t %s Checksum mismatch: calc=0x%02X, recv=0x%02X\n" COLOR_RESET, 
               custom_getRuntimeString(), RCcar_protocol_TAG, calc_checksum, p_data[PROTOCOL_PACKET_SIZE - 1]);
        #endif
        return false;
    }

    // Parse packet
    p_packet->ui8_header = p_data[0];
    p_packet->ui8_cmd_type = p_data[1];
    p_packet->i8_throttle = (int8_t)p_data[2];
    p_packet->i8_steering = (int8_t)p_data[3];
    p_packet->ui8_checksum = p_data[4];

    #if PROTOCOL_DEBUG
    printf("[%s] "COLOR_GREEN"[RX]\t %s Packet: CMD=0x%02X THR=%d STR=%d\n" COLOR_RESET, 
           custom_getRuntimeString(), RCcar_protocol_TAG, 
           p_packet->ui8_cmd_type, p_packet->i8_throttle, p_packet->i8_steering);
    #endif

    return true;
}

void protocol_build_status_response(sps* p_packet) {
    if (p_packet == NULL) return;

    mss motor_state = custom_motor_get_state();
    int16_t servo_angle = custom_servo_get_angle();

    p_packet->ui8_header = PROTOCOL_HEADER;
    p_packet->ui8_cmd_type = CMD_STATUS_RESP;
    p_packet->i8_motor_left = motor_state.i8_speed_left;
    p_packet->i8_motor_right = motor_state.i8_speed_right;
    p_packet->i8_servo_angle = (int8_t)servo_angle;
    
    // Calculate checksum (excluding checksum byte itself)
    uint8_t* p_data = (uint8_t*)p_packet;
    p_packet->ui8_checksum = protocol_calc_checksum(p_data, sizeof(sps) - 1);
}

bool protocol_send_status(void) {
    sps status_packet;
    protocol_build_status_response(&status_packet);

    int bytes_sent = custom_uart_send((uint8_t*)&status_packet, sizeof(sps));
    
    #if PROTOCOL_DEBUG
    printf("[%s] "COLOR_GREEN"[TX]\t %s Status: L=%d R=%d S=%d\n" COLOR_RESET,
           custom_getRuntimeString(), RCcar_protocol_TAG,
           status_packet.i8_motor_left, status_packet.i8_motor_right, status_packet.i8_servo_angle);
    #endif

    return (bytes_sent == sizeof(sps));
}

void protocol_process_command(const cps* p_packet) {
    if (p_packet == NULL) return;

    #if PROTOCOL_DEBUG
    printf("[%s] "COLOR_WHITE"[CMD]\t %s Processing command: 0x%02X\n" COLOR_RESET,
           custom_getRuntimeString(), RCcar_protocol_TAG, p_packet->ui8_cmd_type);
    #endif

    switch (p_packet->ui8_cmd_type) {
        case CMD_MANUAL_CONTROL:
        case CMD_AUTO_CONTROL:
            // Apply throttle to motors
            custom_motor_set_both(p_packet->i8_throttle, p_packet->i8_throttle);
            // Apply steering to servo
            custom_servo_set_angle(p_packet->i8_steering);
            break;

        case CMD_STOP:
            // Emergency stop
            custom_motor_brake();
            custom_servo_center();
            custom_gpio_set_led_strip_color(255, 0, 0);  // Red for emergency
            #if PROTOCOL_DEBUG
            printf("[%s] "COLOR_RED"[STOP]\t %s Emergency stop activated!\n" COLOR_RESET,
                   custom_getRuntimeString(), RCcar_protocol_TAG);
            #endif
            break;

        case CMD_STATUS_REQ:
            // Send status response
            protocol_send_status();
            break;

        default:
            #if PROTOCOL_DEBUG
            printf("[%s] "COLOR_YELLOW"[Warn]\t %s Unknown command: 0x%02X\n" COLOR_RESET,
                   custom_getRuntimeString(), RCcar_protocol_TAG, p_packet->ui8_cmd_type);
            #endif
            break;
    }
}
