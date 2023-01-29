/*
 * fat32.c
 *
 *  Created on: 7 ���. 2020 �.
 *      Author: Gandalf
 */

#include "fat32.h"
#include "sdio.h"
void FAT32_init(){
	uint8_t write_buf[512];
	sd_last_result = SD_ReadBlock(0, (uint32_t *)sd_read_buf, 512);
	if(sd_read_buf[0] == 0 && sd_read_buf[1] == 0 && sd_read_buf[2] == 0 && SDCard.Type == SDCT_SDHC){
		if(sd_read_buf[0x1C7] != 0){
			uint16_t boot_addr = (sd_read_buf[0x1C7] << 8 | sd_read_buf[0x1C6]);
			sd_last_result = SD_ReadBlock(boot_addr * 512, (uint32_t *)sd_read_buf, 512);
			BPB_BytsPerSec = (uint16_t)sd_read_buf[11] + ((uint16_t)sd_read_buf[12] << 8);
			BPB_SecPerClus = sd_read_buf[13];
			BPB_RsvdSecCnt = (uint16_t)sd_read_buf[14] + ((uint16_t)sd_read_buf[15] << 8);
			BPB_NumFATs = sd_read_buf[16];
			BPB_FATSz32 = (uint32_t)sd_read_buf[36] + ((uint32_t)sd_read_buf[37] << 8) + ((uint32_t)sd_read_buf[38] << 16) + ((uint32_t)sd_read_buf[39] << 24);
			BPB_RootClus = (uint32_t)sd_read_buf[44] + ((uint32_t)sd_read_buf[45] << 8) + ((uint32_t)sd_read_buf[46] << 16) + ((uint32_t)sd_read_buf[47] << 24);
			root_dir_addr = boot_addr + BPB_FATSz32 * BPB_NumFATs + BPB_RsvdSecCnt;
			fat_table_addr = boot_addr + BPB_RsvdSecCnt;
			sd_last_result = SD_ReadBlock(root_dir_addr * 512, (uint32_t *)sd_read_buf, 512);

			int16_t file_start_cluster = FAT32_get_file_start_cluster("DATA    .TXT");
			if(file_start_cluster != -1){
				sd_last_result = SD_ReadBlock(fat_table_addr * 512, (uint32_t *)sd_read_buf, 512);
				file_addr = BPB_SecPerClus * (file_start_cluster - 2) + root_dir_addr;
				sd_last_result = SD_ReadBlock(file_addr * 512, (uint32_t *)sd_read_buf, 512);
				for(uint16_t i = 0; i < 512; i++){
					write_buf[i] = i;
				}
				for(uint16_t i = 0; i < 16; i++){
					sd_last_result = SD_WriteBlock(file_addr * 512 + i * 512, (uint32_t *)write_buf, 512);
				}
			}
		}
		else{
			//some shit
		}
	}
}

int16_t FAT32_get_file_start_cluster(char* file_name){ //file name in format 8.3
	for(uint16_t i = 0x70; i < 502; i += 16){
		if((sd_read_buf[i] == file_name[0]) && (sd_read_buf[i+1] == file_name[1]) &&
		   (sd_read_buf[i+2] == file_name[2]) && (sd_read_buf[i+3] == file_name[3]) &&
		   (sd_read_buf[i+4] == file_name[4]) && (sd_read_buf[i+5] == file_name[5]) &&
		   (sd_read_buf[i+6] == file_name[6]) && (sd_read_buf[i+7] == file_name[7]) &&
		   (sd_read_buf[i+8] == file_name[9]) && (sd_read_buf[i+9] == file_name[10]) &&
		   (sd_read_buf[i+10] == file_name[11])){
			return sd_read_buf[i + 0x1A];
		}
	}
	return -1;
}

uint32_t FAT32_check_file_next_cluster(uint32_t current_cluster){
	return (uint32_t)sd_read_buf[BPB_RsvdSecCnt + current_cluster * 4];
}
