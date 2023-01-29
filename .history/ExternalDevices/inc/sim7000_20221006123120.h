#include "stm32l4xx.h"
#include "uart.h"

typedef struct SIM7000_GPIO {
    uint8_t rx;
    uint8_t tx;
    uint8_t rts;
    uint8_t cts;
    uint8_t ri;
    uint8_t dtr;
    uint8_t dcd;
} SIM7000_GPIO;

typedef struct GSM {
    SIM7000_GPIO gpio;

    USART_TypeDef *uart;

    uint8_t sim_status;
    uint8_t gsm_status;
    uint8_t gprs_status;
    uint16_t vbat;
    char *sim7000_ip[15];
    char *server_ip[15];
    char *APN[20];
    char *gprs_user[10];
    char *gpras_pass[10];

} GSM;

void SIM7000_Init(GSM *driver);
void SIM7000_SendSMS(GSM *driver, const char *data);
void SIM7000_SendTCP(GSM *driver, const char *data);


