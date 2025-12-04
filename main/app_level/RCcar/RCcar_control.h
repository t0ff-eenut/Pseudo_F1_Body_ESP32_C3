#ifndef RCCAR_CONTROL_H
#define RCCAR_CONTROL_H

#include "app_level_top.h"

/*
******************************************************************************
* RC Car Main Control Logic
******************************************************************************
* Integrates UART, Protocol, Mode, Motor, Servo
******************************************************************************
*/

/**
 * @brief       Initialize Control Logic
 * @return      bool    true: success
 */
bool rccar_control_init(void);

/**
 * @brief       Main Control Loop Task (call periodically)
 */
void rccar_control_task(void);

/**
 * @brief       Process incoming UART data
 */
void rccar_control_process_uart(void);

#endif // RCCAR_CONTROL_H
