/*
 * global_variables.h
 *
 *  Created on: 18 ���. 2020 �.
 *      Author: Gandalf
 */

#ifndef INC_GLOBAL_VARIABLES_H_
#define INC_GLOBAL_VARIABLES_H_

#include "stm32l4xx.h"
#include "rgb_led.h"
#include "fifo.h"
#include "sx1278.h"
#include "rtc.h"
#include "BME280.h"
//#include "main.h"

// --------- System condition -----------//
ErrorStatus SYSTEM_init_status;
ErrorStatus SYSTEM_I2C_error_flag;
uint32_t SYSTEM_I2C_error_counter;

BME280_Struct BME280_params;
// ===================================== //

//------ LCD ------- //
uint16_t i,x;
uint8_t Y_offset;
uint8_t X_offset;
uint8_t start_cleaner;
uint16_t deg;
uint16_t cleaner;
uint8_t freq;
int x_v;
int y_v_s;
int y_v_e;
int x_h_s;
int x_h_e;
int y_h;
int x_dir;
int y_dir;

uint8_t number_array[3];
uint8_t vbat_arr[4];
char arr[4];
//===========================//



// --------- LoRa -----------//
LoRa lora868;

#define LORA_SLEEP_MODE			3
#define LORA_NORMAL_MODE		0
uint8_t lora_receive_data;
uint8_t LoRaHL_byte_counter;
uint8_t LoRaHL_rx_buffer[8];

uint8_t TIME_TO_RESET;
uint8_t TIME_TO_RESET_COUNTER;
uint8_t PC_CONNECT_START;
uint8_t LoRa_init_flag;
uint8_t LoRa_the_first_message;
FIFO(64) fifo;
FIFO(64) long_cmd_fifo;
FIFO(64) sensors_data_rx_fifo;
uint8_t sensors_data_size;
uint8_t FIFO_flag;
uint32_t packet_counter;
uint8_t LoRa_self_address;
volatile uint8_t LoRa_last_rx_packet[30];
uint8_t LoRa_last_rx_data[10];
typedef struct LoRa_Station_struct {
	uint16_t sensors_data[5];
	uint16_t voltage;
	uint16_t condition_flags;
}LoRa_station_struct;
LoRa_station_struct current_station_stats;

#pragma pack(push, 1)
typedef union{
	struct {
		uint8_t air_bps : 3;
		uint8_t uart_bps : 3;
		uint8_t parity : 2;
	} SPED_REG_REVERSE;

	struct {
		uint8_t parity : 2;
		uint8_t uart_bps : 3;
		uint8_t air_bps : 3;
	} SPED_REG_TRUE;
} SPED_REG;

typedef union{
	struct{
		uint8_t tx_power : 2;
		uint8_t FEC_switch : 1;
		uint8_t wake_up_time : 3;
		uint8_t io_drive_mode : 1;
		uint8_t fixed_transmission : 1;
	}OPTION_REG_REVERSE;

	struct{
		uint8_t fixed_transmission : 1;
		uint8_t io_drive_mode : 1;
		uint8_t wake_up_time : 3;
		uint8_t FEC_switch : 1;
		uint8_t tx_power : 2;
	}OPTION_REG_TRUE;
} OPTION_REG;

typedef struct _LoRa_struct {
	uint8_t HEAD;
	uint8_t ADDH;
	uint8_t ADDL;
	SPED_REG SPED;
	uint8_t CHAN;
	OPTION_REG OPTION;
	uint8_t MODE;
} LoRa_struct;
LoRa_struct lora_struct;
#pragma pack(pop)
//===========================//



// --------- TMP1075 -----------//
uint8_t tmp1075_sensor1_config;
uint8_t tmp1075_sensor2_config;
uint8_t tmp1075_sensor3_config;
uint8_t tmp1075_sensor4_config;
uint8_t tmp1075_sensor5_config;
//===========================//

// --------- USART1 -----------//
uint8_t receive_buf[4];
uint8_t receive_data_counter;
uint8_t command_error_flag;
//===========================//

// --------- BUZZ -----------//
uint16_t buzz_frequency;
uint16_t buzz_duration;
//===========================//

// --------- EXTI -----------//
uint8_t button_pressed_counter;
//===========================//

// --------- RGB_LED -----------//
uint8_t LED_init_flag[3];
//===========================//

// --------- ADC -----------//
volatile float battary_voltage;
//===========================//

// --------- TIMERS -----------//
uint8_t TIM2_finished; // Global flag that indicates status of timer. 0 - still counting; 1 - finished
uint8_t TIM7_finished; // Global flag that indicates status of timer. 0 - still counting; 1 - finished
//===========================//
// RTC_struct rtc;

void init_global_variables();

#endif /* INC_GLOBAL_VARIABLES_H_ */
