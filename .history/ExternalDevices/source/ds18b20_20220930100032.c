#include "ds18b20.h"
#include "delay.h"



uint8_t DS18B20_StartTempMeas(OneWire *ow){
    OneWire_Reset(ow);
    OneWire_Write(ow, DS18B20_SKIP_ROM);
    // OneWire_MatchRom(ow, rom);
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

uint8_t DS18B20_ReadScratchpad(OneWire *ow, RomCode *rom, uint8_t *buf){
    OneWireStatus status = OneWire_MatchRom(ow, rom);
    if(status == ONE_WIRE_OK){
        OneWire_Write(ow, DS18B20_READ_SCRATCHPAD);
        OneWire_ReadArray(ow, buf, 8);
    }
    return status;
}

