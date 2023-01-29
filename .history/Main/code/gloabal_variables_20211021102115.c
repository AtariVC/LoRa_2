/*
 * gloabal_variables.c
 *
 *  Created on: 18 ���. 2020 �.
 *      Author: Gandalf
 */


#include "global_variables.h"


void init_global_variables(){
	// --------- System condition -----------//
	SYSTEM_init_status = SUCCESS;
	SYSTEM_I2C_error_flag = SUCCESS;
	SYSTEM_I2C_error_counter = 0;

	BME280_params = New_BME280(SENSORS_I2C, BME280_ADDRESS);
	// ===================================== //
	lora868 = newLoRa();
	
	lora_struct.HEAD = 0x00;
	lora_struct.ADDH = 0x00;
	lora_struct.ADDL = 0x00;
	lora_struct.SPED.SPED_REG_TRUE.parity = 0x00;
	lora_struct.SPED.SPED_REG_TRUE.uart_bps = 0x00;
	lora_struct.SPED.SPED_REG_TRUE.air_bps = 0x00;
	lora_struct.CHAN = 0x00;
	lora_struct.MODE = 0x00;
	lora_struct.OPTION.OPTION_REG_TRUE.fixed_transmission = 0x00;
	lora_struct.OPTION.OPTION_REG_TRUE.io_drive_mode = 0x00;
	lora_struct.OPTION.OPTION_REG_TRUE.wake_up_time = 0x00;
	lora_struct.OPTION.OPTION_REG_TRUE.FEC_switch = 0x00;
	lora_struct.OPTION.OPTION_REG_TRUE.tx_power = 0x00;

	tmp1075_sensor1_config = 0;
	tmp1075_sensor2_config = 0;
	tmp1075_sensor3_config = 0;
	tmp1075_sensor4_config = 0;
	tmp1075_sensor5_config = 0;

	//------ FOR LCD ------- //
	i = 0;
	x = 1;
	Y_offset = 60;
	X_offset = 8;
	start_cleaner = 0;
	deg = 0;
	cleaner = 6;
	freq = 3;
	x_v = 48;
	y_v_s = 15;
	y_v_e = 112;
	x_h_s = 30;
	x_h_e = 159;
	y_h = 60;
	x_dir = 1;
	y_dir = 1;

	number_array[0] = 0;
	number_array[1] = 0;
	number_array[2] = 0;
	number_array[3] = 0;
	//===========================//

	// --------- LoRa -----------//
	FIFO_flag = 0;
	TIME_TO_RESET = 40;
	TIME_TO_RESET_COUNTER = 0;
	PC_CONNECT_START = 0;
	sensors_data_size = 10; // 5 �������� �� 2 �����
	LoRaHL_byte_counter = 0;
	LoRa_the_first_message = 0;
	packet_counter = 0;
	LoRa_self_address = 0x00; //1- 100mW, 2- 1W
	   //(start, addr, type, length,  counter, addr1, addr2, cmd_r, cmd_b, crc)
//uint8_t tx_packet[] = {0xFF, 0x02, 0x00, packet_len, 0x00, 0x01, 0x02, 0x03, 0x00, cmd, 0x00};
	LoRa_last_rx_packet[0] = 0xFF;
	LoRa_last_rx_packet[1] = 0x03;
	LoRa_last_rx_packet[2] = 0x00;
	LoRa_last_rx_packet[3] = 12;
	LoRa_last_rx_packet[4] = 0x02;
	LoRa_last_rx_packet[5] = 0x01;
	LoRa_last_rx_packet[6] = 0x02;
	LoRa_last_rx_packet[7] = 0x03;
	LoRa_last_rx_packet[8] = 16;
	LoRa_last_rx_packet[9] = 0x00;
	LoRa_last_rx_packet[10] = 0xAB;
	LoRa_last_rx_packet[11] = 0xC3;

	current_station_stats.sensors_data[0] = 0xFEAA;
	current_station_stats.sensors_data[1] = 0xFCBB;
	current_station_stats.sensors_data[2] = 0xFDCC;
	current_station_stats.sensors_data[3] = 0xFADD;
	current_station_stats.sensors_data[4] = 0xFBEE;
	//===========================//


	// --------- USART1 -----------//
	receive_buf[0] = 0;
	receive_buf[1] = 0;
	receive_buf[2] = 0;
	receive_buf[3] = 0;
	receive_data_counter = 0;
	command_error_flag = 0;
	//===========================//

	// --------- BUZZ -----------//
	buzz_frequency = 440;
	buzz_duration = 10;
	//===========================//

	// --------- EXTI -----------//
	button_pressed_counter = 0;
	//===========================//

	// --------- RGB_LED -----------//
	LED_init_flag[0] = 0;  // 7, 6 - NA; 0:3 - LED1_R(GB) PWM Inited; 3:6 - LED1_R(GB) bin mode Inited
	LED_init_flag[1] = 0;
	LED_init_flag[2] = 0;
	//===========================//

	// --------- ADC -----------//
	battary_voltage = 0;
	//===========================//

	// RTC_struct rtc = {20, 2, 1, 3, 18, 23, 0};


}
