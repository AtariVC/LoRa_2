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
uint8_t OneWireRead(USART_TypeDef *USARTx){
    uint8_t buffer = 0;
    for(uint8_t i = 0; i < 8; i++){
        buffer |= (UART_hd_txrx(USARTx, 0xFF) == 0xFF ? 1 : 0) << i;
    }
}
void OneWireWrite(USART_TypeDef *USARTx, uint8_t byte){
    for(uint8_t i=0; i < 8; i++){
        UART_tx(USARTx, ((0x01 << i) & byte) == 0x01 ? 0xFF : 0x00);
    }
}