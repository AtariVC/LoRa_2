#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"

#define OneWire_bit_1   0xFF
#define OneWire_bit_0   0x00

typedef enum
{
  OK = 0,
  ERROR = !OK
} OneWireStatus;

void OneWireInit(USART_TypeDef *USARTx);
OneWireStatus OneWireReset(USART_TypeDef *USARTx);
uint8_t OneWireReadBit(USART_TypeDef *USARTx);
uint8_t OneWireReadByte(USART_TypeDef *USARTx);
void OneWireWriteBit(USART_TypeDef *USARTx, uint8_t bit);
void OneWireWriteByte(USART_TypeDef *USARTx, uint8_t byte);

#endif /* INC_ADC_H_ */
