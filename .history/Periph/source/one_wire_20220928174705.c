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
    return OneWireSendBit(ow, OneWire_read_bit) == OneWire_bit_1 ? 1 : 0;
}

OneWireStatus OneWireReset(OneWire *ow){
    UART_init(ow->uart, 9600, HALF_DUPLEX);
    uint8_t answer = OneWireSendBit(ow, OneWire_reset_cmd);
    UART_init(ow->uart, 115200, HALF_DUPLEX);
    if(answer == OneWire_reset_cmd) return ONE_WIRE_EMPTY_BUS;
    return ONE_WIRE_OK;
}

uint8_t OneWireRead(OneWire *ow){
    uint8_t buffer = 0;
    for(uint8_t i = 0; i < 8; i++){
        buffer |= OneWireReadBit(ow) << i;
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
        uint8_t next_bit = ((byte >> i) & 0x01) == 0x01 ? OneWire_bit_1 : OneWire_bit_0;
        buffer |= (OneWireSendBit(ow, next_bit) == OneWire_bit_1 ? 1 : 0) << i;
    }
    return buffer;
}

void OneWireWriteArray(OneWire *ow, uint8_t *array, uint8_t length){
    for(uint8_t i = 0; i < length; i++){
        OneWireWrite(ow, array[i]);
    }
}


/*
 * return 1 if has got one more address
 * return 0 if hasn't
 * return -1 if error reading happened
 */
int8_t hasNextRom(OneWire *ow, uint8_t *ROM) {
    if (OneWireReset(ow) == ONE_WIRE_EMPTY_BUS) return 0;
    OneWireWrite(ow, OneWire_Search_cmd);

    uint8_t rom_bit_counter = 0;
    int8_t zeroFork = -1;
    while(rom_bit_counter < OneWire_ROM_ID_lenth){
        uint8_t byteNum = rom_bit_counter >> 3;
        uint8_t *current_byte_in_rom = (ROM) + byteNum;
        uint8_t cB, cmp_cB, searchDirection = 0;
        cB = OneWireReadBit(ow);  // чтение прямого бита
        cmp_cB = OneWireReadBit(ow);  // чтение комплементарного бита
        if (cB == cmp_cB && cB == 1) return -1;  // empty bus or algorythm issue
        if (cB != cmp_cB) searchDirection = cB;
        if (cB == cmp_cB && cB == 0) {  // коллизия
            if (rom_bit_counter == ow->last_diff_bit_position)  // идем по правой ветке
                searchDirection = 1;
            else if (rom_bit_counter > ow->last_diff_bit_position)
                searchDirection = 0;
            else  // (rom_bit_counter < ow->last_diff_bit_position) // не дошли до развилки
                searchDirection = (uint8_t) ((ow->lastROM[byteNum] >> rom_bit_counter % 8) & 0x01);
            if (searchDirection == 0)
                zeroFork = rom_bit_counter;
        }
        // сохраняем бит
        if (searchDirection)
            *(current_byte_in_rom) |= 1 << rom_bit_counter % 8;
        uint8_t answerBit = (searchDirection == 0) ? OneWire_bit_0 : OneWire_bit_1;
        OneWireSendBit(ow, answerBit);
        rom_bit_counter++;
    }
    ow->last_diff_bit_position = zeroFork;
    for (uint8_t i = 0; i < 7; i++)
        ow->lastROM[i] = ROM[i];
    return ow->last_diff_bit_position > 0;
}