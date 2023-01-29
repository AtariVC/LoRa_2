#include "1-wire.h"
#include "uart.h"

void OneWireInit(USART_TypeDef *USARTx){
    UART_init(USARTx, 115200, HALF_DUPLEX);
}
OneWireStatus OneWireReset(USART_TypeDef *USARTx){
    UART_init(USARTx, 9600, HALF_DUPLEX);
    uint8_t answer = UART_hd_txrx(USARTx, 0xF0);
    UART_init(USARTx, 115200, HALF_DUPLEX);
    if(answer != 0xF0) return ERROR;
    return OK;
}
uint8_t OneWireReadBit(USART_TypeDef *USARTx){
    UART_tx(USARTx, 0xFF);
}
uint8_t OneWireReadByte(USART_TypeDef *USARTx){

}
void OneWireWriteBit(USART_TypeDef *USARTx, uint8_t bit){
    UART_tx(USARTx, bit);
}
void OneWireWriteByte(USART_TypeDef *USARTx, uint8_t byte){
    for(uint8_t i=0; i < 8; i++){
        uint8_t bit = ((0x01 << i) & byte) == 0x01 ? 0xFF : 0x00
        UART_tx(USARTx, bit);
    }
}