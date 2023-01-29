#include "stm32l4xx.h"
#include "uart.h"
#include "gpio.h"

typedef struct GSM_GPIO {
    GPIO_Pin rx;  // SIM_UART_RX  <-> MCU_UART_TX
    GPIO_Pin tx;  // SIM_UART_TX  <-> MCU_UART_RX
    GPIO_Pin rts;  // Request to send (Input)
    GPIO_Pin cts;  // Clear to Send (Output)
    /* The RI pin can be used to interrupt output signal to inform the host controller such as application
    CPU. Before that, users must use AT command “AT+CFGRI=1” to enable this function.
    Normally RI will keep high level until certain conditions such as receiving SMS, or a URC report
    coming, then it will output a low level pulse 120ms, in the end, it will become high level  (Output)*/
    GPIO_Pin ri;
    /* After setting the AT command “AT+CSCLK=1”, and then pulling up the DTR pin, SIM7000 will
    enter sleep mode when module is in idle mode. In sleep mode, the UART is unavailable. When
    SIM7000 enters sleep mode, pulling down DTR can wake up module.
    After setting the AT command “AT+CSCLK=0”, SIM7000 will do nothing when the DTR pin is
    pulling up. (Input)*/
    GPIO_Pin dtr;
    GPIO_Pin dcd;  // Carrier detects (Output)
    GPIO_Pin pwr;
    GPIO_Pin nreset;

    GPIO_Mode __tx_af_pin;
    GPIO_Mode __rx_af_pin;
    GPIO_Mode __cts_af_pin;
    GPIO_Mode __rts_af_pin;

} GSM_GPIO;

typedef GSM_Status{
    uint8_t waiting_for_answer: 1;
    uint8_t gsm_status: 1;
    uint8_t gprs_status: 1;
    uint8_t sim_status: 1;
    uint8_t pwr_status: 1;
}

typedef struct GSM {

    GSM_GPIO gpio;

    USART_TypeDef *uart;
    char operator[10];
    uint16_t vbat;
    int8_t signal_level;
    char ip[15];  // SIM7000 IP address assigned by the operator
    char APN[20];
    char last_cmd[128];
    char rx_buf[128];
    GSM_Status status;
} GSM;

void GSM_Init(GSM *driver);
void GSM_SetPowerState(GSM *driver, GPIO_State power_state);
void GSM_SendSMS(GSM *driver, const char *data, const char *phone_num);
void GSM_SendTCP(GSM *driver, const char *data, const char *ip, const char *port);
uint8_t GSM_isAlive(GSM *driver);


