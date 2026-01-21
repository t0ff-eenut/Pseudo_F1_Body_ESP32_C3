#ifndef RCCAR_PROTOCOL_H
#define RCCAR_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>

// Protocol Constants
#define PROTOCOL_STX            0xAA
#define PROTOCOL_ETX            0x55
#define PROTOCOL_PACKET_SIZE    6

// Command Types
typedef enum {
    CMD_HEARTBEAT       = 0x00,
    CMD_CONTROL         = 0x01,  // param1: throttle, param2: steering
    CMD_SET_MODE        = 0x02,  // param1: mode
    CMD_EMERGENCY_STOP  = 0xFF
} rc_cmd_type_t;

// Packet Structure
typedef struct {
    uint8_t stx;        // Start of Text
    uint8_t cmd;        // Command Type
    int8_t  param1;     // Parameter 1 (e.g., Throttle: -100 to 100)
    int8_t  param2;     // Parameter 2 (e.g., Steering: -100 to 100)
    uint8_t checksum;   // XOR Checksum of cmd, param1, param2
    uint8_t etx;        // End of Text
} __attribute__((packed)) rc_packet_t;

/**
 * @brief Initialize protocol handler
 */
void rccar_protocol_init(void);

/**
 * @brief Parse a single byte from UART
 *
 * @param byte Received byte
 * @param out_packet Pointer to store the parsed packet when complete
 * @return true if a valid packet is completed, false otherwise
 */
bool rccar_protocol_parse_byte(uint8_t byte, rc_packet_t *out_packet);

/**
 * @brief Create a packet
 *
 * @param cmd Command type
 * @param p1 Parameter 1
 * @param p2 Parameter 2
 * @param out_packet Pointer to store the created packet
 */
void rccar_protocol_create_packet(rc_cmd_type_t cmd, int8_t p1, int8_t p2, rc_packet_t *out_packet);

#endif // RCCAR_PROTOCOL_H
