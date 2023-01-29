#include "ds18b20.h"
#include "delay.h"

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

uint8_t DS18B20_StartTempMeas(OneWire *ow){
    OneWire_Reset(ow);
    OneWire_Write(ow, DS18B20_SKIP_ROM);
    OneWire_Write(ow, DS18B20_CONVERT_TEMP);
    uint8_t answer = 0;
    uint8_t timeout_ms_counter = 0;
    // while(answer == 0 && timeout_ms_counter < DS18B20_CONVERTION_TIMEOUT_MS){
    //     answer = OneWire_SendBit(ow, 0);
    //     timeout_ms_counter = GetMili();
    // }

    // if(answer > 0) return 0;
    Delay(300);
    return 0;
    // return 1;
}

uint8_t DS18B20_ReadScratchpad(OneWire *ow, RomCode *rom, uint8_t lsb, uint8_t msb){
    OneWireStatus status = OneWire_MatchRom(ow, rom);
    if(status == ONE_WIRE_OK){
        OneWire_Write(ow, DS18B20_READ_SCRATCHPAD);
        lsb = OneWire_Read(ow);
        msb = OneWire_Read(ow);
    }
    return status;
}