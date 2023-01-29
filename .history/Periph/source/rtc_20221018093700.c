/*
 * rtc.c
 *
 *  Created on: 3 февр. 2020 г.
 *      Author: BreakingBad
 */
#include "rtc.h"


/*
 * 						Effect of low-power modes on RTC
 * 		 _____________________________________________________________________________________________________
 * 		|	Sleep			 |	No effect. RTC interrupts cause the device to exit the Sleep mode.			  |
 * 		|	Low-power run	 |	No effect.																	  |
		|	Low-power sleep  |	No effect. RTC interrupts cause the device to exit the Low-power sleep mode	  |
		|____________________|________________________________________________________________________________|
 * 		|	Stop 0			 |																				  |
 * 		|	Stop 1			 |	Peripheral registers content is kept.										  |
 * 		|___Stop 2___________|________________________________________________________________________________|
 * 		|	Standby			 |	The RTC remains active when the RTC clock source is LSE or LSI. RTC alarm,	  |
		|					 |	RTC tamper event, RTC timestamp event, and RTC Wakeup cause the device to	  |
		|					 |	exit the Standby mode.														  |
 * 		|	Shutdown		 |	The RTC remains active when the RTC clock source is LSE. RTC alarm, RTC		  |
		|					 |	tamper event, RTC timestamp event, and RTC Wakeup cause the device to exit	  |
		|					 |	the Shutdown mode.															  |
 * 		|____________________|________________________________________________________________________________|
 *
 *
 */


