#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "stm32l4xx.h"
#include "gpio.h"
#include "stdbool.h"


typedef enum ADC_CH_SMP_Time{
    SMP_2,
    SMP_6,
    SMP_12,
    SMP_24,
    SMP_47,
    SMP_92,
    SMP_247,
    SMP640
} ADC_CH_SMP_Time;

typedef enum ADC_CH_Group{
    ADC_CH_Injected,
    ADC_CH_Regular
} ADC_CH_Group;

typedef struct ADC_channel{
    uint8_t ch_num;
    GPIO_Pin ch;
    ADC_CH_Group group;
    ADC_CH_SMP_Time smp_time;
    uint16_t result;
} ADC_channel;

typedef enum ADC_Mode{
    ADC_SINGLE_MODE,
    ADC_CONT_MODE
} ADC_Mode;

typedef struct ADC_Internal_channels{
    bool vref;
    bool temp;
    bool vbat;
} ADC_Internal_channels;

typedef enum ADC_Trigger_type{
    ADC_Software_trigger,
    ADC_EXTI_Trigger
}
typedef enum ADC_EXTI_Polarity{
    ADC_EXTI_Rising,
    ADC_EXTI_Falling,
    ADC_EXTI_Rising_Falling
} ADC_EXTI_Polarity;

typedef struct ADC_EXTI{
    uint8_t exti_num;
    ADC_EXTI_Polarity polarity;
}

typedef struct ADC{
    ADC_TypeDef *ADC;
    ADC_channel ch[16];
    ADC_Internal_channels internal_channels;
    ADC_Mode mode;
    ADC_Trigger_type trigger;
    uint8_t ovrsmpl_ratio;
    uint8_t clk_devider;
} ADC;

// void ADC_InitGPIO(*ADC);
void ADC_Init();
void ADC_Init_regular(uint8_t ch_num);
void adc_single_conversion( ADC_TypeDef* ADCx, uint16_t* adc_data_arr ) ;

#endif /* INC_ADC_H_ */
