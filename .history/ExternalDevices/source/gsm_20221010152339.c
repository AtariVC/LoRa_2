#include "main.h"
// #include "gsm.h"
// #include "gpio.h"
// #include "uart.h"
// #include "delay.h"
#include "string.h"

void GSM_Init(GSM *driver){
    gpio_init(driver->gpio.tx, driver->gpio.__tx_af_pin, Push_pull, no_pull, Very_high_speed);
    gpio_init(driver->gpio.rx, driver->gpio.__rx_af_pin, Open_drain, no_pull, Input);
    gpio_init(driver->gpio.pwr, General_output, Push_pull, pull_up, Low_speed);

    UART_init(driver->uart, 9600, FULL_DUPLEX);

}
void GSM_wait_for_answer(GSM *driver, int32_t timeout_ms){
    while((timeout_ms--) && (driver->status.waiting_for_answer))
        Delay(1);
}
void GSM_SendCMD(GSM *driver, char *cmd){
    UART_tx_string(driver->uart, cmd);
    memcpy(driver->last_cmd, cmd, strlen(cmd));
    driver->tx_counter++;
    driver->status.waiting_for_answer = 1;
    GSM_wait_for_answer(driver, 2000);
}

uint8_t GSM_isAlive(GSM *driver){
    GSM_SendCMD(driver, "AT\r");
    if(driver->status.timeout_event)
        return 1;
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
    GSM_SendCMD(driver, "AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"");
}
void GSM_SetTime(GSM *driver, const char *time_data){
    char *buf[20] = {"AT+CCLK="};
    GSM_SendCMD(driver, buf);
}



void GSM_RX_Handler(){
    if (sim7000g.uart->ISR & USART_ISR_RXNE) {
        while(!(sim7000g.uart->ISR & USART_ISR_TC));
        static uint8_t rx_counter = 0;
        char data = sim7000g.uart->RDR;
        sim7000g.rx_buf[rx_counter] = data;
        rx_counter++;
        if(data == '\n'){
            if(strstr(sim7000g.rx_buf, "OK\r\n") != 0){
                sim7000g.status.waiting_for_answer = 0;
                sim7000g.status.last_answer = 0;
                rx_counter = 0;
            }
            else if(strstr(sim7000g.rx_buf, "ERROR\r\n") != 0){
                sim7000g.status.waiting_for_answer = 0;
                sim7000g.status.last_answer = 1;
                rx_counter = 0;
            }
            else if(strstr(sim7000g.rx_buf, "POWER DOWN\r\n") != 0){
                sim7000g.status.pwr_status = 0;
                rx_counter = 0;
            }
            else if(strstr(sim7000g.rx_buf, "+CPIN") != 0){
                sim7000g.status.sim_status = 1;
                rx_counter = 0;
            }
        }
        if(rx_counter >= 126){
            rx_counter = 0;
            sim7000g.status.buffer_filled = 1;
        }
	}
	if(sim7000g.uart->ISR & USART_ISR_ORE){
        char data = sim7000g.uart->RDR;
		sim7000g.uart->ICR |= USART_ICR_ORECF;
		// UART_tx_array(USART1, "USART3 OVERRUN ERROR!\r\n");
	}
    if(sim7000g.uart->ISR & USART_ISR_FE){
        sim7000g.uart->ICR |= USART_ICR_FECF;
    }
}