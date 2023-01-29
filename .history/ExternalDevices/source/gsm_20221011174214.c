#include "main.h"
#include "gsm.h"
#include "gpio.h"
// #include "uart.h"
// #include "delay.h"
#include "string.h"
#include "stdlib.h"

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
    memcpy(driver->last_cmd, cmd, strlen(cmd));
    UART_tx_string(driver->uart, cmd);
    UART_tx(driver->uart, '\r');
    driver->tx_counter++;
    driver->status.waiting_for_answer = 1;
    GSM_wait_for_answer(driver, 2000);
    // if(driver->status.last_answer){
    //     memcpy(driver->last_cmd, cmd, strlen(cmd));
    //     UART_tx_string(driver->uart, cmd);
    //     driver->tx_counter++;
    //     driver->status.waiting_for_answer = 1;
    //     GSM_wait_for_answer(driver, 2000);
    // }
}

void GSM_SetAPN(GSM *driver, char *apn){
    char *cmd = "AT+CSTT=\"";
    size_t apn_len = strlen(apn);
    size_t cmd_len = strlen(cmd);
    char *buf = malloc(apn_len + cmd_len + 2); // один дополнительный байт для \", а второй для \0
    memcpy(buf, cmd, cmd_len);
    memcpy(buf + cmd_len, apn, apn_len);
    memcpy(buf + cmd_len + apn_len, "\"", 2);
    GSM_SendCMD(driver, buf);
    free(buf);
}

void GSM_CheckSignal(GSM *driver){
    GSM_SendCMD(driver, "AT+CSQ?");
}
uint16_t GSM_GetVBAT(GSM *driver){
    GSM_SendCMD(driver, "AT+CBC");
    return driver->vbat;
}
uint8_t GSM_isAlive(GSM *driver){
    GSM_SendCMD(driver, "AT");
    if(driver->status.timeout_event){
        driver->status.pwr_status = 0;
        return 1;
    }
    if(!driver->status.last_answer)
        driver->status.pwr_status = 1;
    return 0;
}
void GSM_SetPowerState(GSM *driver, GPIO_State state){
    gpio_state(driver->gpio.pwr, LOW);
    Delay(900);
    gpio_state(driver->gpio.pwr, HIGH);
    Delay(5000);
    driver->status.pwr_status = state;
}

void GSM_InitGPRS(GSM *driver){

}

void GSM_SendSMS(GSM *driver, char *data, char *phone_num){
    GSM_SendCMD(driver, "AT+CMGF=1");
}
void GSM_OpenConnection(GSM *driver, char *ip, char *port){
    if(driver->ip_status == GPRS_STATUS){
        char *cmd = "AT+CIPSTART=\"TCP\",\"";
        size_t ip_len = strlen(ip);
        size_t port_len = strlen(port);
        size_t cmd_len = strlen(cmd);
        char *buf = malloc(cmd + ip_len + port_len + 2);  // "AT+CIPSTART=\"TCP\",\"10.6.1.4\",80");
        memcpy(buf, cmd, cmd_len);
        memcpy(buf + cmd_len, ip, ip_len);
        memcpy(buf + cmd_len + ip_len, "\"", 1);
        memcpy(buf + cmd_len + ip_len + 1, port, port_len + 1);
        GSM_SendCMD(driver, buf);
        free(buf);
    }
}
void GSM_SendTCP(GSM *driver, char *data, char *data_len){

}

