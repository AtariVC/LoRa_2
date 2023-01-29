#include "sim7000.h"
#include "gpio.h"
#include "uart.h"
#include "delay.h"

void SIM7000_Init(SIM7000 *driver){
    gpio_init(driver->gpio.tx, driver->gpio.__tx_af_pin, Push_pull, no_pull, Very_high_speed);
    gpio_init(driver->gpio.rx, driver->gpio.__rx_af_pin, Open_drain, no_pull, Input);
    gpio_init(driver->gpio.pwr, General_output, Push_pull, pull_up, Low_speed);

    UART_init(driver->uart, 9600, FULL_DUPLEX);

}
uint8_t SIM7000_isAlive(SIM7000 *driver){
    UART_tx_string(driver->uart, "AT\r\n");
    return 0;
}
void SIM7000_SetPowerState(SIM7000 *driver, GPIO_State state){
    gpio_state(driver->gpio.pwr, LOW);
    Delay(900);
    gpio_state(driver->gpio.pwr, HIGH);
    driver->power_state = state;
}

void SIM7000_InitGPRS(SIM7000 *driver){

}

void SIM7000_SendSMS(SIM7000 *driver, const char *data, const char *phone_num){

}

void SIM7000_SendTCP(SIM7000 *driver, const char *data, const char *ip, const char *port){

}

void SIM7000_SetTime(SIM7000 *driver, const char *time_data){
    char *buf[20] = {"AT+CCLK="};
    UART_tx_string(driver->uart, buf);
}