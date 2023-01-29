#include "main.h"
#include "rtc.h"
#include "string.h"
#include "low_power.h"

/*
Было       ID          Стало
            1
11) 0x28ff6402ff84e7ed - 1
4)  0x28ff6402f97692f3 - 2
10) 0x28ff6402e39e6e07 - 3
3)  0x28ff64021951c31e - 4
12) 0x28ff6402ffd57d31 - 5
5)  0x28ff6402f9452005 - 6
6)  0x28ff6402c59ac32b - 7
9)  0x28ff6402e38ce5d6 - 8
2)  0x28ff64021e145a8d - 9
7)  0x28ff6402c587b10a - 10
1)  0x28ff6402ec660ab3 - 11
8)  0x28ff6402eddb17f3 - 12

---------------------------
            2
9)  0x28ff6402e381d75d - 1
12) 0x28ff6402e7096f6a - 2
2)  0x28ff64021e1df76d - 3
6)  0x28ff6402196f9755 - 4
10) 0x28ff6402e3d35b94 - 5
8)  0x28ff6402ed545b21 - 6
7)  0x28ff6402c5890b9a - 7
4)  0x28ff6402192c8919 - 8
3)  0x28ff6402192ca1f8 - 9
11) 0x28ff6402e70928af - 10
1)  0x28ff6402184018ea - 11
5)  0x28ff6402196d3a96 - 12

----------------------------

            3
8)  0x28ff6402ed7b1c3d - 1
7)  0x28ff6402ed8e9e6b - 2
1)  0x28ff64021e15b81c - 3
10) 0x28ff6402e3833eb9 - 4
4)  0x28ff64021e3fa8a7 - 5
5)  0x28ff6402196f0be7 - 6
11) 0x28ff6402e70935cf - 7
9)  0x28ff6402e3889e35 - 8
12) 0x28ff6402ffde9e19 - 9
3)  0x28ff64021e1d4260 - 10
2)  0x28ff64021e15ab63 - 11
6)  0x28ff6402f92f9af4 - 12

*/

uint64_t ReverseBytes(uint64_t value)
{
  return (value & 0x00000000000000FFUL) << 56 | (value & 0x000000000000FF00UL) << 40 |
         (value & 0x0000000000FF0000UL) << 24 | (value & 0x00000000FF000000UL) << 8 |
         (value & 0x000000FF00000000UL) >> 8 | (value & 0x0000FF0000000000UL) >> 24 |
         (value & 0x00FF000000000000UL) >> 40 | (value & 0xFF00000000000000UL) >> 56;
}
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
    // uint64_t ser_nums[12] = {0};
    // if(DS18B20_Init(sensors, &ow)){
    //     uint8_t status = DS18B20_StartTempMeas(&ow);
    //     for(uint8_t i = 0; i < 12; i++){
    //         status += DS18B20_ReadScratchpad(&sensors[i]);
    //         ser_nums[i] = ReverseBytes(*(uint64_t *)(sensors[i].serialNumber));
    //     }
    // }
    OneWireStatus status;
    DS18B20_StartTempMeas(&ow);
    for(uint8_t i = 0; i < 12; i++){
        if(SELF_ID == 1){
            uint64_t data = ReverseBytes(gs1_ow_nums[i]);
            status = OneWire_MatchRom(&ow, (RomCode*)(&data));
        } else if(SELF_ID == 2){
            uint64_t data = ReverseBytes(gs2_ow_nums[i]);
            status = OneWire_MatchRom(&ow, (RomCode*)(&data));
        } else if(SELF_ID == 3){
            uint64_t data = ReverseBytes(gs3_ow_nums[i]);
            status = OneWire_MatchRom(&ow, (RomCode*)(&data));
        }
        if(status == ONE_WIRE_OK){
            OneWire_Write(&ow, DS18B20_READ_SCRATCHPAD);
            OneWire_ReadArray(&ow, (uint8_t *)(&(sensors[i].scratchpad)), 9);
            sensors[i].temperature = (uint32_t)(sensors[i].scratchpad.temperature) * 0.0625;
        }
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
        .self_addr = SELF_ID,
        .bandWidth = SX126X_BW_125,
        .crcRate = SX126X_CR_4_5,
        .frequency = 430000000,
        .spredingFactor = 8, // x = SFx
        .preamble_len = 8,
        .power_dbm = 0x0E,
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
    uint8_t init_status = RTC_Init();
    if(init_status) {  // ship mode
        write_single_bkp_reg(5, 0x55);
    //     // SX126x_SendData(&SX1268, (uint8_t*)"Start", 5);
    //     RTC_auto_wakeup_enable(350);
    //     Delay(10000);
    //     stop_cortex();
    }
    uint32_t data = RTC->BKP4R;
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
        gpio_init(SOIL_SENSOR_PWR_EN, Input_mode, Open_drain, pull_down, Input);
        gpio_init(TEMP_SENSOR_PWR_EN, Input_mode, Open_drain, pull_down, Input);
        gpio_init(UART2_TX, Input_mode, Open_drain, pull_down, Input);
        SX126x_SendData(&SX1268, gs_packet, 53);

        RTC_auto_wakeup_enable(600);
        Delay(2000);
        SX126x_SetSleep(&SX1268);
        stop_cortex();
    }
}