void GSM_SetDNS(GSM *driver){  // "AT+CMGS=\"+7xxxxxxxxxx\"\r"
    GSM_SendCMD(driver, "AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"");
}

// void GSM_SetTime(GSM *driver, char *time_data){
//     char *buf[20] = {"AT+CCLK="};
//     GSM_SendCMD(driver, buf);
// }

void GSM_CheckIPstatus(GSM *driver){
    GSM_SendCMD(driver, "AT+CIPSTATUS");
}

void GSM_ActivateContext(GSM *driver){
    GSM_SendCMD(driver, "AT+CIICR");
}
void GSM_RequestIP(GSM *driver){
    GSM_SendCMD(driver, "AT+CIFSR");
}
void GSM_RX_Handler(){
    if (sim7000g.uart->ISR & USART_ISR_RXNE) {
        // while(!(sim7000g.uart->ISR & USART_ISR_TC));
        static uint8_t rx_counter = 0;
        char data = sim7000g.uart->RDR;
        sim7000g.rx_buf[rx_counter] = data;
        rx_counter++;
        // if(data == '\n'){
        //     if(strstr(sim7000g.rx_buf, "OK\r\n") != 0){
        //         sim7000g.status.waiting_for_answer = 0;
        //         sim7000g.status.last_answer = 0;
        //         // memset(sim7000g.rx_buf, 0, rx_counter);
        //         // rx_counter = 0;
        //     }
        //     else if(strstr(sim7000g.rx_buf, "ERROR\r\n") != 0){
        //         sim7000g.status.waiting_for_answer = 0;
        //         sim7000g.status.last_answer = 1;
        //         // memset(sim7000g.rx_buf, 0, rx_counter);
        //         // rx_counter = 0;
        //     }
            // else if(strstr(sim7000g.rx_buf, "STATE:") != 0){
            //     if(strstr(sim7000g.rx_buf, "IP INITIAL") != 0){
            //         sim7000g.ip_status = GPRS_INITIAL;
            //     }
            //     else if(strstr(sim7000g.rx_buf, "IP START") != 0){
            //         sim7000g.ip_status = GPRS_START;
            //     }
            //     else if(strstr(sim7000g.rx_buf, "IP GPRSACT") != 0){
            //         sim7000g.ip_status = GPRS_GPRSACT;
            //     }
            //     else if(strstr(sim7000g.rx_buf, "IP STATUS") != 0){
            //         sim7000g.ip_status = GPRS_STATUS;
            //     }
            //     else if(strstr(sim7000g.rx_buf, "CONNECT OK") != 0){
            //         sim7000g.ip_status = GPRS_CONNECT_OK;
            //     }
            //     rx_counter = 0;
            //     memset(sim7000g.rx_buf, 0, sizeof(sim7000g.rx_buf));
            // }
            // else if(strstr(sim7000g.rx_buf, "POWER DOWN\r\n") != 0){
            //     sim7000g.status.pwr_status = 0;
            //     memset(sim7000g.rx_buf, 0, rx_counter);
            //     rx_counter = 0;
            // }
            // else if(strstr(sim7000g.rx_buf, "+CPIN:") != 0){
            //     if(strstr(sim7000g.rx_buf, "+CPIN: READY") != 0) sim7000g.status.sim_status = 1;
            //     else sim7000g.status.sim_status = 0;
            // }
            // else if(strstr(sim7000g.rx_buf, "+CREG:") != 0){
            //     if(strstr(sim7000g.rx_buf, "+CREG: 0,1") != 0) sim7000g.status.gsm_status = 1;
            //     else sim7000g.status.gsm_status = 0;
            // }
            // else if(strstr(sim7000g.rx_buf, "+CGREG:") != 0){
            //     if(strstr(sim7000g.rx_buf, "+CGREG: 0,1") != 0) sim7000g.status.gprs_status = 1;
            //     else sim7000g.status.gprs_status = 0;
            // }
            // else if(strstr(sim7000g.rx_buf, "+CSQ:") != 0){
            //     sscanf(sim7000g.rx_buf, "\r\n+CSQ: %hhd\r\n", &sim7000g.signal_level);
            // }
            // else if(strstr(sim7000g.rx_buf, "+CSTT:") != 0){
            // }
            // else if(strstr(sim7000g.rx_buf, "+CBC") != 0){
            //     uint8_t charger_status = 0;
            //     uint8_t percent = 0;
            //     sscanf(sim7000g.rx_buf, "\r\n+CBC: %hhd,%hhd,%hd\r\n", &charger_status, &percent, &sim7000g.vbat);
            // }
        // }
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

