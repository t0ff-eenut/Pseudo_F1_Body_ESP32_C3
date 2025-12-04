/*
******************************************************************************
* File Name          : RCcar_control.c
* Description        : RC Car Main Control Logic Implementation
******************************************************************************
* Handles UART input, updates mode, controls motors/servo
******************************************************************************
* first update : 2025/12/04
******************************************************************************
*/

#include "RCcar_control.h"

#define CONTROL_DEBUG  DEBUG
static const char *RCcar_control_TAG = "[@]RCcar_control.c";

static uint8_t s_rx_buffer[UART_BUF_SIZE];

bool rccar_control_init(void) {
    #if CONTROL_DEBUG
    printf("[%s] "COLOR_WHITE"[Start]\t %s rccar_control_init()\n" COLOR_RESET, custom_getRuntimeString(), RCcar_control_TAG);
    #endif

    bool b_success = true;
    b_success &= rccar_mode_init();

    #if CONTROL_DEBUG
    printf("[%s] "COLOR_GREEN"[Done]\t %s Control logic initialized\n" COLOR_RESET, custom_getRuntimeString(), RCcar_control_TAG);
    #endif

    return b_success;
}

void rccar_control_process_uart(void) {
    // Read UART data
    int len = custom_uart_read(s_rx_buffer, PROTOCOL_PACKET_SIZE, 10); // Short timeout for non-blocking feel
    
    if (len > 0) {
        cps packet;
        if (protocol_parse_packet(s_rx_buffer, len, &packet)) {
            
            // Handle Mode Switching based on Command
            switch (packet.ui8_cmd_type) {
                case CMD_MANUAL_CONTROL:
                    rccar_mode_set(MODE_MANUAL);
                    break;
                case CMD_AUTO_CONTROL:
                    rccar_mode_set(MODE_AUTO);
                    break;
                case CMD_STOP:
                    rccar_mode_set(MODE_IDLE); // Or stay in current mode but stop? Let's go IDLE for safety
                    break;
                case CMD_STATUS_REQ:
                    // Status request doesn't change mode
                    break;
                default:
                    break;
            }

            // Execute Command if allowed in current mode
            rme current_mode = rccar_mode_get();
            
            if (current_mode == MODE_MANUAL && packet.ui8_cmd_type == CMD_MANUAL_CONTROL) {
                protocol_process_command(&packet);
            }
            else if (current_mode == MODE_AUTO && packet.ui8_cmd_type == CMD_AUTO_CONTROL) {
                protocol_process_command(&packet);
            }
            else if (packet.ui8_cmd_type == CMD_STOP) {
                // Always allow STOP
                protocol_process_command(&packet);
            }
            else if (packet.ui8_cmd_type == CMD_STATUS_REQ) {
                // Always allow Status Request
                protocol_process_command(&packet);
            }
        }
    }
}

void rccar_control_task(void) {
    // 1. Process UART Input
    rccar_control_process_uart();

    // 2. Mode specific background tasks
    rccar_mode_task();

    // 3. Failsafe (optional): Check if UART timeout -> Stop car
    // TODO: Implement failsafe
}
