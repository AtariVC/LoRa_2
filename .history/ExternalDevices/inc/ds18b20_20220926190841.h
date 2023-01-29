#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include "stm32l4xx.h"
#define DS18B20_SERIAL_NUMBER_LEN_BYTES   8
typedef struct DS18B20
{
  uint8_t isInitialized;
  uint8_t isConnected;
  USART_TypeDef *uart;
  uint8_t serialNumber[DS18B20_SERIAL_NUMBER_LEN_BYTES];
  uint8_t temperatureLimitLow;
  uint8_t temperatureLimitHigh;
  uint8_t configRegister;
  float temperature;
} DS18B20;

#endif  //INC_DS18B20_H_