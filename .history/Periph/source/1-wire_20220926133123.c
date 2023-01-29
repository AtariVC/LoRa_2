#include "1-wire.h"
#include "uart.h"

void OneWireInit(USART_TypeDef *USARTx){
    UART_init(USARTx, 115200, HALF_DUPLEX);
}
void OneWireReset(USART_TypeDef *USARTx){
    UART_init(USARTx, 9600, HALF_DUPLEX);
}
uint8_t OneWireReadBit(USART_TypeDef *USARTx){

}
uint8_t OneWireReadByte(USART_TypeDef *USARTx){

}
void OneWireWriteBit(USART_TypeDef *USARTx, uint8_t bit){

}
void OneWireWriteByte(USART_TypeDef *USARTx, uint8_t byte){

}