void fill_RTC_struct_full(RTC_struct_brief volatile *br_data, RTC_struct_full volatile *f_data, uint32_t is_clock_cfg_mode)
{
	// get brief data format --> fill full data format
	// is_clock_cfg_mode == 0 --> alarm_cfg mode --> need only date, hour and minute tens/units

	f_data->year_tens = (is_clock_cfg_mode) ? (br_data->years / 10) : (f_data->year_tens);
	f_data->year_units = (is_clock_cfg_mode) ? (br_data->years - (f_data->year_tens * 10)) : (f_data->year_units);

	f_data->week_day = (is_clock_cfg_mode) ? br_data->week_day : (f_data->week_day);

	f_data->month_tens = (is_clock_cfg_mode) ? (br_data->months / 10) : (f_data->month_tens);
	f_data->month_units = (is_clock_cfg_mode) ? (br_data->months - (f_data->month_tens * 10)) : (f_data->month_units);

	f_data->date_tens = br_data->date / 10;
	f_data->date_units = (br_data->date - (f_data->date_tens * 10));

	f_data->hour_tens = br_data->hours / 10;
	f_data->hour_units = (br_data->hours - (f_data->hour_tens * 10));

	f_data->minute_tens = br_data->minutes / 10;
	f_data->minute_units = (br_data->minutes - (f_data->minute_tens * 10));

	f_data->second_tens = (is_clock_cfg_mode) ? (br_data->seconds / 10) : (f_data->second_tens);
	f_data->second_units = (is_clock_cfg_mode) ? (br_data->seconds - (f_data->second_tens * 10)) : (f_data->second_units);
}
void fill_struct_default(RTC_struct_brief volatile *br_data, RTC_struct_full volatile *f_data)
{
	// default date: FRI 27.12.19 04:49:00

	f_data->year_tens = 0x1;
	f_data->year_units = 0x9;
	f_data->week_day = 0x5;
	f_data->month_tens = 0x1;
	f_data->month_units = 0x2;
	f_data->date_tens = 0x2;
	f_data->date_units = 0x7;
	f_data->hour_tens = 0x0;
	f_data->hour_units = 0x4;
	f_data->minute_tens = 0x4;
	f_data->minute_units = 0x9;
	f_data->second_tens = 0x0;
	f_data->second_units = 0x0;

	br_data->years = 0x19;
	br_data->week_day = 0x5;
	br_data->months = 0x12;
	br_data->date = 0x27;
	br_data->hours = 0x04;
	br_data->minutes = 0x49;
	br_data->seconds = 0x00;
}
void RTC_auto_wakeup_enable(void) {
	// unlock write protection
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	RTC->CR &= ~RTC_CR_WUTE;  // disable the wakeup timer
	// polling to make sure the access to wakeup auto-reload counter and to WUCKSEL[2:0] bits is allowed
	while (!(RTC->ISR & RTC_ISR_WUTWF));
	// program the wakeup auto-reload value and the wakeup clock selection
	RTC->WUTR = 30;	// the WUTF flag is set every (WUT[15:0] + 1) = (0 + 1) = (1) ck_wut cycles
	RTC->CR &= ~RTC_CR_WUCKSEL_1;	// 10x: ck_spre (usually 1 Hz) clock is selected
	RTC->CR |= RTC_CR_WUCKSEL_2;

	// enable the RTC Wakeup interrupt - enable the EXTI Line 18
	if (!(RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN)) RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	EXTI->IMR1 |= EXTI_IMR1_IM20;	// interrupt request mask - IM22 is not masked now
	EXTI->RTSR1 |= EXTI_RTSR1_RT20;	// rising edge trigger enabled for EXTI line 17

	NVIC_EnableIRQ(RTC_WKUP_IRQn);	// enable the RTC_WKUP IRQ channel in the NVIC
	NVIC_ClearPendingIRQ(RTC_WKUP_IRQn);
	NVIC_SetPriority(RTC_WKUP_IRQn, 0);	// highest priority

	RTC->CR |= RTC_CR_WUTIE;  // 1: Wakeup timer interrupt enabled
	RTC->CR |= RTC_CR_WUTE;  // enable the timer again

	// lock write protection - writing a wrong key reactivates the write protection
	RTC->WPR = 0xFF;
	__enable_irq();	// global interrupts enable
}
void RTC_data_init(void){
	// uint32_t time_value, date_value;
	// unlock write protection
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	// initialization mode on (INITF == 1) - calendar counter is stopped, can update now
	RTC->ISR |= RTC_ISR_INIT;
	while (!(RTC->ISR & RTC_ISR_INITF));  // INITF polling
	// Fck_spre = RTCCLK / (PREDIV_S+1)*(PREDIV_A+1)
	RTC->PRER |= (0x7F << 16) | 0xFF;  // (0x7F+1) * (0xFF + 1) = 32768
	RTC->TR = 0x00000000;
	RTC->DR = 0x00000000;
	RTC->CR &= ~RTC_CR_FMT;  // 24h format == 0
    RTC->CALR |= 1 << 12;  // enable LPCAL
	RTC->ISR &= ~RTC_ISR_INIT;  // exit from the init mode
	// lock write protection - writing a wrong key reactivates the write protection
	RTC->WPR = 0xFF;
}

void RTC_data_update(RTC_struct_full volatile *f_data){
	uint32_t time_value, date_value;
	time_value = time_value ^ time_value;
	date_value = date_value ^ date_value;
	// unlock write protection
	PWR->CR1 |= PWR_CR1_DBP;
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	// initialization mode on (INITF == 1) - calendar counter is stopped, can update now
	RTC->ISR |= RTC_ISR_INIT;
	while (!(RTC->ISR & RTC_ISR_INITF));  // INITF polling
	// prescalers - two separate write access - synch predivider
	RTC->PRER |= 0xFF; // 255
	// asynch predivider
	RTC->PRER |= (0x7F << 16);	// 127
	RTC->TR = 0x00000000;
	time_value |= ((f_data->hour_tens << RTC_TR_HT_Pos) | (f_data->hour_units << RTC_TR_HU_Pos) | (f_data->minute_tens << RTC_TR_MNT_Pos) | (f_data->minute_units << RTC_TR_MNU_Pos));
	time_value |= ((f_data->second_tens << RTC_TR_ST_Pos) | (f_data->second_units << RTC_TR_SU_Pos));
	RTC->TR = time_value;
	RTC->DR = 0x00000000;
	date_value |= ((f_data->year_tens << RTC_DR_YT_Pos) | (f_data->year_units << RTC_DR_YU_Pos) | (f_data->week_day << RTC_DR_WDU_Pos) | (f_data->month_tens << RTC_DR_MT_Pos) | (f_data->month_units << RTC_DR_MU_Pos) | (f_data->date_tens << RTC_DR_DT_Pos) | (f_data->date_units << RTC_DR_DU_Pos));
	RTC->DR = date_value;
	RTC->CR &= ~RTC_CR_FMT;  // 24h format == 0
	RTC->ISR &= ~RTC_ISR_INIT;  // exit from the init mode
	// lock write protection - writing a wrong key reactivates the write protection
	RTC->WPR = 0xFF;
	PWR->CR1 &= ~PWR_CR1_DBP;
}

