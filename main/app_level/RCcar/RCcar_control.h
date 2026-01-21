#ifndef RCCAR_CONTROL_H
#define RCCAR_CONTROL_H

#include "app_level_top.h"
#include "RCcar_protocol.h"

/**
 * @brief Initialize RC Car Control Logic
 */
bool rccar_control_init(void);

/**
 * @brief Main Control Task (Run in loop)
 */
void rccar_control_task(void);

/**
 * @brief Process incoming byte from UART
 */
void rccar_control_process_byte(uint8_t byte);

#endif // RCCAR_CONTROL_H
