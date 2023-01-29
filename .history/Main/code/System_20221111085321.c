#include "main.h"
#include "rtc.h"
#include "string.h"
#include "low_power.h"
/*
0x200006f8 <ow+84>
*/

/*
0x2000bf44      12      100cm
0x2000bf24      11      90cm
0x2000bf74      10      80cm
0x2000bf34      9       70cm


*/

void System_Init(){
    SysTick_Config(millisec);
    gpio_init(LED, General_output, Push_pull, no_pull, Low_speed);
    gpio_init(SOIL_SENSOR_PWR_EN, General_output, Push_pull, no_pull, Low_speed);
    gpio_init(TEMP_SENSOR_PWR_EN, General_output, Push_pull, no_pull, Low_speed);
    gpio_state(TEMP_SENSOR_PWR_EN, HIGH);
    gpio_state(SOIL_SENSOR_PWR_EN, HIGH);

    gpio_init(UART2_TX, PA2_USART2_TX, Push_pull, pull_up, High_speed);

    ow = (OneWire){.uart=USART2};
    for(uint8_t i = 0; i < 12; i++){
        sensors[i] = (DS18B20){0};
        sensors[i].ow = &ow;
    }
    // DS18B20_ReadROM(sensors);
    uint64_t ser_nums[12] = {0};
    if(DS18B20_Init(sensors, &ow)){
        uint8_t status = DS18B20_StartTempMeas(&ow);
        for(uint8_t i = 0; i < 12; i++)
            status += DS18B20_ReadScratchpad(&sensors[i]);
            ser_nums[i] = (uint64_t *)&sensors[i].
    }
    while(1){
        Delay(500);
        uint8_t status = DS18B20_StartTempMeas(&ow);
        for(uint8_t i = 0; i < 12; i++)
            status += DS18B20_ReadScratchpad(&sensors[i]);
    }
    // GSM_SetPowerState(&sim7000g, LOW);
    // NVIC_SystemReset();
    // GSM_SendSMS(&sim7000g, "Test_GSM_module", "+79994643543");
    adc = (ADC){
        .ADCx = ADC1,
        .clk_devider = ADC_ClockDevider_1,
        .internal_channels = {
            .temp = false,
            .vbat = false,
            .vref = true
        },
        .resolution = ADC_12bit,
        .mode = ADC_SINGLE_MODE,
        .trigger.polarity = ADC_Software_trigger,
        .ovrsmpl_ratio = OVRSMPL_32x
    };
    ADC_Init(&adc);
    ADC_InitRegChannel(&adc, CH12, PA7, SMP_92);
    ADC_InitRegChannel(&adc, CH8, PA3, SMP_92);
    ADC_InitRegChannel(&adc, CH13, PC4, SMP_92);
    ADC_InitRegChannel(&adc, CH15, PB0, SMP_92);
    ADC_InitRegChannel(&adc, CH14, PC5, SMP_92);
    ADC_InitRegChannel(&adc, CH9, PA4, SMP_92);
    ADC_InitRegChannel(&adc, CH10, PA5, SMP_92);
    ADC_InitRegChannel(&adc, CH6, PA1, SMP_92);
    ADC_InitRegChannel(&adc, CH4, PC3, SMP_92);
    ADC_InitRegChannel(&adc, CH2, PC1, SMP_92);
    ADC_InitRegChannel(&adc, CH5, PA0, SMP_92);
    ADC_InitRegChannel(&adc, VREF, uninitialized, SMP_92);
    adc.ADCx->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));
    adc.ADCx->ISR |= ADC_ISR_ADRDY;
    ADC_Start(&adc);
    while(adc.measure_process);
    gpio_state(TEMP_SENSOR_PWR_EN, LOW);
    gpio_state(SOIL_SENSOR_PWR_EN, LOW);


    SX1268 = (SX126x){
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
        .self_addr = 1,
        .bandWidth = SX126X_BW_125,
        .crcRate = SX126X_CR_4_5,
        .frequency = 430000000,
        .spredingFactor = 8, // x = SFx
        .preamble_len = 8,
        .power_dbm = 0xF7,
        .low_data_rate_optim = 0,
        .ramping_time = SX126X_RAMP_80U,
        .packet_type = SX126X_PACKET_TYPE_LORA,
        .header_type = SX126X_HEADER_TYPE_VARIABLE_LENGTH,
        .crc_on_off = SX126X_CRC_ON,
        .iq_polarity = SX126X_STANDARD_IQ,
        .overCurrentProtection = 0x38,
        .sync_word = 0x1424 //0x1424
    };
    SX126x_Init(&SX1268);
    RTC_Init();

    if(SX1268.self_addr != 1){
        uint16_t measured_data[24] = {0};
        uint8_t gs_packet[53] = {0};
        gs_packet[0] = 0x55;
        gs_packet[1] = SX1268.self_addr;
        gs_packet[2] = 0x01;
        gs_packet[3] = MY_PERIODIC_DATA;
        gs_packet[4] = sizeof(measured_data);
        // [control_word, self_id, target_id, cmd, [datetime[], adc_data[], temp_data[], vbat, gsm_vbat],]
        //       1          1         1       1       7           11*2         12*2      1*2     1*2
        for(uint8_t i = 0; i < 12; i++){
            measured_data[i] = adc.reg_channel_queue[i].result;
            measured_data[i + 12] = sensors[i].scratchpad.temperature;
        }
        measured_data[11] = adc.vdda_mvolt;
        memcpy(gs_packet + 5, measured_data, sizeof(measured_data));
        gpio_state(TEMP_SENSOR_PWR_EN, LOW);
        gpio_state(SOIL_SENSOR_PWR_EN, LOW);
        SX126x_SendData(&SX1268, gs_packet, 53);

        RTC_auto_wakeup_enable(5);
        Delay(2000);
        stop_cortex();
    } else{
        SX126x_SendData(&SX1268, (uint8_t*)"Start", 5);
    }
}

void EXTI9_5_IRQHandler(void){
    EXTI->PR1 |= EXTI_PR1_PIF6;
    SX126x_RxHandler(&SX1268);

}
