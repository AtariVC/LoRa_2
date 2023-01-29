#include "main.h"
// #include "sdio.h"
// #include "fat32.h"
#include "low_power.h"



volatile time_t current_time;
void LED_BLINK(){
	gpio_toggle(LED);
	Delay(1000);
}

int main(){
	// init_global_variables();
    SCB->CPACR |= 0x0F << 20;
    System_Init();
    while(1){
	// delay_action(5000, 0, LED_BLINK);
        switch (event_register)
        {
        case ECHO:
            SX126x_SendData(&SX1268, SX1268.rx_data, SX1268.rx_pkt_len);
            break;

        default:
            break;
        }
		LED_BLINK();

   	// delay_action(15, 1, LCD_PINBALL);
	// delay_action(500, 2, UART_SEND);
	// delay_action(100, 3, SEND_SENSORS_DATA);
	}
}


