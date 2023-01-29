#include "adc.h"
#include "delay.h"

void ADC_reset_registers(ADC_TypeDef* ADCx){
    ADCx->CR = 0x20000000;
    ADCx->CFGR = 0x80000000;  // reset value
    ADCx->IER = 0;
    ADCx->ISR = 0;
    ADCx->CFGR2 = 0;
    ADCx->SMPR1 = 0;
    ADCx->SMPR2 = 0;
    ADCx->TR1 = 0xFF0000;
    ADCx->TR2 = 0xFF0000;
    ADCx->TR3 = 0xFF0000;
    ADCx->SQR1 = 0;
    ADCx->SQR2 = 0;
    ADCx->SQR3 = 0;
    ADCx->SQR4 = 0;
    ADCx->JSQR = 0;
    ADCx->OFR1 = 0;
    ADCx->OFR2 = 0;
    ADCx->OFR3 = 0;
    ADCx->OFR4 = 0;
}

void ADC_Init(ADC *ADC_struct){
    ADC_reset_registers(ADC_struct->ADCx);
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;  // или придется настраивать делитель PLLADC1CLK
	RCC->CCIPR &= ~RCC_CCIPR_ADCSEL;
	RCC->CCIPR |= 3 << RCC_CCIPR_ADCSEL_Pos;  // set clock source from system clock

	ADC_struct->ADCx->CR &= ~ADC_CR_DEEPPWD;  // Bring the ADC out of 'deep power-down' mode.
	ADC_struct->ADCx->CR |= ADC_CR_ADVREGEN;  // Enable the ADC voltage regulator.
	Delay(30);  // Delay for a handful of microseconds.
	ADC_struct->ADCx->CR |= ADC_CR_ADCAL;  // Calibrate the ADC.
	while (ADC_struct->ADCx->CR & ADC_CR_ADCAL);

    ADC_struct->ADCx->CFGR |= ADC_struct->mode << ADC_CFGR_CONT_Pos;  // continuous or single mode
    ADC_struct->ADCx->CFGR |= ADC_CFGR_OVRMOD;  // data will be always actual
    ADC_struct->ADCx->CFGR |= ADC_CFGR_JQDIS;  // Injected Queue disabled
    ADC_struct->ADCx->CFGR |= ADC_struct->trigger.polarity << ADC_CFGR_EXTEN_Pos;
    ADC_struct->ADCx->CFGR |= ADC_struct->trigger.exti_channel << ADC_CFGR_EXTSEL_Pos;
    ADC_struct->ADCx->CFGR |= ADC_struct->resolution << ADC_CFGR_RES_Pos;

    // enable internal channels
    if(ADC_struct->internal_channels.vref) ADC1_COMMON->CCR |= ADC_CCR_VREFEN;
    if(ADC_struct->internal_channels.vbat) ADC1_COMMON->CCR |= ADC_CCR_VBATEN;
    if(ADC_struct->internal_channels.temp) ADC1_COMMON->CCR |= ADC_CCR_TSEN;

    // enable oversampling
    if(ADC_struct->ovrsmpl_ratio != OVRSMPL_Disable){
        // ADC_struct->ADCx->CFGR2 |= ADC_CFGR2_JOVSE;
        ADC_struct->ADCx->CFGR2 |= ADC_CFGR2_ROVSE;  // Regular channels Oversampling Enable
        ADC_struct->ADCx->CFGR2 |= ADC_struct->ovrsmpl_ratio << ADC_CFGR2_OVSR_Pos;
        ADC_struct->ADCx->CFGR2 |= (ADC_struct->ovrsmpl_ratio + 1) << ADC_CFGR2_OVSS_Pos;
    }

    ADC_struct->vrefinternal_cal = *(uint16_t*)0x1FFF75AA;

    ADC_struct->ADCx->IER |= ADC_IER_EOCIE | ADC_IER_EOSIE;
    ADC_struct->ADCx->IER |= ADC_IER_JEOSIE; // | ADC_IER_JEOCIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);
}

void ADC_InitChannel(ADC *ADC_struct, uint8_t ch_num, GPIO_Pin gpio, ADC_CH_Group group, ADC_CH_SMP_Time smp_time){
	gpio_init(gpio, Input_mode, Open_drain, no_pull, Input);
    if (ch_num <= 9) {
		ADC_struct->ADCx->SMPR1 |= (smp_time << (ch_num * 3));
	} else {
		ADC_struct->ADCx->SMPR2 |= (smp_time << ((ch_num * 3) - 30));
	}
    ADC_struct->channel[ADC_struct->reg_ch_amount].ch_num = ch_num;
    ADC_struct->channel[ADC_struct->reg_ch_amount].group = group;
    ADC_struct->channel[ADC_struct->reg_ch_amount].pin = gpio;
    ADC_struct->channel[ADC_struct->reg_ch_amount].smp_time = smp_time;
    ADC_struct->reg_ch_amount++;

    if(group == ADC_CH_Regular){
        ADC_struct->ADCx->SQR1 |= ADC_struct->reg_ch_amount << ADC_SQR1_L_Pos;
        if(ch_num <= 4) ADC_struct->ADCx->SQR1 |= ch_num <<
    }
}

void ADC_Enable(ADC_TypeDef* ADCx){
    ADCx->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));
    ADCx->ISR |= ADC_ISR_ADRDY;
}
void ADC_Disable(ADC_TypeDef* ADCx){

}

void adc_single_conversion( ADC_TypeDef* ADCx, uint16_t* adc_data_arr ) {
	// Start the ADC conversion.
	//   ADCx->CR  |=  ( ADC_CR_ADSTART );
	ADC1->CR |= ADC_CR_JADSTART;
	// Wait for the 'End Of Conversion' flag.
	Delay(50);
	//  while ( !( ADCx->ISR & ADC_ISR_JEOC ) ) {};
	// Read the converted value (this also clears the EOC flag).
	adc_data_arr[0] = ADCx->JDR1;
	// adc_data_arr[1] = ADCx->JDR2;
	// adc_data_arr[2] = ADCx->JDR3;

	// Wait for the 'End Of Sequence' flag and clear it.
	Delay(50);
	//  while ( !( ADCx->ISR & ADC_ISR_EOS ) ) {};
	ADCx->ISR |= ADC_ISR_JEOS;
	// ADCx->ISR |=  ( ADC_ISR_EOS );
}
