#include "stm32l4xx.h"
#include "uart.h"

typedef struct GSM
{
    uint8_t rx;
    uint8_t tx;
    uint8_t rts;
    uint8_t cts;
    uint8_t ri;
    uint8_t dtr;
    uint8_t dcd;

    USART_TypeDef *uart;


    uint8_t gsm_status;
    uint8_t gprs_status;
    uint16_t vbat;
    char *ip[15];
    char *APN[20];

} GSM;

