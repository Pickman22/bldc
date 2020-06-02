#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "apptask.h"
#include "dbg.h"
#include "uart.h"
#include "app.h"
#include "tmr.h"
#include "mtrif.h"
#include "ctrl.h"
#include "rlsq.h"

#define SLOG_SIGBUF_SIZE (10)

/* static MtrIf_S gs_mtr_if = {0}; */

/* static uint32_t TmrCntGenCh; */

typedef struct {
  float Params[3];
  float Err;
  float SpdEst;
} RLSQ_S;

/* static volatile RLSQ_S RLSQ_Output; */

#ifdef __SLOG__
static void _slog(uint32_t* sigbuf) {
  sigbuf[0] = 0x00CD00AB;
  sigbuf[1] = (int32_t)MtrIf_GetCurrent();
  /* sigbuf[2] = (int32_t)MtrIf_GetPos(&gs_mtr_if); */
  /* sigbuf[3] = (int32_t)MtrIf_GetSpd(&gs_mtr_if); */
  sigbuf[4] = (int32_t)MtrIf_GetVin();
  /* sigbuf[5] = (int32_t)TmrCntGenCh; */
  /* sigbuf[6] = (int32_t)(1.0e3f * RLSQ_Output.Params[0]); */
  /* sigbuf[7] = (int32_t)(1.0e3f * RLSQ_Output.Params[1]); */
  /* sigbuf[8] = (int32_t)(1.0e3f * RLSQ_Output.Params[2]); */
  /* sigbuf[9] = (int32_t)RLSQ_Output.Err; */
  /* sigbuf[10] = (int32_t)RLSQ_Output.SpdEst; */
}

void AppTask_SLog(void* params) {
  TickType_t last_wake_time = xTaskGetTickCount();
  uint32_t sigbuf[SLOG_SIGBUF_SIZE + 1] = {0};
  for(;;) {
    _slog(sigbuf);
    UART_DMAPutBytes((uint8_t*)sigbuf, sizeof(sigbuf));
    vTaskDelayUntil(&last_wake_time, APP_TASK_SLOG_TS);
  }
}
#endif

#ifdef ENBL_MOTOR_IDENT
void motor_ident_run(MtrIf_S* mtr_if ) {
  const int32_t Vin[] = {-3000, 3000};
  const uint32_t TCycleMillis = 500;
  const uint32_t TSampleMillis = 1;
  static uint32_t cnt;
  static uint32_t idx;
  if(cnt++ >= (uint32_t)(TCycleMillis / TSampleMillis)) {
    idx ^= 1;
    MtrIf_SetVin(mtr_if, Vin[idx]);
    cnt = 0;
  }
}
#endif

void AppTask_MotorControl(void* params) {
  TickType_t last_wake_time = xTaskGetTickCount();
  /* MtrIf_Init(&gs_mtr_if); */
  RLSQ_Init();
  for(;;) {
    /* Trig_1Khz(); */
    /* MtrIf_SetSpd(&gs_mtr_if, rtY.MtrSpdFil); */
#ifndef ENBL_MOTOR_IDENT
    /* MtrIf_SetIfbkTgt(&gs_mtr_if, (int32_t)rtY.IfbkPhATgt); */
    /* MtrIf_SetPosTgt(&gs_mtr_if, (int32_t)rtY.MtrPosRef); */
#else
    /* MtrIf_SetIfbkTgt(&gs_mtr_if, 0); */
    /* MtrIf_SetPosTgt(&gs_mtr_if, 0); */
    /* motor_ident_run(&gs_mtr_if); */

    /* Measure execution time. */
    TMR_Reset(TMR_CH_GENERAL);
    TMR_Start(TMR_CH_GENERAL);
    /* RLSQ_Output.SpdEst = RLSQ_Estimate(MtrIf_GetCurrent(), */
    /*     MtrIf_GetVin(), MtrIf_GetSpd(&gs_mtr_if), */
    /*     &RLSQ_Output.Params[0], &RLSQ_Output.Err); */
    /* TmrCntGenCh = TMR_GetCnt(TMR_CH_GENERAL); */
    TMR_Stop(TMR_CH_GENERAL);
    /* End measurement. */

#endif
    vTaskDelayUntil(&last_wake_time, APP_TASK_MOTOR_CONTROL_TS); 
  }
}

/* MtrIf_S* AppTask_GetMtrIf(void) { */
/*   return &gs_mtr_if; */
/* } */
