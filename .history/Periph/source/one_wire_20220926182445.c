#include "one_wire.h"
#include "uart.h"

OneWireStatus OneWireReset(USART_TypeDef *USARTx){
    UART_init(USARTx, 9600, HALF_DUPLEX);
    uint8_t answer = UART_hd_txrx(USARTx, OneWire_reset_cmd);
    UART_init(USARTx, 115200, HALF_DUPLEX);
    if(answer != OneWire_reset_cmd) return ERROR;
    return OK;
}

uint8_t OneWireRead(USART_TypeDef *USARTx){
    uint8_t buffer = 0;
    for(uint8_t i = 0; i < 8; i++){
        buffer |= (UART_hd_txrx(USARTx, OneWire_read_bit) == OneWire_bit_1 ? 1 : 0) << i;
    }
    return buffer;
}

void OneWireReadArray(USART_TypeDef *USARTx, uint8_t *array, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        array[i] = OneWireRead(USARTx);
    }
}

uint8_t OneWireWrite(USART_TypeDef *USARTx, uint8_t byte){
    uint8_t buffer = 0;
    for(uint8_t i = 0; i < 8; i++){
        buffer |= UART_hd_txrx(USARTx, ((byte >> i) & 0x01) == 1 ? OneWire_bit_1 : OneWire_bit_0);
    }
    return buffer;
}

void OneWireWriteArray(USART_TypeDef *USARTx, uint8_t *array, uint8_t length){
    uint8_t buf[5] = {0};
    for(uint8_t i = 0; i < length; i++){
        buf[i] = OneWireWrite(USARTx, array[i]);
    }
    return;
}