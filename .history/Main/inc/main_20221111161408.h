#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include "stm32l4xx.h"
#include <stdio.h>
#include "formating_output.h"
#include "delay.h"
#include "gpio.h"
#include "rcc.h"
#include "pwm.h"
#include "uart.h"
#include "spi.h"
#include "dwt.h"
#include "i2c.h"
#include "tmp1075.h"
#include "adc.h"
#include "global_variables.h"
#include "periph_handlers.h"
#include "tim.h"
#include "fifo.h"
#include "sx1278.h"
#include "rtc.h"
#include "one_wire.h"
#include "ds18b20.h"
#include "sx126x.h"
#include "gsm.h"
#include "cli.h"

// #define SCREEN_PLUGGED
// #define SENSORS_PLUGGED

#define LED 		PA9		//DIO0 TX_EN 868

#define UART2_RX    PA3
#define UART2_TX    PA2

#define UART3_RX	PB11
#define UART3_TX	PB10

#define I2C1_SDA	PB9
#define I2C1_SCL	PB8


#define TEMP_SENSOR_PWR_EN    PC7
#define SOIL_SENSOR_PWR_EN    PA8

#define LoRa_SPI          SPI2
#define LoRa_BUSY         PB12
#define LoRa_SCK          PB13
#define LoRa_MISO         PB14
#define LoRa_MOSI         PB15
#define LoRa_NSS          PC6
#define LoRa_DIO1         PB6
#define LoRa_DIO2         PB5
#define LoRa_DIO3         PB4
#define LoRa_NRESET       PA15

#define GSM_RX          PB11
#define GSM_TX          PB10
#define GSM_CTS         PA6
#define GSM_RTS         PB1
#define GSM_DCD         PA10
#define GSM_RI          PA11
#define GSM_DTR         PA12
#define GSM_PWR         PC13
#define GSM_NRESET      PB7


#define SD_D0				PC9
#define SD_D1				PC8
#define SD_D2				PC10
#define SD_D3				PC11
#define SD_CK				PC12
#define SD_CMD				PD2


#define SELF_ID        1
//-----------------------//
GSM sim7000g;
SX126x SX1268;
ADC adc;
DS18B20 sensors[12];
OneWire ow;
uint8_t data_buffer[768];
uint16_t buffer_ptr;
uint8_t gs2_buffer[256];
uint8_t gs2_buf_ptr;
uint8_t gs3_buffer[256];
uint8_t gs3_buf_ptr;
RTC_struct_brief current_rtc;

void (*handler)();
uint64_t gs1_ow_nums[12] = {
    0x28ff6402ff84e7ed,
    0x28ff6402f97692f3,
    0x28ff6402e39e6e07,
    0x28ff64021951c31e,
    0x28ff6402ffd57d31,
    0x28ff6402f9452005,
    0x28ff6402c59ac32b,
    0x28ff6402e38ce5d6,
    0x28ff64021e145a8d,
    0x28ff6402c587b10a,
    0x28ff6402ec660ab3,
    0x28ff6402eddb17f3
};
uint64_t gs2_ow_nums[12] = {
    0x28ff6402e381d75d,
    0x28ff6402e7096f6a,
    0x28ff64021e1df76d,
    0x28ff6402196f9755,
    0x28ff6402e3d35b94,
    0x28ff6402ed545b21,
    0x28ff6402c5890b9a,
    0x28ff6402192c8919,
    0x28ff6402192ca1f8,
    0x28ff6402e70928af,
    0x28ff6402184018ea,
    0x28ff6402196d3a96
};
uint64_t gs3_ow_nums[12] = {
    0x28ff6402ed7b1c3d,
    0x28ff6402ed8e9e6b,
    0x28ff64021e15b81c,
    0x28ff6402e3833eb9,
    0x28ff64021e3fa8a7,
    0x28ff6402196f0be7,
    0x28ff6402e70935cf,
    0x28ff6402e3889e35,
    0x28ff6402ffde9e19,
    0x28ff64021e1d4260,
    0x28ff64021e15ab63,
    0x28ff6402f92f9af4
};

#endif /* INC_MAIN_H_ */