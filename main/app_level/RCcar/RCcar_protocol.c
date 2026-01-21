#include "RCcar_protocol.h"

// Parsing state machine
typedef enum {
    STATE_WAIT_STX,
    STATE_WAIT_CMD,
    STATE_WAIT_P1,
    STATE_WAIT_P2,
    STATE_WAIT_CS,
    STATE_WAIT_ETX
} parse_state_t;

static parse_state_t current_state = STATE_WAIT_STX;
static rc_packet_t temp_packet;

void rccar_protocol_init(void) {
    current_state = STATE_WAIT_STX;
}

bool rccar_protocol_parse_byte(uint8_t byte, rc_packet_t *out_packet) {
    bool packet_ready = false;

    switch (current_state) {
        case STATE_WAIT_STX:
            if (byte == PROTOCOL_STX) {
                temp_packet.stx = byte;
                current_state = STATE_WAIT_CMD;
            }
            break;

        case STATE_WAIT_CMD:
            temp_packet.cmd = byte;
            current_state = STATE_WAIT_P1;
            break;

        case STATE_WAIT_P1:
            temp_packet.param1 = (int8_t)byte;
            current_state = STATE_WAIT_P2;
            break;

        case STATE_WAIT_P2:
            temp_packet.param2 = (int8_t)byte;
            current_state = STATE_WAIT_CS;
            break;

        case STATE_WAIT_CS:
            temp_packet.checksum = byte;
            current_state = STATE_WAIT_ETX;
            break;

        case STATE_WAIT_ETX:
            if (byte == PROTOCOL_ETX) {
                temp_packet.etx = byte;

                // Verify checksum
                uint8_t calculated_cs = temp_packet.cmd ^ (uint8_t)temp_packet.param1 ^ (uint8_t)temp_packet.param2;
                if (calculated_cs == temp_packet.checksum) {
                    *out_packet = temp_packet;
                    packet_ready = true;
                }
            }
            // Reset state regardless of success/fail, or after success
            current_state = STATE_WAIT_STX;
            break;

        default:
            current_state = STATE_WAIT_STX;
            break;
    }

    return packet_ready;
}

void rccar_protocol_create_packet(rc_cmd_type_t cmd, int8_t p1, int8_t p2, rc_packet_t *out_packet) {
    out_packet->stx = PROTOCOL_STX;
    out_packet->cmd = (uint8_t)cmd;
    out_packet->param1 = p1;
    out_packet->param2 = p2;
    out_packet->checksum = (uint8_t)cmd ^ (uint8_t)p1 ^ (uint8_t)p2;
    out_packet->etx = PROTOCOL_ETX;
}
