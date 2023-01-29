/*
 * global_variables.h
 *
 *  Created on: 18 ���. 2020 �.
 *      Author: Gandalf
 */

#ifndef INC_GLOBAL_VARIABLES_H_
#define INC_GLOBAL_VARIABLES_H_

#include "stm32l4xx.h"
#include "rgb_led.h"
#include "fifo.h"
#include "sx1278.h"
#include "rtc.h"
#include "BME280.h"
//#include "main.h"

// --------- System condition -----------//
ErrorStatus SYSTEM_init_status;
ErrorStatus SYSTEM_I2C_error_flag;
uint32_t SYSTEM_I2C_error_counter;


FIFO(64) fifo;
FIFO(64) long_cmd_fifo;
FIFO(64) sensors_data_rx_fifo;
uint8_t FIFO_flag;

// --------- ADC -----------//
volatile float battary_voltage;
//===========================//

// --------- TIMERS -----------//
uint8_t TIM2_finished; // Global flag that indicates status of timer. 0 - still counting; 1 - finished
uint8_t TIM7_finished; // Global flag that indicates status of timer. 0 - still counting; 1 - finished
//===========================//
// RTC_struct rtc;

void init_global_variables();

#endif /* INC_GLOBAL_VARIABLES_H_ */
