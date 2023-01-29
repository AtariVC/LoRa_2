/*
 * tim.h
 *
 *  Created on: 18 ���. 2020 �.
 *      Author: Gandalf
 */

#ifndef INC_TIM_H_
#define INC_TIM_H_

//#define TIM_LOOP_MODE
void TIM_init(TIM_TypeDef *TIMx, uint32_t delay, uint8_t loop_mode);

#endif /* INC_TIM_H_ */
