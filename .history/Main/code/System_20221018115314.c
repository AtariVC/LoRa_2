#include "main.h"
#include "rtc.h"

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
    OneWire ow = {.uart=USART2};
    DS18B20 sensors[20] = {.ow=ow};
    DS18B20_ReadROM(&sensors[0]);
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
    // Delay(1000);
    // uint8_t gsm_init_flag = GSM_Init(&sim7000g);
    // GSM_CheckSignal(&sim7000g);
    // uint16_t vbat = GSM_GetVBAT(&sim7000g);
    // uint8_t gprs_init_flag = GSM_InitGPRS(&sim7000g);
    // if(gprs_init_flag == 0)
    //     GSM_OpenConnection(&sim7000g, "hirundo.ru", "8083");
    // if(sim7000g.ip_status==GPRS_CONNECT_OK){
    //     Delay(200);
    //     GSM_SendTCP(&sim7000g, "hello", 5);
    //     Delay(500);
    //     GSM_CloseConnections(&sim7000g);
    // }
    // else GSM_CloseConnections(&sim7000g);
    // NVIC_SystemReset();
    ADC adc = {
        .ADC = ADC1,
        .clk_devider = 1,
        .internal_channels = {
            .temp = false,
            .vbat = false,
            .vref = true
        },
        .mode = ADC_SINGLE_MODE,
        .trigger = ADC_Software_trigger,
        .ovrsmpl_ratio = 32,

    };
    ADC_Init_regular(0);
    uint16_t adc_result = 0;
    adc_single_conversion(ADC1, &adc_result);
    volatile uint32_t vrefint_cal = *(uint16_t*)0x1FFF75AA;
	volatile uint16_t battary_voltage = 3000 * (vrefint_cal) / (float)adc_result;

    fill_struct_default(&RTC_data_brief_buff, &RTC_data_full_buff);
	RTC_init();
    RTC_auto_wakeup_enable();
	// if(RTC->ISR & RTC_ISR_ALRAF){
	// 	NVIC_DisableIRQ(RTC_Alarm_IRQn);
  	// 	NVIC_ClearPendingIRQ(RTC_Alarm_IRQn);
	// 	PWR->CR1 |= PWR_CR1_DBP;
	// 	RTC->WPR = 0xCA;
	// 	RTC->WPR = 0x53;
	// 	RTC->ISR &= (uint32_t)~RTC_ISR_ALRAF;	//  flag is cleared by software by writing 0
	// 	RTC->CR &= ~(RTC_CR_ALRAIE |RTC_CR_ALRAE);   // clear
	// 	while((RTC->ISR & RTC_ISR_ALRAWF)  == 0);
	// 	// 		RTC->ALRMAR = RTC_ALRMAR_MSK4  | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK2 | bcdsecs;
	// 	// RTC->CR |= RTC_CR_ALRAE | RTC_CR_ALRAIE ;   // enable  ALARM A
	// 	RTC->WPR = 0xff; //  enable write protection
	// 	PWR->CR1 &= ~PWR_CR1_DBP;
	// }
    // RTC_auto_wakeup_enable();
	// RTC_alarm_init();
	// RTC_alarm_update(&RTC_data_full_buff);
    // GSM_SendSMS(&sim7000g, "Test_GSM_module", "+79994643543");

}
