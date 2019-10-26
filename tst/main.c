#include "unity.h"
#include "app.h"
#include "adc.h"
#include "pwm.h"
#include <string.h> 
#include <stdio.h>

/********************************************************************************
  EXTERN FUNCTIONS & VARS.
********************************************************************************/
extern uint32_t ADC_ReadCh(ADC_Channel_E adc_ch_e);

extern int8_t MockADC_SetBuffer(uint8_t idx, uint32_t val);

extern uint32_t dctocnts(uint32_t dcycle);

extern uint32_t gs_adc_ch_buf[ADC_CH_MAX_E];

extern uint16_t gs_pwm_ch_buf[PWM_PH_MAX]; 
/*******************************************************************************/

void setUp(void) {
  memset(gs_adc_ch_buf, 0, sizeof(uint32_t) * ADC_CH_MAX_E);
  memset(gs_pwm_ch_buf, 0, sizeof(uint16_t) * PWM_PH_MAX); 
}

void tearDown(void) {}

void test_ADC_read_value(void) {
  MockADC_SetBuffer(0, 10);
  MockADC_SetBuffer(ADC_CH_MAX_E - 1, 40);
  TEST_ASSERT_EQUAL_UINT32(10, ADC_ReadCh(0));
  TEST_ASSERT_EQUAL_UINT32(40, ADC_ReadCh(ADC_CH_MAX_E - 1));
}

void test_ADC_read_value_out_of_range_channel(void) {
  MockADC_SetBuffer(0, 10);
  TEST_ASSERT_EQUAL_UINT32(INT32_MAX, ADC_ReadCh(ADC_CH_MAX_E));
}

void test_ADC_get_voltage_zero(void) {
  MockADC_SetBuffer(0, 0);
  TEST_ASSERT_EQUAL_UINT32(0, App_GetVoltage(0));
}

void test_ADC_get_voltage_out_of_range_channel(void) {
  TEST_ASSERT_EQUAL_UINT32(INT32_MAX, App_GetVoltage(VAdcChMax_E));
}

void test_ADC_get_voltage_max(void) {
  MockADC_SetBuffer(0, 4095);
  TEST_ASSERT_EQUAL_UINT32(3300, App_GetVoltage(0));
}

void test_ADC_get_voltage_mid_range(void) {
  MockADC_SetBuffer(0, 2200);
  TEST_ASSERT_EQUAL_UINT32(1772, App_GetVoltage(0));
}

void test_ADC_get_voltage_bias(void) {
  MockADC_SetBuffer(0, 1934);
  TEST_ASSERT_EQUAL_UINT32(APP_PARAMS_IFBK_BIAS, App_GetVoltage(0));
}

void test_ADC_get_current_zero(void) {
  MockADC_SetBuffer(0, 0);
  TEST_ASSERT_EQUAL_INT32(-2337, App_GetCurrent(0));
}

void test_ADC_get_current_out_of_range_channel(void) {
  MockADC_SetBuffer(0, 0);
  TEST_ASSERT_EQUAL_INT32(INT32_MAX, App_GetCurrent(IfbkPhMax_E));
}

void test_ADC_get_current_zero_bias(void) {
  MockADC_SetBuffer(IfbkPhA_E, APP_PARAMS_IFBK_BIAS_ADC_COUNTS);
  TEST_ASSERT_EQUAL_INT32(0, App_GetCurrent(IfbkPhA_E));
}

void test_ADC_get_current_max(void) {
  MockADC_SetBuffer(IfbkPhA_E, 4095);
  TEST_ASSERT_EQUAL_INT32(2613, App_GetCurrent(IfbkPhA_E));
}

void test_ADC_get_current_min(void) {
  MockADC_SetBuffer(IfbkPhA_E, 0);
  TEST_ASSERT_EQUAL_INT32(-2337, App_GetCurrent(IfbkPhA_E));
}

void test_ADC_get_current_channels(void) {
  MockADC_SetBuffer(0, 1000);
  MockADC_SetBuffer(1, 2000);
  MockADC_SetBuffer(2, 3000);
  MockADC_SetBuffer(3, 4000);
  TEST_ASSERT_EQUAL_INT32(-1129, App_GetCurrent(IfbkPhA_E));
  TEST_ASSERT_EQUAL_INT32(79, App_GetCurrent(IfbkPhB_E));
  TEST_ASSERT_EQUAL_INT32(1288, App_GetCurrent(IfbkPhC_E));
  TEST_ASSERT_EQUAL_INT32(INT32_MAX , App_GetCurrent(IfbkPhMax_E));
}

void test_App_SetPwmDutyCycle_value(void) {
  App_SetPwmDutyCycle(0, 10);
  TEST_ASSERT_EQUAL_UINT16(2, gs_pwm_ch_buf[0]);
}

void test_App_SetPwmDutyCycle_out_of_range_channel(void) {
  uint8_t idx;
  App_SetPwmDutyCycle(PwmChMax_E, 10);
  for(idx = 0; idx < 3; idx++) {
    TEST_ASSERT_EQUAL_UINT16(0, gs_pwm_ch_buf[idx]);
  }
}

void test_App_SetPwmDutyCycle_out_of_range_value(void) {
  App_SetPwmDutyCycle(PwmChA_E, 10001);
  TEST_ASSERT_EQUAL_UINT16(PWM_TMR_ARR, gs_pwm_ch_buf[0]);
}

