#ifndef CODE_HEADER_GSM_H_
#define CODE_HEADER_GSM_H_

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
typedef enum GSM_IP_STATUS{
    GPRS_INITIAL,
    GPRS_START,
    GPRS_GPRSACT,
    GPRS_STATUS,
    GPRS_CONNECT_OK
} GSM_IP_STATUS;
typedef struct GSM_Status {
    uint8_t waiting_for_answer: 1;  // 0 - have got answer, 1 - wating for answer
    uint8_t gsm_status: 1;          // 0 - GSM not connected, 1 - GSM connected
    uint8_t gprs_status: 1;         // 0 - GPRS not connected, 1 - GPRS connected
    uint8_t sim_status: 1;          // 0 - SIM not connected, 1 - SIM connected
    uint8_t pwr_status: 1;          // 0 - PWR OFF, 1 - PWR ON
    uint8_t buffer_filled: 1;       // 0 - buffer OK, 1 - buffer filled
    uint8_t timeout_event: 1;       // 0 - timeout OK, 1 - timeout occured
    uint8_t last_answer: 1;         // 0 - OK, 1 - ERROR
} GSM_Status;

typedef struct GSM {
    GSM_GPIO gpio;
    USART_TypeDef *uart;
    char operator[10];
    uint16_t vbat;
    uint8_t signal_level;
    char ip[15];  // SIM7000 IP address assigned by the operator
    char APN[20];
    char last_cmd[128];
    char rx_buf[128];
    GSM_IP_STATUS ip_status;
    GSM_Status status;
    uint16_t tx_counter;
    uint8_t sms_counter;
    uint8_t call_counter;
    uint8_t timeout_sec;

} GSM;

void GSM_RX_Handler();
void GSM_Init(GSM *driver);
void GSM_CheckSignal(GSM *driver);
void GSM_SetAPN(GSM *driver);
void GSM_SendCMD(GSM *driver, char *cmd);
uint16_t GSM_GetVBAT(GSM *driver);
void GSM_ActivateContext(GSM *driver);
void GSM_OpenConnection(GSM *driver, char *ip, char *port);
void GSM_RequestIP(GSM *driver);
void GSM_CheckIPstatus(GSM *driver);
void GSM_SetPowerState(GSM *driver, GPIO_State power_state);
void GSM_SendSMS(GSM *driver, char *data, char *phone_num);
void GSM_SendTCP(GSM *driver, char *data);
uint8_t GSM_isAlive(GSM *driver);


#endif