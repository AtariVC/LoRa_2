#include "main.h"
// #include "sdio.h"
// #include "fat32.h"
#include "low_power.h"
#include "string.h"
#include "System.h"


volatile time_t current_time;
void LED_BLINK(){
	gpio_toggle(LED);
	// Delay(1000);
}
uint64_t ReverseBytes(uint64_t value)
{
  return (value & 0x00000000000000FFUL) << 56 | (value & 0x000000000000FF00UL) << 40 |
         (value & 0x0000000000FF0000UL) << 24 | (value & 0x00000000FF000000UL) << 8 |
         (value & 0x000000FF00000000UL) >> 8 | (value & 0x0000FF0000000000UL) >> 24 |
         (value & 0x00FF000000000000UL) >> 40 | (value & 0xFF00000000000000UL) >> 56;
}
void SEND_TO_SERVER(){
    //
    if(SX1268.self_addr == 1){
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
        NVIC_DisableIRQ(EXTI9_5_IRQn);
        uint8_t gsm_init_flag = GSM_Init(&sim7000g);
        if(buffer_ptr + gs2_buf_ptr < sizeof(data_buffer)){
            uint8_t data[3] = {0x02, gs2_buf_ptr >> 8, gs2_buf_ptr & 0xFF};
            memcpy(data_buffer + buffer_ptr, (uint8_t *)(&data), 3);
            buffer_ptr += 2;
            memcpy(data_buffer + buffer_ptr, (uint8_t *)(&gs2_buffer), gs2_buf_ptr);
            buffer_ptr += gs2_buf_ptr;
        }

        if(buffer_ptr + gs3_buf_ptr < sizeof(data_buffer)){
            uint8_t data[3] = {0x03, gs3_buf_ptr >> 8, gs3_buf_ptr & 0xFF};
            memcpy(data_buffer + buffer_ptr, (uint8_t *)(&data), 3);
            buffer_ptr += 2;
            memcpy(data_buffer + buffer_ptr, (uint8_t *)(&gs3_buffer), gs3_buf_ptr);
            buffer_ptr += gs3_buf_ptr;
        }


        GSM_CheckSignal(&sim7000g);
        uint16_t gsm_vbat = GSM_GetVBAT(&sim7000g);
        if(buffer_ptr + 4 < sizeof(data_buffer)){
            memcpy(data_buffer + buffer_ptr, (uint8_t *)(&gsm_vbat), 2);
            memcpy(data_buffer + buffer_ptr + 2, (uint8_t *)(&sim7000g.signal_level), 1);
        }
        uint8_t gprs_init_flag = GSM_InitGPRS(&sim7000g);
        if(gprs_init_flag == 0)
            GSM_OpenConnection(&sim7000g, "hirundo.ru", "8083");
        uint16_t timeout = 1500;
        if(sim7000g.ip_status==GPRS_CONNECT_OK){
            Delay(300);
            // мб стоит ожидать появления символа '>'
            // GSM_SendTCP(&sim7000g, "hello", 5);
            GSM_SendTCP(&sim7000g, (char*)data_buffer, buffer_ptr);
            while(!sim7000g.status.tcp_server_answer && timeout > 0){
                Delay(1);
                timeout--;
            }
            if(timeout == 0){
                GSM_CloseConnections(&sim7000g);
                Delay(500);
                gprs_init_flag = GSM_InitGPRS(&sim7000g);
                Delay(500);
                GSM_OpenConnection(&sim7000g, "hirundo.ru", "8083");
                Delay(500);
                GSM_SendTCP(&sim7000g, (char*)data_buffer, buffer_ptr);
                // GSM_SendTCP(&sim7000g, "hello", 5);
                Delay(500);
            }
            GSM_CloseConnections(&sim7000g);
        }
        else GSM_CloseConnections(&sim7000g);
        GSM_PowerOFF(&sim7000g);
        buffer_ptr = 0;
        gs2_buf_ptr = 0;
        gs3_buf_ptr = 0;
        // EXTI->PR1 |= EXTI_PR1_PIF6;
        NVIC_EnableIRQ(EXTI9_5_IRQn);
    }
}
void MakeMeasure(){
    for(uint8_t i = 0; i < 11; i++){
        adc.reg_channel_queue[i].result = 0;
        sensors[i].scratchpad.temperature = 0;
    }
    sensors[11].scratchpad.temperature = 0;

    gpio_state(TEMP_SENSOR_PWR_EN, HIGH);
    gpio_state(SOIL_SENSOR_PWR_EN, HIGH);
    Delay(100);
    ADC_Start(&adc);


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


    while(adc.measure_process);
    gpio_state(TEMP_SENSOR_PWR_EN, LOW);
    gpio_state(SOIL_SENSOR_PWR_EN, LOW);
    uint16_t measured_data[24] = {0};
    // [control_word, self_id, target_id, cmd, [datetime[], adc_data[], temp_data[], vbat, gsm_vbat],]
    //       1          1         1       1       7           11*2         12*2      1*2     1*2
    for(uint8_t i = 0; i < 12; i++){
        measured_data[i] = adc.reg_channel_queue[i].result;
        measured_data[i + 12] = sensors[i].scratchpad.temperature;
    }
    measured_data[11] = adc.vdda_mvolt;

    if(buffer_ptr + sizeof(current_rtc) + sizeof(measured_data) < sizeof(data_buffer)){
        RTC_get_time(&current_rtc);
        memcpy(data_buffer + buffer_ptr, (uint8_t *)(&current_rtc), sizeof(current_rtc));
        buffer_ptr += sizeof(current_rtc);
        memcpy(data_buffer + buffer_ptr, (uint8_t *)(&measured_data), sizeof(measured_data));
        buffer_ptr += sizeof(measured_data);
    }
}
int main(){
	// init_global_variables();
    SCB->CPACR |= 0x0F << 20;
    System_Init();
    buffer_ptr = 0;
    memset(data_buffer, 0, 256);
    while(1){
	// delay_action(5000, 0, LED_BLINK);
        if(SX1268.new_rx_data_flag){
            SX126x_RxDataParse(&SX1268);
        }

		delay_action(30000, 1, SEND_TO_SERVER);
        delay_action(6000, 2, MakeMeasure);

   	// delay_action(15, 1, LCD_PINBALL);
	// delay_action(500, 2, UART_SEND);
	// delay_action(100, 3, SEND_SENSORS_DATA);
	}
}


