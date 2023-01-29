#ifndef INC_1_WIRE_H_
#define INC_1_WIRE_H_

#include "stm32l4xx.h"

#define OneWire_bit_1          0xFF
#define OneWire_bit_0          0x00
#define OneWire_read_bit       0xFF
#define OneWire_reset_cmd      0xF0

#define MAXDEVICES_ON_THE_BUS  20
#define OneWire_ROM_ID_lenth   64
typedef enum
{
  ONE_WIRE_OK = 0,
  ONE_WIRE_EMPTY_BUS = !ONE_WIRE_OK
} OneWireStatus;

typedef struct {
    uint8_t family;
    uint8_t code[6];
    uint8_t crc;
} RomCode;

typedef struct OneWire{
    USART_TypeDef *uart;
    RomCode ids[MAXDEVICES_ON_THE_BUS];
    int last_diff_bit_position;
    uint8_t lastROM[8];  // 64 bit ROM
} OneWire;

OneWireStatus OneWireReset(OneWire *ow);
uint8_t OneWireRead(OneWire *ow);
uint8_t OneWireWrite(OneWire *ow, uint8_t byte);
void OneWireReadArray(OneWire *ow, uint8_t *array, uint8_t length);
void OneWireWriteArray(OneWire *ow, uint8_t *array, uint8_t length);
int OneWireSearchCmd(OneWire *ow);
#endif /* INC_1_WIRE_H_ */
