/*
 * uart.h
 *
 *  Created on: 3 ����. 2020 �.
 *      Author: BilboBaggins
 */

#ifndef CODE_HEADER_UART_H_
#define CODE_HEADER_UART_H_

#include "stm32l4xx.h"

uint8_t uart_receive_data;

void UART_init(USART_TypeDef *USARTx, int baudrate);
void UART_tx(USART_TypeDef *USARTx, uint8_t data);
void UART_tx_array(USART_TypeDef *USARTx, char *array);

#endif /* CODE_HEADER_UART_H_ */
