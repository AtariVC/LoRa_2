#include "ds18b20.h"

void DS18B20_Init(DS18B20 *sensor, USART_TypeDef *USARTx){
  sensor->isConnected = 0;
  sensor->uart = USARTx;
  sensor->isInitialized = 1;
}

static uint8_t CalculateChecksum(uint8_t *data, uint8_t length){
  uint8_t checksum = 0;
  while (length--)
  {
    uint8_t currentByte = *data++;
    for (uint8_t i = 8; i; i--)
    {
      uint8_t temp = (checksum ^ currentByte) & 0x01;
      checksum >>= 1;
      if (temp)
      {
        checksum ^= 0x8C;
      }
      currentByte >>= 1;
    }
  }
  return checksum;
}