#include "main.h"


// #define static_assert(X) ({ extern int __attribute__((error("assertion failure: '" #X "' not true"))) compile_time_check(); ((X)?0:compile_time_check()),0; })


void System_Init(){
    SysTick_Config(millisec);
    // static_assert(1 == 0);
    gpio_init(LED, General_output, Push_pull, no_pull, Low_speed);
    gpio_init(SOIL_SENSOR_PWR_EN, General_output, Push_pull, no_pull, Low_speed);
    gpio_init(TEMP_SENSOR_PWR_EN, General_output, Push_pull, no_pull, Low_speed);
    gpio_state(TEMP_SENSOR_PWR_EN, HIGH);
    gpio_state(SOIL_SENSOR_PWR_EN, HIGH);

    gpio_init(UART2_TX, PA2_USART2_TX, Push_pull, pull_up, High_speed);
    OneWire ow = {USART2};
    DS18B20 sensors[20] = {0};
    // DS18B20_ReadROM(&sensors[0]);
    DS18B20_Init(sensors, &ow);
    uint8_t status = DS18B20_StartTempMeas(&ow);
    for(uint8_t i = 0; i < 6; i++)
        status += DS18B20_ReadScratchpad(&sensors[i]);
    Delay(15);

    SX126x SX1268 = {
        .gpio = {
            .MOSI_pin = LoRa_MOSI,
            .MISO_pin = LoRa_MISO,
            .SCK_pin = LoRa_SCK,
            .busy_pin = LoRa_BUSY,
            .CS_pin = LoRa_NSS,
            .DIO1_pin = LoRa_DIO1,
            .reset_pin = LoRa_NRESET,
            .__MOSI_AF_pin = PB15_SPI2_MOSI,
            .__MISO_AF_pin = PB14_SPI2_MISO,
            .__SCK_AF_pin = PB13_SPI2_SCK
        },
        .spi = SPI2,
        .bandWidth = SX126X_BW_125,
        .crcRate = SX126X_CR_4_5,
        .frequency = 434000000,
        .spredingFactor = 9, // x = SFx
        .preamble_len = 8,
        .power_dbm = 0,
        .low_data_rate_optim = 1,
        .ramping_time = SX126X_RAMP_10U,
        .packet_type = SX126X_PACKET_TYPE_LORA,
        .header_type = SX126X_HEADER_TYPE_VARIABLE_LENGTH,
        .crc_on_off = SX126X_CRC_ON,
        .iq_polarity = SX126X_STANDARD_IQ,
        .overCurrentProtection = 0x38
    };
    // SX126x_Init(&SX1268);
    // uint8_t tx_data[13] = {0, 15, 61, 22, 77, 45, 23, 65, 32, 12, 23, 124, 42};
    // SX126x_SendData(&SX1268, tx_data, 13);

    sim7000g = (GSM){
        .gpio = {
            .rx = UART3_RX,
            .tx = UART3_TX,
            .rts = uninitialized,
            .cts = uninitialized,
            .dcd = GSM_DCD,
            .ri = GSM_RI,
            .dtr = GSM_DTR,
            .pwr = GSM_PWR,
            .nreset = uninitialized,
            .__rx_af_pin = PB11_USART3_RX,
            .__tx_af_pin = PB10_USART3_TX,
            .__cts_af_pin = uninitialized,
            .__rts_af_pin = uninitialized
        },
        .uart = USART3
    };
    GSM_Init(&sim7000g);
    uint8_t flag = GSM_isAlive(&sim7000g);
    GSM_CheckIPstatus(&sim7000g);
    GSM_SetAPN(&sim7000g, "internet.tele2.ru");
    GSM_CheckIPstatus(&sim7000g);
    GSM_SendCMD(&sim7000g, "AT+CIICR");
    GSM_CheckIPstatus(&sim7000g);
}
