#ifndef INC_1_WIRE_H_
#define INC_1_WIRE_H_

#include "stm32l4xx.h"

#define OneWire_bit_1     0xFF
#define OneWire_bit_0     0x00
#define OneWire_read_bit  0xFF
#define OneWire_reset_cmd 0xF0

typedef enum
{
  OK = 0,
  ResetError = !OK
} OneWireStatus;

OneWireStatus OneWireReset(USART_TypeDef *USARTx);
uint8_t OneWireRead(USART_TypeDef *USARTx);
void OneWireWrite(USART_TypeDef *USARTx, uint8_t byte);
void OneWireReadArray(USART_TypeDef *USARTx, uint8_t *array, uint8_t length);
void OneWireWriteArray(USART_TypeDef *USARTx, uint8_t *array, uint8_t length);
#endif /* INC_1_WIRE_H_ */
