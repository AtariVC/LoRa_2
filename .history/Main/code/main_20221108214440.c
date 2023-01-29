#include "main.h"
// #include "sdio.h"
// #include "fat32.h"
#include "low_power.h"



volatile time_t current_time;
void LED_BLINK(){
	gpio_toggle(LED);
	// Delay(1000);
}
void SEND_TO_SERVER(){
    if(SX1268.self_addr == 1){
        sim7000g = (GSM){
            .gpio = {
                .rx = UART3_RX,
                .tx = UART3_TX,
                .rts = uninitialized,
                .cts = uninitialized,
                .dcd = GSM_DCD,
                .ri = GSM_RI,
                .dtr = GSM_DTR,
                .pwr = GSM_PWR,
                .nreset = uninitialized,
                .__rx_af_pin = PB11_USART3_RX,
                .__tx_af_pin = PB10_USART3_TX,
                .__cts_af_pin = uninitialized,
                .__rts_af_pin = uninitialized
            },
            .uart = USART3
        };

        uint8_t gsm_init_flag = GSM_Init(&sim7000g);
        GSM_CheckSignal(&sim7000g);
        uint16_t vbat = GSM_GetVBAT(&sim7000g);
        uint8_t gprs_init_flag = GSM_InitGPRS(&sim7000g);
        if(gprs_init_flag == 0)
            GSM_OpenConnection(&sim7000g, "hirundo.ru", "8083");
        if(sim7000g.ip_status==GPRS_CONNECT_OK){
            Delay(300);
            // мб стоит ожидать появления символа '>'
            GSM_SendTCP(&sim7000g, "hello", 5);
            Delay(500);
            GSM_CloseConnections(&sim7000g);
        }
        else GSM_CloseConnections(&sim7000g);
        GSM_PowerOFF(&sim7000g);
    }
}
int main(){
	// init_global_variables();
    SCB->CPACR |= 0x0F << 20;
    System_Init();
    while(1){
	// delay_action(5000, 0, LED_BLINK);
        if(SX1268.new_rx_data_flag){
            SX126x_RxDataParse(&SX1268);
        }


		delay_action(300000, 1, SEND_TO_SERVER);

   	// delay_action(15, 1, LCD_PINBALL);
	// delay_action(500, 2, UART_SEND);
	// delay_action(100, 3, SEND_SENSORS_DATA);
	}
}


