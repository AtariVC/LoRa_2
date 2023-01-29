#ifndef CODE_HEADER_RTC_H_
#define CODE_HEADER_RTC_H_

#include "stm32l4xx.h"

typedef struct s_RTC_struct_full {
	uint8_t year_tens;		// 20[1]9
	uint8_t year_units;		// 201[9]
	uint8_t week_day;		// 001 for Monday, 111 for Sunday
	uint8_t month_tens;		// [1]2
	uint8_t month_units;		// 1[2]
	uint8_t date_tens;		// [2]5
	uint8_t date_units;		// 2[5]
	uint8_t hour_tens;		// [0]0
	uint8_t hour_units;		// 0[0]
	uint8_t minute_tens;		// [0]3
	uint8_t minute_units;		// 0[3]
	uint8_t second_tens;		// [1]5
	uint8_t second_units;		// 1[5]
} RTC_struct_full;

typedef struct s_RTC_struct_brief {
	uint8_t years;			// 2019
	uint8_t week_day;		// 001 for Monday, 111 for Sunday
	uint8_t months;			// 12
	uint8_t date;			// 25
	uint8_t hours;			// 0
	uint8_t minutes;		// 03
	uint8_t seconds;		// 15
} RTC_struct_brief;

volatile RTC_struct_full RTC_data_full_buff;
volatile RTC_struct_brief RTC_data_brief_buff;
void rtc_writeToBkp(uint32_t *val, uint8_t size);
void RTC_auto_wakeup_enable(uint16_t period_sec);
void fill_struct_default(RTC_struct_brief volatile *br_data, RTC_struct_full volatile *f_data);
void RTC_Init();
#endif /* CODE_HEADER_RTC_H_ */
