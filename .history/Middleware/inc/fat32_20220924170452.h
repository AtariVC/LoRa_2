/*
 * fat32.h
 *
 *  Created on: 7 ���. 2020 �.
 *      Author: Gandalf
 */

#ifndef INC_FAT32_H_
#define INC_FAT32_H_
#include "stm32l4xx.h"

#define BPB_BytsPerSec_offset		11
#define BPB_SecPerClus_offset		13
#define BPB_RsvdSecCnt_offset		14
#define BPB_NumFATs_offset			16
#define BPB_FATSz32_offset			36
#define BPB_RootClus_offset			44
uint16_t BPB_BytsPerSec;	//11
uint8_t BPB_SecPerClus;		//13
uint16_t BPB_RsvdSecCnt;	//14
uint8_t BPB_NumFATs;		//16
uint32_t BPB_FATSz32;		//36
uint32_t BPB_RootClus;		//44

uint32_t root_dir_addr;
uint32_t data_file_addr;
uint32_t file_addr;
uint32_t fat_table_addr;
uint8_t sd_read_buf[512U];

void FAT32_init();
int16_t FAT32_get_file_start_cluster(char* file_name);
uint32_t FAT32_check_file_next_cluster(uint32_t current_cluster);

#endif /* INC_FAT32_H_ */
