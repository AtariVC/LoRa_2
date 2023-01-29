#include "stm32l4xx.h"
#include "uart.h"
#include "gpio.h"
#include "fifo.h"

typedef struct SIM7000_GPIO {
    GPIO_Pin rx;
    GPIO_Pin tx;
    GPIO_Pin rts;
    GPIO_Pin cts;
    GPIO_Pin ri;
    /* After setting the AT command “AT+CSCLK=1”, and then pulling up the DTR pin, SIM7000 will
    enter sleep mode when module is in idle mode. In sleep mode, the UART is unavailable. When
    SIM7000 enters sleep mode, pulling down DTR can wake up module.
    After setting the AT command “AT+CSCLK=0”, SIM7000 will do nothing when the DTR pin is
    pulling up. */
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
    char operator[10];
    uint8_t sim_status;
    uint8_t gsm_status;
    uint8_t gprs_status;
    uint16_t vbat;
    char ip[15];  // SIM7000 IP
    char APN[20];
    char gprs_user[10];
    char gpras_pass[10];
    char last_cmd[128];
    uint8_t waiting_answer_flag;

    FIFO(128) rx_fifo;
} GSM;

void SIM7000_Init(GSM *driver);
void SIM7000_SetPowerState(GSM *driver, GPIO_State power_state);
void SIM7000_SendSMS(GSM *driver, const char *data, const char *phone_num);
void SIM7000_SendTCP(GSM *driver, const char *data, const char *ip, const char *port);


