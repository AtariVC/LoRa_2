#include "one_wire.h"
#include "uart.h"


uint8_t OneWireSendBit(OneWire *ow, uint8_t data){
	while(!(ow->uart->ISR & USART_ISR_TC));
	ow->uart->TDR = data;
	while(!(ow->uart->ISR & USART_ISR_TC));
	while(!(ow->uart->ISR & USART_ISR_RXNE));
	uint8_t rx_data = ow->uart->RDR;
	return rx_data;
}

uint8_t OneWireReadBit(OneWire *ow){
    return (OneWireSendBit(ow, OneWire_read_bit) == OneWire_bit_1 ? 1 : 0;
}

OneWireStatus OneWireReset(OneWire *ow){
    UART_init(ow->uart, 9600, HALF_DUPLEX);
    uint8_t answer = OneWireSendBit(ow, OneWire_reset_cmd);
    UART_init(ow->uart, 115200, HALF_DUPLEX);
    if(answer == OneWire_reset_cmd) return ERROR;
    return OK;
}

uint8_t OneWireRead(OneWire *ow){
    uint8_t buffer = 0;
    for(uint8_t i = 0; i < 8; i++){
        buffer |= (OneWireSendBit(ow, OneWire_read_bit) == OneWire_bit_1 ? 1 : 0) << i;
    }
    return buffer;
}

void OneWireReadArray(OneWire *ow, uint8_t *array, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        array[i] = OneWireRead(ow);
    }
}

uint8_t OneWireWrite(OneWire *ow, uint8_t byte){
    uint8_t buffer = 0;
    for(uint8_t i = 0; i < 8; i++){
        // Для каждого отправляемого бита, равного 1 отправляем 0xFF.
        // Полученное значение интерпретируем также: получили 0xFF - прочитали бит, равный 1.
        buffer |= (OneWireSendBit(ow, ((byte >> i) & 0x01) == 0x01 ? OneWire_bit_1 : OneWire_bit_0) == OneWire_bit_1 ? 1 : 0) << i;
    }
    return buffer;
}

void OneWireWriteArray(OneWire *ow, uint8_t *array, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        OneWireWrite(ow, array[i]);
    }
}

int hasNextRom(OneWire *ow, uint8_t *ROM) {
    if (owResetCmd(ow) == ONEWIRE_NOBODY) {
        return 0;
    }

    owSendByte(ow, ONEWIRE_SEARCH);
    uint8_t ui32BitNumber = 0;
    int zeroFork = -1;
    do {
        int byteNum = ui32BitNumber / 8;
        uint8_t *current = (ROM) + byteNum;
        uint8_t cB, cmp_cB, searchDirection = 0;
        cB = OneWireSendBit == OneWire_bit_1 ? 1 : 0; // чтение прямого бита

        owSend(ow, OW_READ); // чтение инверсного бита
        cmp_cB = owReadSlot(owEchoRead(ow));
        if (cB == cmp_cB && cB == 1)
            return -1;
        if (cB != cmp_cB) {
            searchDirection = cB;
        } else {
            if (ui32BitNumber == ow->lastDiscrepancy)
                searchDirection = 1;
            else {
                if (ui32BitNumber > ow->lastDiscrepancy) {
                    searchDirection = 0;
                } else {
                    searchDirection = (uint8_t) ((ow->lastROM[byteNum] >> ui32BitNumber % 8) & 0x01);
                }
                if (searchDirection == 0)
                    zeroFork = ui32BitNumber;
            }
        }
        // сохраняем бит
        if (searchDirection)
            *(current) |= 1 << ui32BitNumber % 8;
        uint8_t answerBit = (uint8_t) ((searchDirection == 0) ? OneWire_bit_0 : OneWire_bit_1);
        owSend(ow, answerBit);
        ui32BitNumber++;
    } while (ui32BitNumber < 64);
    ow->lastDiscrepancy = zeroFork;
    uint8_t i = 0;
    for (; i < 7; i++)
        ow->lastROM[i] = ROM[i];
    return ow->lastDiscrepancy > 0;
}