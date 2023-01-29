#ifndef INC_1_WIRE_H_
#define INC_1_WIRE_H_

#include "stm32l4xx.h"

#define OneWire_bit_1     0xFF
#define OneWire_bit_0     0x00
#define OneWire_read_bit  0xFF
#define OneWire_reset_cmd 0xF0

#define MAXDEVICES_ON_THE_BUS 20

typedef enum
{
  OK = 0,
  ResetError = !OK
} OneWireStatus;

typedef struct {
    uint8_t family;
    uint8_t code[6];
    uint8_t crc;
} RomCode;

typedef struct {
    USART_TypeDef uart;
    RomCode ids[MAXDEVICES_ON_THE_BUS];
    int lastDiscrepancy;
    uint8_t lastROM[8];
} OneWire;

OneWireStatus OneWireReset(USART_TypeDef *USARTx);
uint8_t OneWireRead(USART_TypeDef *USARTx);
uint8_t OneWireWrite(USART_TypeDef *USARTx, uint8_t byte);
void OneWireReadArray(USART_TypeDef *USARTx, uint8_t *array, uint8_t length);
void OneWireWriteArray(USART_TypeDef *USARTx, uint8_t *array, uint8_t length);
#endif /* INC_1_WIRE_H_ */
