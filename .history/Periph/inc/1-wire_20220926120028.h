#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"
#include "uart.h"


void OneWireInit(USART_TypeDef *USARTx);
void ADC_Init_regular();
uint16_t adc_single_conversion( ADC_TypeDef* ADCx );

#endif /* INC_ADC_H_ */
