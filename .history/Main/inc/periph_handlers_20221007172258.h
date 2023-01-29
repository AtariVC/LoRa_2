/*
 * periph_handlers.h
 *
 *  Created on: 18 ���. 2020 �.
 *      Author: Gandalf
 */

#ifndef INC_PERIPH_HANDLERS_H_
#define INC_PERIPH_HANDLERS_H_
#include "main.h"

// void EXTI4_IRQHandler();
// void USART1_IRQHandler(void);
void USART3_IRQHandler(void);
// void TIM7_IRQHandler(void);
void RTC_Alarm_IRQHandler(void);
#endif /* INC_PERIPH_HANDLERS_H_ */