void RTC_init(void){
	RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;

	if((RCC->BDCR & RCC_BDCR_RTCEN) == 0){
		// check the INITS status flag in RTC_ISR register to verify if the calendar is already initialized
		if (RTC->ISR & RTC_ISR_INITS) return;
		// PWR clock on
		PWR->CR1 |= PWR_CR1_DBP;  // enable WRITE - allow access to backup registers (BDCR)
		if (!(RCC->BDCR & RCC_BDCR_RTCEN)) {  // pass only at the first time
			RCC->BDCR |= RCC_BDCR_BDRST;  // software reset - 1: Resets the entire Backup domain
			RCC->BDCR &= ~RCC_BDCR_BDRST;
		}
		RCC->BDCR |= RCC_BDCR_LSEON;  // enable LSE - Low-speed external oscillator
		while (!(RCC->BDCR & RCC_BDCR_LSERDY));  // wait for being ready by polling
		// LSE as clk source - [01] - LSE oscillator clock used as the RTC clock
		RCC->BDCR |= RCC_BDCR_RTCSEL_0;
		RCC->BDCR &= ~RCC_BDCR_RTCSEL_1;
		RCC->BDCR |= RCC_BDCR_RTCEN;  // RTC clock on
		// RTC_auto_wakeup_enable();
		// RTC_data_update(f_data);
		RTC_data_init();
		PWR->CR1 &= ~PWR_CR1_DBP;
	}
}
void rtc_writeToBkp(uint32_t *val, uint8_t size){
	if(!(RCC->APB1ENR1 & RCC_APB1ENR1_PWREN))
		RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;
	PWR->CR1 |= PWR_CR1_DBP;                     //Разрешить доступ к Backup области
	while((RTC->ISR & RTC_ISR_RSF) == 0);        //Wait for RTC APB registers synchronisation
	RTC->WPR = 0xCA;                             //Unlock write protection
	RTC->WPR = 0x53;                             //Unlock write protection

	for(uint8_t i = 0; i < size; i++){
		*(uint32_t*)(RTC + (uint32_t)(0x54 + i)) = val[i];
	}

	RTC->WPR = 0xFF;							//Enable the write protection for RTC registers
	PWR->CR1 &= ~PWR_CR1_DBP;                   //Запреть доступ к Backup области
}
void RTC_get_time(RTC_struct_brief volatile *br_data)
{
	// we need to clear less bits: (RTC->DR & RTC_DR_DT)
	// and to shift right the part, which we want to --> to normal decimal

	while (!(RTC->ISR & RTC_ISR_RSF));	//  Calendar shadow registers synchronized

	uint32_t TR_buf = 0, DR_buf = 0;

	TR_buf = (RTC->TR);

	br_data->hours = ((((TR_buf & RTC_TR_HT) >> RTC_TR_HT_Pos) * 10) + ((TR_buf & RTC_TR_HU) >> RTC_TR_HU_Pos));
	br_data->minutes = ((((TR_buf & RTC_TR_MNT) >> RTC_TR_MNT_Pos) * 10) + ((TR_buf & RTC_TR_MNU) >> RTC_TR_MNU_Pos));
	br_data->seconds = ((((TR_buf & RTC_TR_ST) >> RTC_TR_ST_Pos) * 10) + ((TR_buf & RTC_TR_SU) >> RTC_TR_SU_Pos));

	DR_buf = (RTC->DR);

	br_data->years = ((((DR_buf & RTC_DR_YT) >> RTC_DR_YT_Pos) * 10) + ((DR_buf & RTC_DR_YU) >> RTC_DR_YU_Pos));
	br_data->months = ((((DR_buf & RTC_DR_MT) >> RTC_DR_MT_Pos) * 10) + ((DR_buf & RTC_DR_MU) >> RTC_DR_MU_Pos));
	br_data->date = ((((DR_buf & RTC_DR_DT) >> RTC_DR_DT_Pos) * 10) + ((DR_buf & RTC_DR_DU) >> RTC_DR_DU_Pos));
	br_data->week_day = ((DR_buf & RTC_DR_WDU) >> RTC_DR_WDU_Pos);
}

