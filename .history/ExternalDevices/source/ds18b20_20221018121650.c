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

uint8_t DS18B20_ReadScratchpad(DS18B20 *sensor){
    OneWireStatus status = OneWire_MatchRom(sensor->ow, sensor->serialNumber);
    if(status == ONE_WIRE_OK){
        OneWire_Write(sensor->ow, DS18B20_READ_SCRATCHPAD);
        OneWire_ReadArray(sensor->ow, (uint8_t *)(&(sensor->scratchpad)), 9);
        sensor->temperature = (uint32_t)(sensor->scratchpad.temperature) * 0.0625;
    }
    return status;
}

void DS18B20_Init(DS18B20 *sensor, OneWire *ow){
    int8_t devices_on_bus = OneWire_SearchDevices(ow);
    for(uint8_t i = 0; i < MAXDEVICES_ON_THE_BUS; i++){
        if(ow->ids[i].family != 0){
            sensor[i].ow = ow;
            sensor[i].isConnected = 1;
            sensor[i].serialNumber = &ow->ids[i];  // копируем адрес первого байта ROM
        }
        else break;
    }
}

void DS18B20_ReadROM(DS18B20 *sensor){
    if(OneWire_Reset(sensor->ow) == ONE_WIRE_EMPTY_BUS) return;
    OneWire_Write(sensor->ow, DS18B20_READ_ROM);
    OneWire_ReadArray(sensor->ow, (uint8_t *)(&sensor->serialNumber), 8);
}

uint16_t DS18B20_ReadTemperature(DS18B20 *sensor){
    OneWireStatus status = OneWire_MatchRom(sensor->ow, sensor->serialNumber);
    if(status == ONE_WIRE_OK){
        OneWire_Write(sensor->ow, DS18B20_READ_SCRATCHPAD);
        OneWire_ReadArray(sensor->ow, (uint8_t *)(&(sensor->scratchpad)), 2);
        sensor->temperature = (float)(sensor->scratchpad.temperature >> 4);
    }
    return (uint16_t)(sensor->scratchpad.temperature);
}