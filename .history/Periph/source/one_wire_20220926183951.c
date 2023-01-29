#include "one_wire.h"
#include "uart.h"


uint8_t OneWireSendBit(USART_TypeDef *USARTx, uint8_t data){
	while(!(USARTx->ISR & USART_ISR_TC));
	USARTx->TDR = data;
	while(!(USARTx->ISR & USART_ISR_TC));
	while(!(USARTx->ISR & USART_ISR_RXNE));
	uint8_t rx_data = USARTx->RDR;
	return rx_data;
}

OneWireStatus OneWireReset(USART_TypeDef *USARTx){
    UART_init(USARTx, 9600, HALF_DUPLEX);
    uint8_t answer = OneWireSendBit(USARTx, OneWire_reset_cmd);
    UART_init(USARTx, 115200, HALF_DUPLEX);
    if(answer != OneWire_reset_cmd) return ERROR;
    return OK;
}

uint8_t OneWireRead(USART_TypeDef *USARTx){
    uint8_t buffer = 0;
    for(uint8_t i = 0; i < 8; i++){
        buffer |= (OneWireSendBit(USARTx, OneWire_read_bit) == OneWire_bit_1 ? 1 : 0) << i;
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
        // Для каждого отправляемого бита, равного 1 отправляем 0xFF.
        // Полученное значение интерпретируем также: получили 0xFF - прочитали бит, равный 1.
        buffer |= (OneWireSendBit(USARTx, ((byte >> i) & 0x01) == 0x01 ? OneWire_bit_1 : OneWire_bit_0) == OneWire_bit_1 ? 1 : 0) << i;
    }
    return buffer;
}

void OneWireWriteArray(USART_TypeDef *USARTx, uint8_t *array, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        OneWireWrite(USARTx, array[i]);
    }
}