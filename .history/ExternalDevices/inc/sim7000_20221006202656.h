#include "stm32l4xx.h"
#include "uart.h"
#include "gpio.h"


typedef struct SIM7000_GPIO {
    GPIO_Pin rx;
    GPIO_Pin tx;
    GPIO_Pin rts;
    GPIO_Pin cts;
    GPIO_Pin ri;
    GPIO_Pin dtr;
    GPIO_Pin dcd;
    GPIO_Pin pwr;
    GPIO_Pin nreset;

    GPIO_Mode __tx_af_pin;
    GPIO_Mode __rx_af_pin;
    GPIO_Mode __cts_af_pin;
    GPIO_Mode __rts_af_pin;

} SIM7000_GPIO;

typedef struct GSM {
    SIM7000_GPIO gpio;

    USART_TypeDef *uart;
    GPIO_State power_state;
    uint8_t sim_status;
    uint8_t gsm_status;
    uint8_t gprs_status;
    uint16_t vbat;
    char *ip[15];  // SIM7000 IP
    char *APN[20];
    char *gprs_user[10];
    char *gpras_pass[10];
} GSM;

void SIM7000_Init(GSM *driver);
void SIM7000_SetPowerState(GSM *driver, GPIO_State power_state);
void SIM7000_SendSMS(GSM *driver, const char *data, const char *phone_num);
void SIM7000_SendTCP(GSM *driver, const char *data, const char *ip, const char *port);


