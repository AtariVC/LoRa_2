/*
 * adc.h
 *
 *  Created on: 18 ���. 2020 �.
 *      Author: Gandalf
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"

void ADC_Init();
void ADC_Init_regular(uint8_t ch_num);
void adc_single_conversion( ADC_TypeDef* ADCx, uint16_t* adc_data_arr ) ;

#endif /* INC_ADC_H_ */