void RTC_get_alarm(RTC_struct_brief volatile *br_data)
{
	br_data->date = ((((RTC->ALRMAR & RTC_ALRMAR_DT) >> RTC_ALRMAR_DT_Pos) * 10) + ((RTC->ALRMAR & RTC_ALRMAR_DU) >> RTC_ALRMAR_DU_Pos));
	br_data->hours = ((((RTC->ALRMAR & RTC_ALRMAR_HT) >> RTC_ALRMAR_HT_Pos) * 10) + ((RTC->ALRMAR & RTC_ALRMAR_HU) >> RTC_ALRMAR_HU_Pos));
	br_data->minutes = ((((RTC->ALRMAR & RTC_ALRMAR_MNT) >> RTC_ALRMAR_MNT_Pos) * 10) + ((RTC->ALRMAR & RTC_ALRMAR_MNU) >> RTC_ALRMAR_MNU_Pos));
}

uint32_t rtc_ReadFromBkp(void){
	return RTC->BKP0R;
}

void RTC_alarm_init(void)
{
	// unlock write protection
	PWR->CR1 |= PWR_CR1_DBP;
	while((RTC->ISR & RTC_ISR_RSF) == 0);
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;

	// disable Alarm A
	RTC->CR &= ~RTC_CR_ALRAE;
	RTC->ISR &= ~RTC_ISR_ALRAF;

	// wait for Alarm A write flag, to make sure the access to alarm reg is allowed
	while (!(RTC->ISR & RTC_ISR_ALRAWF));
	// date, hours, minutes, seconds mask - Alarm A set if they match
	RTC->ALRMAR |= RTC_ALRMAR_MSK4;		// 0: Alarm A set if the date/day match
	RTC->ALRMAR |= RTC_ALRMAR_MSK3;	// 0: Alarm A set if the hours match
	RTC->ALRMAR |= RTC_ALRMAR_MSK2;		// 0: Alarm A set if the minutes match
	// RTC->ALRMAR &= ~RTC_ALRMAR_MSK4;	// 0: Alarm A set if the date/day match
	// RTC->ALRMAR &= ~RTC_ALRMAR_MSK3;	// 0: Alarm A set if the hours match
	// RTC->ALRMAR &= ~RTC_ALRMAR_MSK2;	// 0: Alarm A set if the minutes match
	RTC->ALRMAR &= ~RTC_ALRMAR_MSK1;		// 1: Seconds don’t care in Alarm A comparison
	// RTC->ALRMAR |= RTC_ALRMAR_MSK1;		// 1: Seconds don’t care in Alarm A comparison

	RTC->ALRMAR &= ~RTC_ALRMAR_WDSEL;	// DU[3:0] field represents the date units
	RTC->ALRMAR &= ~RTC_ALRMAR_PM;		// 0: AM or 24-hour format

	// enable the RTC Alarm interrupt - enable the EXTI Line 18
	// SYSCFG (System configuration controller) clock through APB2 bus enable
	if (!(RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN)) RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	EXTI->IMR1 |= EXTI_IMR1_IM18;	// interrupt request mask - IM18 is not masked now
	EXTI->RTSR1 |= EXTI_RTSR1_RT18;	// rising edge trigger enabled for EXTI line 18

	NVIC_EnableIRQ(RTC_Alarm_IRQn);	// enable the RTC_Alarm IRQ channel in the NVIC
	NVIC_ClearPendingIRQ(RTC_Alarm_IRQn);
	NVIC_SetPriority(RTC_Alarm_IRQn, 0);	// highest priority

	// 1: Alarm A interrupt enabled
	RTC->CR |= RTC_CR_ALRAIE;
	// lock write protection - writing a wrong key reactivates the write protection
	RTC->WPR = 0xFF;
	PWR->CR1 &= ~PWR_CR1_DBP;

	__enable_irq();	// global interrupts enable
}