void test_App_SetPwmVoltage_zero_value(void) {
  App_SetPwmVoltage(PwmChC_E, 0);
  TEST_ASSERT_EQUAL_UINT16(0, gs_pwm_ch_buf[2]);
}

void test_App_SetPwmVoltage_mid_range_value(void) {
  App_SetPwmVoltage(PwmChB_E, 5001);
  TEST_ASSERT_EQUAL_UINT16(999, gs_pwm_ch_buf[1]);
}

void test_App_SetPwmVoltage_max_value(void) {
  App_SetPwmVoltage(PwmChC_E, APP_PARAMS_PWM_VOLTS);
  TEST_ASSERT_EQUAL_UINT16(PWM_TMR_ARR, gs_pwm_ch_buf[2]);
}

void test_APP_SetPwmVoltage_out_of_range_value(void) {
  App_SetPwmVoltage(PwmChA_E, 100000000);
  TEST_ASSERT_EQUAL_UINT16(PWM_TMR_ARR, gs_pwm_ch_buf[0]);
}

void test_App_GetPwmDutyCycle_zero(void) {
  TEST_ASSERT_EQUAL_UINT32(0, App_GetPwmDutyCycle(0));
}

void test_App_GetPwmDutyCycle_out_of_range_channel(void) {
  TEST_ASSERT_EQUAL_UINT32(UINT32_MAX, App_GetPwmDutyCycle(PwmChMax_E));
}

void test_App_GetPwmDutyCycle_max_counter(void) {
  gs_pwm_ch_buf[0] = PWM_TMR_ARR;
  TEST_ASSERT_EQUAL_UINT32(10000, App_GetPwmDutyCycle(0));
}

void test_App_GetPwmDutyCycle_mid_range_value(void) {
  gs_pwm_ch_buf[2] = 1399;
  TEST_ASSERT_EQUAL_UINT32(5831, App_GetPwmDutyCycle(PwmChC_E));
}

void test_App_GetPwmVoltage_zero_value(void) {
  TEST_ASSERT_EQUAL_UINT32(0, App_GetPwmVoltage(PwmChA_E));
}

void test_App_GetPwmVoltage_out_of_range_channel(void) {
  TEST_ASSERT_EQUAL_UINT32(UINT32_MAX, App_GetPwmVoltage(PwmChMax_E));
}

void test_App_GetPwmVoltage_mid_range_value(void) {
  gs_pwm_ch_buf[1] = 999; 
  TEST_ASSERT_EQUAL_UINT32(4996, App_GetPwmVoltage(PwmChB_E));
}

void test_App_GetBusVoltage_zero_value(void) {
  MockADC_SetBuffer(ADC_VBUS_CH_E, 0);
  TEST_ASSERT_EQUAL_UINT32(0, App_GetBusVoltage());
}

void test_App_GetBusVoltage_mid_range_value(void) {
  MockADC_SetBuffer(ADC_VBUS_CH_E, 100);
  TEST_ASSERT_EQUAL_UINT32(1532, App_GetBusVoltage());
}

void test_App_GetBusVoltage_max_range_value(void) {
  MockADC_SetBuffer(ADC_VBUS_CH_E, 4095);
  TEST_ASSERT_EQUAL_UINT32(63201, App_GetBusVoltage());
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_ADC_read_value);
  RUN_TEST(test_ADC_read_value_out_of_range_channel);
  RUN_TEST(test_ADC_get_voltage_zero);
  RUN_TEST(test_ADC_get_voltage_out_of_range_channel);
  RUN_TEST(test_ADC_get_voltage_max);
  RUN_TEST(test_ADC_get_voltage_mid_range);
  RUN_TEST(test_ADC_get_voltage_bias);
  RUN_TEST(test_ADC_get_current_zero);
  RUN_TEST(test_ADC_get_current_out_of_range_channel);
  RUN_TEST(test_ADC_get_current_zero_bias);
  RUN_TEST(test_ADC_get_current_max);
  RUN_TEST(test_ADC_get_current_min);
  RUN_TEST(test_ADC_get_current_channels);
  RUN_TEST(test_App_SetPwmDutyCycle_value);
  RUN_TEST(test_App_SetPwmDutyCycle_out_of_range_channel);
  RUN_TEST(test_App_SetPwmDutyCycle_out_of_range_value);
  RUN_TEST(test_App_SetPwmVoltage_zero_value);
  RUN_TEST(test_App_SetPwmVoltage_mid_range_value);
  RUN_TEST(test_App_SetPwmVoltage_max_value);
  RUN_TEST(test_APP_SetPwmVoltage_out_of_range_value);
  RUN_TEST(test_App_GetPwmDutyCycle_zero);
  RUN_TEST(test_App_GetPwmDutyCycle_out_of_range_channel);
  RUN_TEST(test_App_GetPwmDutyCycle_max_counter);
  RUN_TEST(test_App_GetPwmDutyCycle_mid_range_value);
  RUN_TEST(test_App_GetPwmVoltage_zero_value);
  RUN_TEST(test_App_GetPwmVoltage_out_of_range_channel);
  RUN_TEST(test_App_GetPwmVoltage_mid_range_value);
  RUN_TEST(test_App_GetBusVoltage_zero_value);
  RUN_TEST(test_App_GetBusVoltage_mid_range_value);
  return UNITY_END();
}
