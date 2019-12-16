#include "enc.h"
#include "tmr.h"
#include "stm32f3xx_hal_dma.h"
#include "stm32f3xx_hal_tim.h"
#include "stm32f3xx_hal_tim_ex.h"
#include "stm32f3xx_hal_rcc_ex.h"
#include "stm32f3xx_hal_rcc.h"
#include "stm32f3xx_ll_tim.h"
#include "stm32f3xx_hal_cortex.h"
#include "dbg.h"

static TIM_HandleTypeDef gs_tim_enc_conf = TIM_ENC_INIT_CONF;

typedef struct ENC_Handle {
  int32_t counts;
  int8_t dir;
} ENC_Handle_S;

static ENC_Handle_S gs_enc_handle;

static void ENC_ErrorHandler(char* errmsg) {
  DBG_ERR("ENC: %s", errmsg);
  while(1);
}

void ENC_Init(void) {
  DBG_DEBUG("Initializing encoder interface...\n\r");
  TIM_Encoder_InitTypeDef s_enc_conf = ENC_INIT_CONF;
  if(HAL_TIM_Encoder_Init(&gs_tim_enc_conf, &s_enc_conf) != HAL_OK) {
    ENC_ErrorHandler("Error initializing encoder interface!\n\r");
  }
  gs_enc_handle.dir = ENC_DIR_UNKNOWN;
  gs_enc_handle.counts = 0;
  ENC_Start();
  DBG_DEBUG("Done.\n\r");
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* s_enc_conf) {
  UNUSED(s_enc_conf);
  GPIO_InitTypeDef s_enc_ch_a_gpio_conf = ENC_CH_A_GPIO_CONF;
  GPIO_InitTypeDef s_enc_ch_b_gpio_conf = ENC_CH_B_GPIO_CONF;
  GPIO_InitTypeDef s_enc_ch_idx_gpio_conf = ENC_CH_IDX_GPIO_CONF;
 
  if(!__HAL_RCC_TIM2_IS_CLK_ENABLED()) {
    __HAL_RCC_TIM2_CLK_ENABLE(); 
  } 
  if(!__HAL_RCC_GPIOA_IS_CLK_ENABLED()) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  }
  if(!__HAL_RCC_GPIOB_IS_CLK_ENABLED()) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  }
  HAL_GPIO_Init(ENC_CH_A_PORT, &s_enc_ch_a_gpio_conf);
  HAL_GPIO_Init(ENC_CH_B_PORT, &s_enc_ch_b_gpio_conf);
#ifdef CONF_ENC_CH_IDX_ENBL
  HAL_GPIO_Init(ENC_CH_IDX_PORT, &s_enc_ch_idx_gpio_conf);
  HAL_NVIC_SetPriority(ENC_CH_IDX_IT_IRQn, ENC_CH_IDX_IT_PRIO, ENC_CH_IDX_IT_SUBPRIO);
  HAL_NVIC_EnableIRQ(ENC_CH_IDX_IT_IRQn);
#endif
}

void ENC_Start(void) {
  HAL_TIM_Encoder_Start(&gs_tim_enc_conf, ENC_CH_ALL);
}

int32_t ENC_GetCnt(void) {
  int32_t ret;
  int16_t counts_reg = (int16_t)__HAL_TIM_GET_COUNTER(&gs_tim_enc_conf);
  TMR_ENC_LOCK();
  ret = (int32_t)counts_reg + gs_enc_handle.counts;
  TMR_ENC_UNLOCK();
  return ret;
}

void ENC_SetCnt(int16_t cnt) {
  __HAL_TIM_SET_COUNTER(&gs_tim_enc_conf, (uint16_t)cnt);
}

void ENC_DeInit(void) {

}

#ifdef CONF_ENC_CH_IDX_ENBL
void EXTI15_10_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(ENC_CH_IDX_PIN);
}

void ENC_ChIdxCallback(void) {
  DBG_DEBUG("Index callback!\n\r");  
}
#endif

void ENC_TmrCallback(void) {
/* Function to handle encoder unwrapping when encoder
   does not have index channel. Called by TMR module @ 1ms. */
  int16_t counts = (int16_t)ENC_GetCnt();   
  if(counts > gs_enc_handle.counts) {
    gs_enc_handle.dir = ENC_DIR_FWD;
  } else if (counts < gs_enc_handle.counts) {
    gs_enc_handle.dir = ENC_DIR_REV;
  } else {
    gs_enc_handle.dir = ENC_DIR_UNKNOWN;
  }
  if((counts >= ENC_COUNTS_THRESHOLD) || (counts <= -ENC_COUNTS_THRESHOLD)) {
    gs_enc_handle.counts += counts;
    ENC_SetCnt(0);
  }
}
