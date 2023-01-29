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
    GPIO_Pin pin;
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

typedef enum ADC_EXTI_Polarity{
    ADC_Software_trigger,
    ADC_EXTI_Rising,
    ADC_EXTI_Falling,
    ADC_EXTI_Rising_Falling
} ADC_EXTI_Polarity;


/// @brief
/*
External triggers for regular channels
______________________________
Name  Source       EXTSEL[3:0]
______________________________
EXT0  TIM1_CH1     0000
EXT1  TIM1_CH2     0001
EXT2  TIM1_CH3     0010
EXT3  TIM2_CH2     0011
EXT4  TIM3_TRGO(1) 0100
EXT5  -            0101
EXT6  EXTI line 11 0110
EXT9  TIM1_TRGO    1001
EXT10 TIM1_TRGO2   1010
EXT11 TIM2_TRGO    1011
EXT13 TIM6_TRGO    1101
EXT14 TIM15_TRGO   1110
======================================
External trigger for injected channels
______________________________________
Name   Source           JEXTSEL[3..0]
______________________________________
JEXT0  TIM1_TRGO        0000
JEXT1  TIM1_CH4         0001
JEXT2  TIM2_TRGO        0010
JEXT3  TIM2_CH1         0011
JEXT4  TIM3_CH4(1)      0100
JEXT5  -                0101
JEXT6  EXTI line 15     0110
JEXT8  TIM1_TRGO2       1000
JEXT14 TIM6_TRGO        1110
JEXT15 TIM15_TRGO       1111
*/
typedef struct ADC_Trigger{
    uint8_t exti_num;  // (J)EXT0-(J)EXT14
    ADC_EXTI_Polarity polarity;
} ADC_Trigger;

typedef struct ADC{
    ADC_TypeDef *ADC;
    ADC_channel regular_channels[16];
    ADC_channel injective_channels[4];
    ADC_Internal_channels internal_channels;
    ADC_Mode mode;
    ADC_Trigger trigger;
    uint8_t ovrsmpl_ratio;
    uint8_t clk_devider;
    uint8_t reg_ch_amount;  // количество опрашиваемых регулярных каналов
    uint8_t inj_ch_amount;  // количество опрашиваемых инжектированных каналов
} ADC;

// void ADC_InitGPIO(*ADC);
void ADC_Init();
void ADC_Init_regular(uint8_t ch_num);
void adc_single_conversion( ADC_TypeDef* ADCx, uint16_t* adc_data_arr ) ;

#endif /* INC_ADC_H_ */
