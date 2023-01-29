#include "periph_handlers.h"
#include "main.h"
#include "string.h"
// #include "periph_handlers.h"
// #include "sim7000.h"
// #include "fifo.h"



void USART3_IRQHandler(void) {
	if (sim7000g.uart->ISR & USART_ISR_RXNE) {
        while(!(sim7000g.uart->ISR & USART_ISR_TC));
        static uint8_t rx_counter = 0;
//		while(!(USART3->ISR & USART_ISR_TC));
        char data = sim7000g.uart->RDR;
        sim7000g.rx_buf[rx_counter] = data;
        rx_counter++;
        if(data == '\n'){
            if((strstr(sim7000g.rx_buf, "OK\r\n") != 0) || (strstr(sim7000g.rx_buf, "ERROR\r\n") != 0)){
            // if(array[rx_counter - 1] == '\r' && array[rx_counter - 2] == 'K' && array[rx_counter - 3] == 'O')
                sim7000g.status.waiting_for_answer = 0;
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