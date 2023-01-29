#include "gsm.h"
#include "gpio.h"
#include "uart.h"
#include "delay.h"

void GSM_Init(GSM *driver){
    gpio_init(driver->gpio.tx, driver->gpio.__tx_af_pin, Push_pull, no_pull, Very_high_speed);
    gpio_init(driver->gpio.rx, driver->gpio.__rx_af_pin, Open_drain, no_pull, Input);
    gpio_init(driver->gpio.pwr, General_output, Push_pull, pull_up, Low_speed);

    UART_init(driver->uart, 9600, FULL_DUPLEX);

}
uint8_t GSM_isAlive(GSM *driver){
    UART_tx_string(driver->uart, "AT\r\n");
    driver->tx_counter++;
    driver->status.waiting_for_answer = 1;
    while(driver->status.waiting_for_answer);
    if(driver->last_cmd)
    return 0;
}
void GSM_SetPowerState(GSM *driver, GPIO_State state){
    gpio_state(driver->gpio.pwr, LOW);
    Delay(900);
    gpio_state(driver->gpio.pwr, HIGH);
    driver->status.pwr_status = state;
}

void GSM_InitGPRS(GSM *driver){

}

void GSM_SendSMS(GSM *driver, const char *data, const char *phone_num){

}

void GSM_SendTCP(GSM *driver, const char *data, const char *ip, const char *port){

}
void GSM_SetDNS(GSM *driver){
     UART_tx_string(driver->uart, "AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"");
     driver->tx_counter++;
}
void GSM_SetTime(GSM *driver, const char *time_data){
    char *buf[20] = {"AT+CCLK="};
    UART_tx_string(driver->uart, buf);
}