/*
 * adc.h
 *
 *  Created on: 18 ���. 2020 �.
 *      Author: Gandalf
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"
#include "gpio.h"
#include "stdbool.h"

// typedef struct ADC_channels{
//     GPIO_Pin AI1;
//     GPIO_Pin AI2;
//     GPIO_Pin AI3;
//     GPIO_Pin AI4;
//     GPIO_Pin AI5;
//     GPIO_Pin AI6;
//     GPIO_Pin AI7;
//     GPIO_Pin AI8;
//     GPIO_Pin AI9;
//     GPIO_Pin AI10;
//     GPIO_Pin AI11;
//     GPIO_Pin AI12;
//     GPIO_Pin AI13;
//     GPIO_Pin AI14;
//     GPIO_Pin AI15;
// } ADC_channels;

typedef struct ADC{
    GPIO_Pin ch[16];
    ADC_TypeDef *ADC;
    uint8_t ovrsmpl_ratio;
    bool en_;
} ADC;

// void ADC_InitGPIO(*ADC);
void ADC_Init();
void ADC_Init_regular(uint8_t ch_num);
void adc_single_conversion( ADC_TypeDef* ADCx, uint16_t* adc_data_arr ) ;

#endif /* INC_ADC_H_ */