void RTC_alarm_update(RTC_struct_full volatile *f_data)
{
	// unlock write protection
	PWR->CR1 |= PWR_CR1_DBP;
	while((RTC->ISR & RTC_ISR_RSF) == 0);
	RTC->WPR = 0xCA;
	RTC->WPR = 0x53;
	// disable Alarm A
	RTC->CR &= ~RTC_CR_ALRAE;

	// wait for Alarm A write flag, to make sure the access to alarm reg is allowed
	while (!(RTC->ISR & RTC_ISR_ALRAWF));

	RTC->ALRMAR |= (f_data->date_tens << RTC_ALRMAR_DT_Pos);	// Bits 29:28 DT[1:0]: Date tens in BCD format
	RTC->ALRMAR |= (f_data->date_units << RTC_ALRMAR_DU_Pos);	// Bits 27:24 DU[3:0]: Date units or day in BCD format.
	RTC->ALRMAR |= (f_data->hour_tens << RTC_ALRMAR_HT_Pos);	// Bits 21:20 HT[1:0]: Hour tens in BCD forma
	RTC->ALRMAR |= (f_data->hour_units << RTC_ALRMAR_HU_Pos);	// Bits 19:16 HU[3:0]: Hour units in BCD format.
	RTC->ALRMAR |= (f_data->minute_tens << RTC_ALRMAR_MNT_Pos);	// Bits 14:12 MNT[2:0]: Minute tens in BCD format.
	RTC->ALRMAR |= (f_data->minute_units << RTC_ALRMAR_MNU_Pos);	// Bits 11:8 MNU[3:0]: Minute units in BCD format.

	// enable Alarm A
	RTC->CR |= RTC_CR_ALRAE;

	// enable the RTC Alarm interrupt - enable the EXTI Line 17
	// SYSCFG (System configuration controller) clock through APB2 bus enable
	if (!(RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN))
	{
		RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	}

	EXTI->IMR1 |= EXTI_IMR1_IM18;	// interrupt request mask - IM17 is not masked now
	EXTI->RTSR1 |= EXTI_RTSR1_RT18;	// rising edge trigger enabled for EXTI line 17

	NVIC_EnableIRQ(RTC_Alarm_IRQn);	// enable the RTC_Alarm IRQ channel in the NVIC
	NVIC_ClearPendingIRQ(RTC_Alarm_IRQn);
	NVIC_SetPriority(RTC_Alarm_IRQn, 0);	// highest priority

	// 1: Alarm A interrupt enabled
	RTC->CR |= RTC_CR_ALRAIE;
	// lock write protection - writing a wrong key reactivates the write protection
	RTC->WPR = 0xFF;
	PWR->CR1 &= ~PWR_CR1_DBP;

	__enable_irq();	// global interrupts enable
}
