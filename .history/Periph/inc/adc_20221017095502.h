#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"
#include "gpio.h"
// #include "stdbool.h"


typedef enum ADC_ch_smp_time{
    SMP_2,
    SMP_6,
    SMP_12,
    SMP_24,
    SMP_47,
    SMP_92,
    SMP_247,
    SMP640
} ADC_ch_smp_time;

typedef struct ADC_channel{
    GPIO_Pin ch;
    ADC_CH_Group group;
    ADC_ch_smp_time smp_time;
    uin16_t result;
} ADC_channel;

typedef enum ADC_Mode{
    ADC_SINGLE_MODE,
    ADC_CONT_MODE
} ADC_Mode;

typedef enum ADC_CH_Group{
    ADC_CH_Injected,
    ADC_CH_Regular
} ADC_CH_Group;

typedef struct ADC{
    ADC_channel ch[16];
    ADC_TypeDef *ADC;
    ADC_mode mode;
    uint8_t ovrsmpl_ratio;
} ADC;

// void ADC_InitGPIO(*ADC);
void ADC_Init();
void ADC_Init_regular(uint8_t ch_num);
void adc_single_conversion( ADC_TypeDef* ADCx, uint16_t* adc_data_arr ) ;

#endif /* INC_ADC_H_ */
