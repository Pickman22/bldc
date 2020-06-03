#ifndef APP_TASK_H
#define APP_TASK_H

#include "mtrif.h"

/*-----------------------------------------------------------------------------
 * Signal logging task definitions.
 *-----------------------------------------------------------------------------*/
#ifndef ENBL_MOTOR_IDENT
#define APP_TASK_SLOG_TS (10)
#else
/* When motor identification is enabled, it is useful
 * to transmit data faster to capture fast dybamics. */
#define APP_TASK_SLOG_TS (1)
#endif
#define APP_TASK_SLOG_PRIO (2)
/* Stack size: word size, not bytes: 300 implies 1200 bytes. */
#define APP_TASK_SLOG_STACK_SIZE (100)

/*-----------------------------------------------------------------------------
 * Motor control task definitions.
 *-----------------------------------------------------------------------------*/
#define APP_TASK_MOTOR_CONTROL_TS (1)
#define APP_TASK_MOTOR_CONTROL_PRIO (3)
#define APP_TASK_MOTOR_CONTROL_STACK_SIZE (100)
#define APP_TASK_MOTOR_CONTROL_N_SIGNALS (5)

void AppTask_SLog(void* params);

void AppTask_MotorControl(void* params);

/* MtrIf_S* AppTask_GetMtrIf(void); */

#endif // APP_TASK_H
