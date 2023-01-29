#include "exti.h"
#include "gpio.h"
void EXTI_init(GPIO_Pin gpio){
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    uint8_t port_num = gpio / 16;  // GPIOA = 1, GPIOB = 2, ..., GPIOE = 5
	uint8_t pin = gpio % 16;  // PB0 = 0, PA1 = 1, PD4 = 4
	gpio_init(gpio, Input_mode, Open_drain, no_pull, Input);
	EXTI->IMR1 |= EXTI_IMR1_IM4;
	EXTI->RTSR1 |= EXTI_RTSR1_RT4;
	EXTI->FTSR1 &= ~EXTI_FTSR1_FT4;
	SYSCFG->EXTICR[1] = SYSCFG_EXTICR2_EXTI4_PB;
	NVIC_EnableIRQ(EXTI4_IRQn);
}
