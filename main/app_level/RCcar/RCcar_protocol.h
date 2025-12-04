#ifndef RCCAR_PROTOCOL_H
#define RCCAR_PROTOCOL_H

#include "hal_level_handle.h"

/*
******************************************************************************
* RC Car Communication Protocol
******************************************************************************
* Packet Format:
* [HEADER][CMD][THROTTLE][STEERING][CHECKSUM]
*  0xAA    1B    1B        1B        1B
*
* Commands:
* - 0x01: Manual Control (throttle + steering)
* - 0x02: Auto Control (AI command)
* - 0x03: Stop (emergency stop)
* - 0x10: Status Request
* - 0x11: Status Response
******************************************************************************
*/

#define PROTOCOL_HEADER         0xAA
#define PROTOCOL_PACKET_SIZE    5

/**
 * @enum        command_type_enum(cte)
 * @brief       Command Type Enum
 */
typedef enum command_type_enum {
    CMD_MANUAL_CONTROL = 0x01,
    CMD_AUTO_CONTROL   = 0x02,
    CMD_STOP           = 0x03,
    CMD_STATUS_REQ     = 0x10,
    CMD_STATUS_RESP    = 0x11,
    CMD_UNKNOWN        = 0xFF,
} cte;

/**
 * @struct      command_packet_struct(cps)
 * @brief       Command Packet Structure
 */
typedef struct command_packet_struct {
    uint8_t ui8_header;       // 0xAA
    uint8_t ui8_cmd_type;     // Command type
    int8_t  i8_throttle;      // -100 ~ +100 (motor speed)
    int8_t  i8_steering;      // -45 ~ +45 (servo angle)
    uint8_t ui8_checksum;     // XOR checksum
} cps;

/**
 * @struct      status_packet_struct(sps)
 * @brief       Status Response Packet Structure
 */
typedef struct status_packet_struct {
    uint8_t ui8_header;       // 0xAA
    uint8_t ui8_cmd_type;     // CMD_STATUS_RESP
    int8_t  i8_motor_left;    // Current left motor speed
    int8_t  i8_motor_right;   // Current right motor speed
    int8_t  i8_servo_angle;   // Current servo angle
    uint8_t ui8_checksum;     // XOR checksum
} sps;

/**
 * @brief       Parse received packet
 * @param[in]   p_data : Raw data buffer
 * @param[in]   ui16_len : Data length
 * @param[out]  p_packet : Parsed packet structure
 * @return      bool    true: valid packet, false: invalid
 */
bool protocol_parse_packet(const uint8_t* p_data, uint16_t ui16_len, cps* p_packet);

/**
 * @brief       Calculate checksum
 * @param[in]   p_data : Data buffer (excluding checksum byte)
 * @param[in]   ui16_len : Data length
 * @return      uint8_t Calculated checksum (XOR of all bytes)
 */
uint8_t protocol_calc_checksum(const uint8_t* p_data, uint16_t ui16_len);

/**
 * @brief       Build status response packet
 * @param[out]  p_packet : Status packet to fill
 */
void protocol_build_status_response(sps* p_packet);

/**
 * @brief       Send status response via UART
 * @return      bool    true: success, false: failed
 */
bool protocol_send_status(void);

/**
 * @brief       Process received command
 * @param[in]   p_packet : Parsed command packet
 */
void protocol_process_command(const cps* p_packet);

#endif // RCCAR_PROTOCOL_H
