#include "tim.h"
#include "uart.h"
#include "gpio.h"
#include "global_variables.h"


volatile uint8_t array[6] = {0};
void USART3_IRQHandler(void) {
	if (USART3->ISR & USART_ISR_RXNE) {
//		while(!(USART3->ISR & USART_ISR_TC));
		sim7000g = USART3->RDR;
		static uint8_t lora_init_byte_counter = 0;

		if(LoRa_init_flag == 1){
			if(LoRa_the_first_message == 0)
				LoRa_the_first_message = lora_receive_data;
			*((uint8_t*)(&lora_struct) + lora_init_byte_counter) = lora_receive_data;
			array[lora_init_byte_counter] = lora_receive_data;
			lora_init_byte_counter++;
			if(lora_init_byte_counter == 6){
				lora_init_byte_counter = 0;
				LoRa_init_flag = 0;
				// gpio_state(LoRa_M0, LOW);
				// gpio_state(LoRa_M1, LOW);
				lora_struct.MODE = 0;
			}
		}
		else{
			if(lora_struct.MODE == 0){
				int8_t packet_status = LoRa_protocol_handler(lora_receive_data);
				switch (packet_status) {
					case START_BYTE_ERROR:
	//					UART_tx_array(USART1, "START_BYTE_ERROR");
						break;
					case ADDRESS_BYTE_ERROR:
	//					UART_tx_array(USART1, "ADDRESS_BYTE_ERROR");
						break;
					case CMD_TYPE_ERROR:
	//					UART_tx_array(USART1, "CMD_TYPE_ERROR");
						break;
					case PACKET_LENGTH_ERROR:
	//					UART_tx_array(USART1, "PACKET_LENGTH_ERROR");
						break;
					case CRC_ERROR:
	//					UART_tx_array(USART1, "CRC_ERROR");
						break;
					case SUCCESS_RX:
		//				UART_tx_array(USART1, "SUCCESS_RX");
						break;
					default:
						break;
				}
	//			FIFO_PUSH(fifo, lora_receive_data);
			}
			else if(lora_struct.MODE == 3){
				FIFO_PUSH(fifo, lora_receive_data);
				FIFO_PUSH(fifo, '\n');
			}
		}
	}
	else if(USART3->ISR & USART_ISR_ORE){
		USART3->ICR |= USART_ICR_ORECF;
		// LED_Handler(2, LED1_R, 0, 0);
		// LED_Handler(2, LED2_R, 0, 0);
		// LED_Handler(2, LED3_R, 0, 0);
		UART_tx_array(USART1, "USART3 OVERRUN ERROR!\r\n");
	}
}
void RTC_Alarm_IRQHandler(void)
{
	EXTI->PR1 |= EXTI_PR1_PIF18;	// clear pending flag
	// // unlock write protection
	PWR->CR1 |= PWR_CR1_DBP;
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	// // disable Alarm A
	// RTC->CR &= ~RTC_CR_ALRAE;

	// // wait for Alarm A write flag, to make sure the access to alarm reg is allowed
	// while (!(RTC->ISR & RTC_ISR_ALRAWF));

	// RTC->ALRMAR &= (RTC_ALRMAR_DU ^ RTC_ALRMAR_DU);		// Bits 27:24 DU[3:0]: Date units or day in BCD format.
	// RTC->ALRMAR &= (RTC_ALRMAR_HT ^ RTC_ALRMAR_HT);		// Bits 21:20 HT[1:0]: Hour tens in BCD forma
	// RTC->ALRMAR &= (RTC_ALRMAR_HU ^ RTC_ALRMAR_HU);		// Bits 19:16 HU[3:0]: Hour units in BCD format.
	// RTC->ALRMAR &= (RTC_ALRMAR_MNT ^ RTC_ALRMAR_MNT);	// Bits 14:12 MNT[2:0]: Minute tens in BCD format.
	// RTC->ALRMAR &= (RTC_ALRMAR_MNU ^ RTC_ALRMAR_MNU);	// Bits 11:8 MNU[3:0]: Minute units in BCD format.

	// // lock write protection - writing a wrong key reactivates the write protection
	RTC->ISR &= ~RTC_ISR_ALRAF;	//  flag is cleared by software by writing 0
	RTC->WPR = 0xFF;
	PWR->CR1 &= ~PWR_CR1_DBP;


	// UART_tx_array(USART1, "RTC handler\r\n");
}