#ifndef RCCAR_MODE_H
#define RCCAR_MODE_H

#include "app_level_top.h"

/*
******************************************************************************
* RC Car Operation Mode Management
******************************************************************************
* Manages the current state of the vehicle
******************************************************************************
*/

/**
 * @enum        rccar_mode_enum(rme)
 * @brief       RC Car Operation Mode
 */
typedef enum rccar_mode_enum {
    MODE_IDLE = 0,      // 대기 상태 (정지)
    MODE_MANUAL,        // 수동 제어 (라즈베리파이 직접 명령)
    MODE_AUTO,          // 자율 주행 (라즈베리파이 AI 명령)
    MODE_ERROR,         // 오류 상태
} rme;

/**
 * @brief       Initialize Mode Manager
 * @return      bool    true: success
 */
bool rccar_mode_init(void);

/**
 * @brief       Set current operation mode
 * @param[in]   mode : Target mode
 */
void rccar_mode_set(rme mode);

/**
 * @brief       Get current operation mode
 * @return      rme     Current mode
 */
rme rccar_mode_get(void);

/**
 * @brief       Handle mode specific tasks (call in main loop)
 */
void rccar_mode_task(void);

#endif // RCCAR_MODE_H
