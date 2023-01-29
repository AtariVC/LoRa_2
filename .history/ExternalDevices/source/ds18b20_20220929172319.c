#include "ds18b20.h"


static uint8_t CalculateChecksum(uint8_t *data, uint8_t length){
    uint8_t checksum = 0;
    while (length--){
        uint8_t currentByte = *data++;
        for (uint8_t i = 8; i; i--){
            uint8_t temp = (checksum ^ currentByte) & 0x01;
            checksum >>= 1;
            if (temp) checksum ^= 0x8C;
            currentByte >>= 1;
        }
    }
    return checksum;
}

void DS18B20_StartTempMeas(OneWire *ow){
    OneWire_Reset(ow);
    OneWire_Write(ow, DS18B20_CONVERT_TEMP);
    uint8_t answer = 0;
    while(!answer)
        answer = OneWire_ReadBit(ow);
}