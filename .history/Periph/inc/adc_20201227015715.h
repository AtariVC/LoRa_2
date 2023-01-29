/*
 * adc.h
 *
 *  Created on: 18 окт. 2020 г.
 *      Author: Gandalf
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"

void ADC_Init();
void ADC_Init_regular();
uint16_t adc_single_conversion( ADC_TypeDef* ADCx );

#endif /* INC_ADC_H_ */
